#include <dirent.h>
#include <sys/stat.h>
#include <algorithm>
#include <cstring>
#include <fcntl.h>
#include <filesystem>
#include <iostream>
#include "xxhash64.h"
#include "DirectoryReader.h"

namespace fs = std::filesystem;
using namespace std;

DirectoryReader::DirectoryReader(AbstractDBWriter &dbWriter) : dbWriter(dbWriter) {
}

void DirectoryReader::readAndStore(const std::string &dirPath) {
    dbWriter.open();
    string canonical = fs::canonical(dirPath);
    cout << canonical << endl;
    auto h = readDirRecur(canonical);
    dbWriter.addRoot(h);
    dbWriter.close();
}

uint64_t DirectoryReader::readDirRecur(const std::string &dirPath) {
    auto content = readDir(dirPath);
    for (auto &entry : content)
        if ((entry.attr & DirEntry::ATTR_IS_DIR) != 0)
            entry.key = readDirRecur(dirPath + "/" + entry.name);
    XXHash64 hash(0);
    uint64_t h = hash.hash(dirPath.c_str(), dirPath.size(), 0);
    dbWriter.addContent(h, content);
    return h;
}

DirContent DirectoryReader::readDir(const std::string &dirPath) {
    DIR *dir;
    struct dirent *ent;
    struct stat file_stat{};
    DirContent dirContent;

    if ((dir = opendir(dirPath.c_str())) != nullptr) {
        int dir_fd = dirfd(dir);
        while ((ent = readdir(dir)) != nullptr) {
            if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {
                if (fstatat(dir_fd, ent->d_name, &file_stat, AT_SYMLINK_NOFOLLOW) == 0) {
                    DirEntry entry;
                    entry.name = ent->d_name;
                    entry.size = file_stat.st_size;
                    entry.mtime = DirEntry::timeNanosecToWindows(file_stat.st_mtim);
                    entry.sectors = file_stat.st_blocks;
                    if (S_ISDIR(file_stat.st_mode))
                        entry.attr |= DirEntry::ATTR_IS_DIR;
                    dirContent.push_back(entry);
                }
            }
        }
        closedir(dir);
    } else {
        perror("");
        return {};
    }
    sort(dirContent.begin(), dirContent.end());
    return dirContent;
}
