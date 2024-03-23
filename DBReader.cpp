#include "DBReader.h"
#include "endian_serial.h"
#include "DirEntry.h"

using namespace std;

DBReader::DBReader() {
    dbm.Open("dcat.tkt", false);
    string value;
    dbm.Get("0", &value);
    uint64_t x;
    deserializeBig(x, value.c_str());
    string_view key(value);
    dbm.Get(key, &value);
    DirContent content;
    content.deserialize(value);
    for (auto &entry: content.entries)
        printf("%s %ld\n",entry.name.c_str(), entry.size);

}
DBReader::~DBReader() {
    dbm.Close();
}
