#include <cstring>
#include "DirEntry.h"
#include "endian_serial.h"
#include "VLQ.h"

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
    dest = VLQ::to_seq(size, dest);
    dest = VLQ::to_seq(sectors, dest);
    dest = VLQ::to_seq(mtime, dest);
    dest = VLQ::to_seq(attr, dest);
    dest = VLQ::to_seq(key, dest);
    dest = VLQ::string_to_seq(name, dest);
    serialLen = dest - serialized;
}

size_t DirEntry::bufferSize() const {
    return VLQ::number_len(size) + VLQ::number_len(sectors)
        + VLQ::number_len(mtime) + VLQ::number_len(attr)
        + VLQ::number_len(key) + VLQ::string_len(name);
}

const char *DirEntry::deserialize(const char *s) {
    delete serialized;
    s = VLQ::from_seq(s, size);
    s = VLQ::from_seq(s, sectors);
    s = VLQ::from_seq(s, mtime);
    s = VLQ::from_seq(s, attr);
    s = VLQ::from_seq(s, key);
    s = VLQ::string_from_seq(s, name);
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
