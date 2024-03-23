#ifndef STRUCTS_H
#define STRUCTS_H

#include <vector>
#include <string>

struct DirEntry {
  int64_t size;
  int sectors;
  int64_t mtime;
  uint16_t attr = 0;
  int key = 0;
  std::string name;
  static const uint16_t ATTR_IS_DIR = 1;
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

using DirContent = std::vector<DirEntry>;


#endif //STRUCTS_H
