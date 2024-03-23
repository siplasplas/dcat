#ifndef DIRENTRY_H
#define DIRENTRY_H

#include <vector>
#include <string>

struct DirEntry {
  int64_t size = 0;
  int sectors = 0;
  int64_t mtime = 0;
  uint16_t attr = 0;
  int key = 0;
  std::string name;
  char *serialized = nullptr;
  int serialLen = 0;
  ~DirEntry() { delete serialized; }
  static const uint16_t ATTR_IS_DIR = 1;
  bool operator<(const DirEntry &other) const;
  static int64_t timeNanosecToWindows(timespec &unixTimeSpec);
  bool mayBeListed() const;
  int bufferSize();
  void serialize();
  const char *deserialize(const char *s);
  bool operator==(const DirEntry &other) const {
      return size == other.size && sectors == other.sectors
          && mtime == other.mtime && attr == other.attr && key == other.key
          && name == other.name;
  };
};

struct DirContent {
  std::vector<DirEntry> entries;
  void deserialize(std::string_view value);
};

#endif //DIRENTRY_H
