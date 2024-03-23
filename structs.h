#ifndef STRUCTS_H
#define STRUCTS_H

#include <vector>
#include <string>

struct DirEntry {
  int64_t size;
  int sectors;
  int64_t mtime;
  int key;
  std::string name;
  bool operator<(const DirEntry& other) const {
      return name < other.name;
  }
  static int64_t timeNanosecToWindows(timespec &unixTimeSpec) {
      return 0;
  }
  bool mayBeListed() {
      auto pos = name.rfind('.');
      if (pos==std::string::npos) return false;
      std::string ext = name.substr(pos);
      if (ext==".zip")
          return true;
      else
          return false;
  }
};

struct DirContent {
  std::vector<DirEntry> dirs;
  std::vector<DirEntry> archives;
  std::vector<DirEntry> files;
};

#endif //STRUCTS_H
