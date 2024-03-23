#ifndef DIRECTORYREADER_H
#define DIRECTORYREADER_H

#include <string>
#include <vector>
#include "AbstractDBWriter.h"

class DirectoryReader {
  AbstractDBWriter &dbWriter;
  DirContent readDir(const std::string &dirPath);
  uint64_t readDirRecur(const std::string &dirPath);
public:
  explicit DirectoryReader(AbstractDBWriter &dbWriter);
  void readAndStore(const std::string &dirPath);
};

#endif //DIRECTORYREADER_H
