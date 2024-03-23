#ifndef DIRENTRY_H
#define DIRENTRY_H

#include <vector>
#include <string>

struct DirEntry {
  int64_t size;
  int sectors;
  int64_t mtime;
  uint16_t attr = 0;
  int key = 0;
  std::string name;
  char *serialized = nullptr;
  int serialLen = 0;
  ~DirEntry() {delete serialized;}
  static const uint16_t ATTR_IS_DIR = 1;
  bool operator<(const DirEntry& other) const;
  static int64_t timeNanosecToWindows(timespec &unixTimeSpec);
  bool mayBeListed() const;
  int bufferSize();
  void serialize();
};

using DirContent = std::vector<DirEntry>;


#endif //DIRENTRY_H
