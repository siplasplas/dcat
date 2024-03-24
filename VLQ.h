#ifndef VLQ_SERIAL_H
#define VLQ_SERIAL_H

#include <cstdint>
#include <string>

struct VLQ {
    static char *to_seq(uint64_t x, char *out);
    static const char *from_seq(const char *in, uint64_t &result);
    static char *string_to_seq(const std::string &s, char *out);
    static const char *string_from_seq(const char *in, std::string &s);
    static int number_len(uint64_t x);
    static size_t string_len(const std::string &s);
};

#endif //VLQ_SERIAL_H
