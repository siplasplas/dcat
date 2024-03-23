#ifndef ABSTRACTDBWRITER_H
#define ABSTRACTDBWRITER_H

#include "structs.h"

class AbstractDBWriter {
public:
  virtual ~AbstractDBWriter() = default;
  virtual void addContent(DirContent &content) = 0;
};

#endif //ABSTRACTDBWRITER_H
