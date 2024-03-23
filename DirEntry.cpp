#include "DirEntry.h"
#include "endian_serial.h"

bool DirEntry::operator<(const DirEntry &other) const {
    return name < other.name;
}

int64_t DirEntry::timeNanosecToWindows(timespec &unixTimeSpec) {
    return 0;
}

bool DirEntry::mayBeListed() const {
    auto pos = name.rfind('.');
    if (pos==std::string::npos) return false;
    std::string ext = name.substr(pos);
    if (ext==".zip")
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
