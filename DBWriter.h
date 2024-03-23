#ifndef DBWRITER_H
#define DBWRITER_H

#include "AbstractDBWriter.h"
#include "tkrzw_dbm_tree.h"

class DBWriter: public AbstractDBWriter {
  tkrzw::TreeDBM dbm;
public:
  DBWriter();
  ~DBWriter() override;
  void addContent(DirContent &content) override;
};

#endif //DBWRITER_H
