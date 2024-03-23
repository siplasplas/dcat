#ifndef DIRECTORYREADER_H
#define DIRECTORYREADER_H

#include <string>
#include <vector>
#include "AbstractDBWriter.h"

struct DirContent {
  std::vector<std::string> dirs;
  std::vector<std::string> files;
};


class DirectoryReader {
  AbstractDBWriter &dbWriter;
  DirContent readDir(const std::string &dirPath);
  void readDirRecur(const std::string &dirPath);
public:
  explicit DirectoryReader(AbstractDBWriter &dbWriter);
  void readAndStore(const std::string &dirPath);
};

#endif //DIRECTORYREADER_H
