#include <dirent.h>
#include <sys/stat.h>
#include <algorithm>
#include <cstring>
#include <fcntl.h>
#include "xxhash64.h"
#include "DirectoryReader.h"

DirectoryReader::DirectoryReader(AbstractDBWriter &dbWriter):dbWriter(dbWriter) {
}

void DirectoryReader::readAndStore(const std::string &dirPath) {
    readDirRecur(dirPath);
}

uint64_t DirectoryReader::readDirRecur(const std::string &dirPath) {
    auto content = readDir(dirPath);
    for (auto &entry : content.dirs)
        readDirRecur(dirPath + "/" + entry.name);
    XXHash64 hash(0);
    return hash.hash(dirPath.c_str(), dirPath.size(), 0);
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
                    entry.key = 0;
                    entry.mtime = DirEntry::timeNanosecToWindows(file_stat.st_mtim);
                    entry.sectors = file_stat.st_blocks;
                    if (S_ISDIR(file_stat.st_mode))
                        dirContent.dirs.emplace_back(entry);
                    else if (entry.mayBeListed())
                        dirContent.archives.emplace_back(entry);
                    else
                        dirContent.files.emplace_back(entry);
                }
            }
        }
        closedir(dir);
    } else {
        perror("");
        return {};
    }
    sort(dirContent.dirs.begin(), dirContent.dirs.end());
    return dirContent;
}
