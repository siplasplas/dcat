#ifndef ABSTRACTDBWRITER_H
#define ABSTRACTDBWRITER_H

#include "DirEntry.h"

class AbstractDBWriter {
public:
  virtual ~AbstractDBWriter() = default;
  virtual void addContent(uint64_t h, DirContent &content) = 0;
  virtual void addRoot(uint64_t h) = 0;
  virtual void open() = 0;
  virtual void close() = 0;
};

#endif //ABSTRACTDBWRITER_H
