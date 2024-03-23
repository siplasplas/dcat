#include <dirent.h>
#include <sys/stat.h>
#include <algorithm>
#include <cstring>
#include <fcntl.h>
#include "DirectoryReader.h"

DirectoryReader::DirectoryReader(AbstractDBWriter *dbWriter):dbWriter(dbWriter) {
}

void DirectoryReader::readAndStore(const std::string &dirPath) {
    readDirRecur(dirPath);
}

void DirectoryReader::readDirRecur(const std::string &dirPath) {
    auto content = readDir(dirPath);
    for (auto &dname : content.dirs)
        readDirRecur(dirPath + "/" + dname);
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
                    if (S_ISDIR(file_stat.st_mode))
                        dirContent.dirs.emplace_back(ent->d_name);
                    else
                        dirContent.files.emplace_back(ent->d_name);
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
