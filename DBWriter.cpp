#include "DBWriter.h"
#include "VLQ.h"

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
    char buf[10];
    char *end = VLQ::to_seq(h, buf);
    std::string_view key(buf, end - buf);
    content.serialize();
    std::string_view value(content.serialized, content.serialLen);
    dbm.Set(key, value);
}

void DBWriter::addRoot(uint64_t h) {
    char buf[10];
    char *end = VLQ::to_seq(h, buf);
    std::string_view value(buf, end - buf);
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

void DBWriter::rebuild() {
    dbm.Open("dcat.tkt", true);
    dbm.Rebuild();
    dbm.Close();
}

