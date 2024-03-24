#ifdef _WIN32
 #include <windows.h>
 #include "UTF.hpp"
#else
 #include <dirent.h>
 #include <sys/stat.h>
 #include <fcntl.h>
 #include <sys/statvfs.h>
#endif
#include <algorithm>
#include <cstring>
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
    string canonical = fs::canonical(dirPath).string();
    cout << canonical << endl;
#ifdef _WIN32
    TCHAR szDrive[4];
    szDrive[0] = canonical[0];
    szDrive[1] = ':';
    szDrive[2] = '\\';
    szDrive[3] = 0;
    DWORD dwSectorsPerCluster;
    DWORD dwBytesPerSector;
    DWORD dwFreeClusters;
    DWORD dwTotalClusters;
    GetDiskFreeSpace(szDrive, &dwSectorsPerCluster, &dwBytesPerSector, &dwFreeClusters, &dwTotalClusters);
    statfs.blkSize = dwBytesPerSector;
    statfs.clusterSize = dwBytesPerSector * dwSectorsPerCluster;
    statfs.toatalClusters = dwTotalClusters;
    statfs.freeClusters = dwFreeClusters;
#else
    struct statvfs fsInfo;
    statvfs(dirPath.c_str(), &fsInfo);
    statfs.blkSize = S_BLKSIZE;
    statfs.clusterSize = fsInfo.f_frsize;
    statfs.toatalClusters = fsInfo.f_blocks;
    statfs.freeClusters = fsInfo.f_bfree;
#endif
    auto h = readDirRecur(canonical);
    dbWriter.addRoot(h);
    dbWriter.close();
}

uint64_t DirectoryReader::readDirRecur(const std::string &dirPath) {
    auto content = readDir(dirPath);
    for (auto &entry : content.entries)
        if ((entry.attr & DirEntry::ATTR_IS_DIR) != 0)
            entry.key = readDirRecur(dirPath + "/" + entry.name);
    XXHash64 hash(0);
    uint64_t h = hash.hash(dirPath.c_str(), dirPath.size(), 0);
    dbWriter.addContent(h, content);
    return h;
}

#ifdef _WIN32
DirContent DirectoryReader::readDir(const std::string &dirPath) {
    DirContent dirContent;
    UTF utf;
    auto path = utf.u8to16(dirPath + "\\*");
    WIN32_FIND_DATAW findFileData;
    HANDLE hFind = FindFirstFileW(reinterpret_cast<LPCWSTR>(path.c_str()), &findFileData);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            DirEntry entry;
            entry.name = utf.u16to8((const char16_t *)findFileData.cFileName);
            entry.size = findFileData.nFileSizeLow | ((uint64_t)findFileData.nFileSizeHigh << 32);
            entry.mtime = findFileData.ftLastWriteTime.dwLowDateTime | ((uint64_t)findFileData.ftLastWriteTime.dwHighDateTime << 32);
            int64_t clusters = (entry.size + statfs.clusterSize -1)/statfs.clusterSize;
            entry.sectors = clusters/(statfs.clusterSize/statfs.blkSize);
            if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                entry.attr |= DirEntry::ATTR_IS_DIR;
            dirContent.entries.push_back(entry);
        } while (FindNextFileW(hFind, &findFileData) != 0);
        FindClose(hFind);
    } else {
        perror("");
        return {};
    }
    return dirContent;
}
#else
DirContent DirectoryReader::readDir(const std::string &dirPath) {
    static int counter = 0;
    counter++;
    printf("%d\n",counter);
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
                    dirContent.entries.push_back(entry);
                }
            }
        }
        closedir(dir);
    } else {
        perror("");
        return {};
    }
    sort(dirContent.entries.begin(), dirContent.entries.end());
    return dirContent;
}
#endif