#include "DBReader.h"
#include "DirEntry.h"
#include "VLQ.h"

using namespace std;

DBReader::DBReader() {
    dbm.Open("dcat.tkt", false);
    string value;
    dbm.Get("0", &value);
    uint64_t x;
    VLQ::from_seq(value.c_str(), x);
    process(x);
}
DBReader::~DBReader() {
    dbm.Close();
}

void DBReader::process(uint64_t key) {
    char buf[10];
    char *end = VLQ::to_seq(key, buf);
    string value;
    dbm.Get(string_view(buf, end-buf), &value);
    DirContent content;
    content.deserialize(value);
    for (auto &entry : content.entries)
        printf("%s %ld %lu\n", entry.name.c_str(), entry.size, entry.key);
    for (auto &entry : content.entries)
        if (entry.key)
            process(entry.key);
}
