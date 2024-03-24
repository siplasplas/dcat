#include "endian_serial.h"
#include <cstring>
#ifdef _WIN32
 #define htobe16(x) __builtin_bswap16 (x)
 #define be16toh(x) __builtin_bswap16 (x)
 #define htobe32(x) __builtin_bswap32 (x)
 #define be32toh(x) __builtin_bswap32 (x)
 #define htobe64(x) __builtin_bswap64 (x)
 #define be64toh(x) __builtin_bswap64 (x)
#else
#include <endian.h>
#endif

using namespace std;

char* serializeBig(int16_t x, char *dest) {
    *reinterpret_cast<int16_t*>(dest) = htobe16(x);
    return dest + sizeof(x);
}
char* serializeBig(uint16_t x, char *dest) {
    *reinterpret_cast<uint16_t*>(dest) = htobe16(x);
    return dest + sizeof(x);
}
char* serializeBig(int32_t x, char *dest) {
    *reinterpret_cast<int32_t*>(dest) = htobe32(x);
    return dest + sizeof(x);
}
char* serializeBig(uint32_t x, char *dest) {
    *reinterpret_cast<uint32_t*>(dest) = htobe32(x);
    return dest + sizeof(x);
}
char* serializeBig(int64_t x, char *dest) {
    *reinterpret_cast<int64_t*>(dest) = htobe64(x);
    return dest + sizeof(x);
}
char* serializeBig(uint64_t x, char *dest) {
    *reinterpret_cast<uint64_t*>(dest) = htobe64(x);
    return dest + sizeof(x);
}

char *serializeString16Big(const std::string &str, char *dest) {
    uint16_t len = static_cast<uint16_t>(str.size());
    dest = serializeBig(len, dest);
    memcpy(dest, str.c_str(), len);
    dest += len;
    return dest;
}

const char *deserializeBig(int16_t &x, const char *src) {
    x = be16toh(*reinterpret_cast<const int16_t*>(src));
    return src + sizeof(x);
}
const char *deserializeBig(uint16_t &x, const char *src) {
    x = be16toh(*reinterpret_cast<const uint16_t*>(src));
    return src + sizeof(x);
}
const char *deserializeBig(int32_t &x, const char *src) {
    x = be32toh(*reinterpret_cast<const int32_t*>(src));
    return src + sizeof(x);
}
const char *deserializeBig(uint32_t &x, const char *src) {
    x = be32toh(*reinterpret_cast<const uint32_t*>(src));
    return src + sizeof(x);
}
const char *deserializeBig(int64_t &x, const char *src) {
    x = be64toh(*reinterpret_cast<const int64_t*>(src));
    return src + sizeof(x);
}
const char *deserializeBig(uint64_t &x, const char *src) {
    x = be64toh(*reinterpret_cast<const uint64_t*>(src));
    return src + sizeof(x);
}

const char *deserializeString16Big(std::string &str, const char *dest) {
    uint16_t len;
    dest = deserializeBig(len, dest);
    string str1(dest, len);
    str = str1;
    dest += len;
    return dest;
}
