#include <cstring>
#include "DirEntry.h"
#include "endian_serial.h"

using namespace std;

bool DirEntry::operator<(const DirEntry &other) const {
    return name < other.name;
}

const uint32_t WinToUnixDateDelta = 134774;// between January 1, 1601 and January 1, 1970
const uint32_t WinUnitsInSec = 10 * 1000 * 1000;// 100-nanosecond intervals

int64_t DirEntry::timeNanosecToWindows(timespec &unixTimeSpec) {
    return (int64_t) unixTimeSpec.tv_sec * WinUnitsInSec + unixTimeSpec.tv_nsec / 100
        + (int64_t)WinToUnixDateDelta*24*3600*WinUnitsInSec;
}

bool DirEntry::mayBeListed() const {
    auto pos = name.rfind('.');
    if (pos == std::string::npos)
        return false;
    std::string ext = name.substr(pos);
    if (ext == ".zip")
        return true;
    else
        return false;
}

void DirEntry::serialize() {
    delete serialized;
    serialized = new char[bufferSize()];
    auto dest = serialized;
    dest = serializeBig(size, dest);
    dest = serializeBig(sectors, dest);
    dest = serializeBig(mtime, dest);
    dest = serializeBig(attr, dest);
    dest = serializeBig(key, dest);
    dest = serializeString16Big(name, dest);
    serialLen = dest - serialized;
}

int DirEntry::bufferSize() {
    return sizeof(size) + sizeof(sectors)
        + sizeof(mtime) + sizeof(attr)
        + sizeof(key) + 2 + name.size();
}

const char *DirEntry::deserialize(const char *s) {
    delete serialized;
    s = deserializeBig(size, s);
    s = deserializeBig(sectors, s);
    s = deserializeBig(mtime, s);
    s = deserializeBig(attr, s);
    s = deserializeBig(key, s);
    s = deserializeString16Big(name, s);
    return s;
}

void DirContent::deserialize(string_view value) {
    entries.clear();
    const char *source = value.begin();
    size_t len = value.size();
    while (source < value.end()) {
        DirEntry dirEntry;
        source = dirEntry.deserialize(source);
        entries.push_back(dirEntry);
    }
}

void DirContent::serialize() {
    serialLen = 0;
    delete serialized;
    for (auto &entry : entries) {
        entry.serialize();
        serialLen += entry.serialLen;
    }
    serialized = new char[serialLen];
    char *dest = serialized;
    for (auto &entry : entries) {
        memcpy(dest, entry.serialized, entry.serialLen);
        dest += entry.serialLen;
    }
}
