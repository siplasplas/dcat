#ifndef DIRECTORYREADER_H
#define DIRECTORYREADER_H

#include <string>
#include <vector>
#include "AbstractDBWriter.h"

struct StatFS {
    unsigned int blkSize;
    unsigned int clusterSize;
    long toatalClusters;
    long freeClusters;
};

class DirectoryReader {
  StatFS statfs;
  AbstractDBWriter &dbWriter;
  DirContent readDir(const std::string &dirPath);
  uint64_t readDirRecur(const std::string &dirPath);
public:
  explicit DirectoryReader(AbstractDBWriter &dbWriter);
  void readAndStore(const std::string &dirPath);
  void rebuild();
};

#endif //DIRECTORYREADER_H
