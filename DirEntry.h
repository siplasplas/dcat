#ifndef DIRENTRY_H
#define DIRENTRY_H

#include <vector>
#include <string>
#include <ctime>

struct DirEntry {
  uint64_t size = 0;
  uint64_t sectors = 0;
  uint64_t mtime = 0;
  uint64_t attr = 0;
  uint64_t key = 0;
  std::string name;
  char *serialized = nullptr;
  int serialLen = 0;
  ~DirEntry() { delete serialized; }
  static const uint16_t ATTR_IS_DIR = 1;
  bool operator<(const DirEntry &other) const;
  static int64_t timeNanosecToWindows(timespec &unixTimeSpec);
  bool mayBeListed() const;
  size_t bufferSize() const;
  void serialize();
  const char *deserialize(const char *s);
  bool operator==(const DirEntry &other) const {
      return size == other.size && sectors == other.sectors
          && mtime == other.mtime && attr == other.attr && key == other.key
          && name == other.name;
  };
};

struct DirContent {
  ~DirContent() {delete serialized;}
  char *serialized = nullptr;
  int serialLen = 0;
  std::vector<DirEntry> entries;
  void serialize();
  void deserialize(std::string_view value);
};

#endif //DIRENTRY_H
