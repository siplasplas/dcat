#ifndef DBWRITER_H
#define DBWRITER_H

#include "AbstractDBWriter.h"
#include "tkrzw_dbm_tree.h"

class DBWriter: public AbstractDBWriter {
  tkrzw::TreeDBM dbm;
public:
  DBWriter();
  ~DBWriter() override;
  void addContent(uint64_t h, DirContent &content) override;
  void addRoot(uint64_t h) override;
  void open() override;
  void close() override;
  void test(int len, int count);
  void rebuild();
};

#endif //DBWRITER_H
