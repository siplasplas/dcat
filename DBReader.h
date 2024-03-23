#ifndef DBREADER_H
#define DBREADER_H

#include <tkrzw_dbm_tree.h>

class DBReader {
  tkrzw::TreeDBM dbm;
public:
  DBReader();
  ~DBReader();
};

#endif //DBREADER_H
