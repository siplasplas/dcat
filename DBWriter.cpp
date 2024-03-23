#include "DBWriter.h"

using namespace tkrzw;

DBWriter::DBWriter() {
    dbm.Open("dcat.tkt", true, File::OPEN_DEFAULT);
    dbm.Set("foo", "hop");
    dbm.Set("bar", "step");
    dbm.Set("baz", "jump");
}

DBWriter::~DBWriter() {
    dbm.Close();
}

void DBWriter::addContent(DirContent &content) {

}
