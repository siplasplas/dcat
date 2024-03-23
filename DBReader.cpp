#include "DBReader.h"
#include "endian_serial.h"

using namespace std;

DBReader::DBReader() {
    dbm.Open("dcat.tkt", false);
    string value;
    dbm.Get("0", &value);
    uint64_t x;
    deserializeBig(x, value.c_str());
    string_view key(value);
    dbm.Get(key, &value);
    cout << value;
}
DBReader::~DBReader() {
    dbm.Close();
}
