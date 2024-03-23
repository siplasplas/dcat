#include "DBWriter.h"

using namespace tkrzw;

DBWriter::DBWriter() {
    dbm.Open("dcat.tkt", true, File::OPEN_DEFAULT);
}

DBWriter::~DBWriter() {
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
