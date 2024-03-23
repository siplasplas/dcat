#ifndef ENDIAN_SERIAL_H
#define ENDIAN_SERIAL_H

#include <cstddef>
#include <cstdint>
#include <string>

char *serializeBig(int16_t x, char *dest);
char *serializeBig(uint16_t x, char *dest);
char *serializeBig(int32_t x, char *dest);
char *serializeBig(uint32_t x, char *dest);
char *serializeBig(int64_t x, char *dest);
char *serializeBig(uint64_t x, char *dest);
char *serializeString16Big(const std::string &str, char *dest);

char *deserializeBig(int16_t &x, char *src);
char *deserializeBig(uint16_t &x, char *src);
char *deserializeBig(int32_t &x, char *src);
char *deserializeBig(uint32_t &x, char *src);
char *deserializeBig(int64_t &x, char *src);
char *deserializeBig(uint64_t &x, char *src);

#endif //ENDIAN_SERIAL_H
