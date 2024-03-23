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
    process(x);
}
DBReader::~DBReader() {
    dbm.Close();
}

void DBReader::process(uint64_t key) {
    char buf[sizeof(key)];
    serializeBig(key, buf);
    string value;
    dbm.Get(buf, &value);
    DirContent content;
    content.deserialize(value);
    for (auto &entry : content.entries)
        printf("%s %ld %llu\n", entry.name.c_str(), entry.size, entry.key);
    for (auto &entry : content.entries)
        if (entry.key)
            process(entry.key);
}
