#include "DBWriter.h"
#include "endian_serial.h"

using namespace tkrzw;

DBWriter::DBWriter() {
}

DBWriter::~DBWriter() {
    close();
}

void DBWriter::open() {
    dbm.Open("dcat.tkt", true);
}
void DBWriter::close() {
    dbm.Close();
}

void DBWriter::addContent(uint64_t h, DirContent &content) {
    int len = 0;
    for (auto &entry: content) {
        entry.serialize();
        len += entry.serialLen;
    }
    char *buf = new char[len];
    char *dest= buf;
    for (auto &entry: content) {
        memcpy(dest, entry.serialized, entry.serialLen);
        dest += entry.serialLen;
    }
    std::string_view key(reinterpret_cast<const char*>(&h), sizeof(h));
    std::string_view value(buf, dest-buf);
    dbm.Set(key, value);
    delete[] buf;
}

void DBWriter::addRoot(uint64_t h) {
    char buf[8];
    serializeBig(h, buf);
    std::string_view value(buf, sizeof(h));
    printf("root = %lu\n",h);
    dbm.Set("0", value);
}


void DBWriter::test(int len, int count) {
    char fname[32];
    sprintf(fname, "%d_%d.tkt", len,count);
    TreeDBM dbm1;
    TreeDBM::TuningParameters tuning_params{};
    tuning_params.record_comp_mode = HashDBM::RECORD_COMP_LZ4;
    dbm1.OpenAdvanced(fname, true, File::OPEN_DEFAULT, tuning_params);
    for (int i=0; i<count; i++) {
        std::string_view key(reinterpret_cast<const char*>(&i), sizeof(i));
        std::string value(len, '0');
        dbm1.Set(key, value);
    }
    bool tobe;
    dbm1.ShouldBeRebuilt(&tobe);
    if (tobe)
        dbm1.Rebuild();
}

