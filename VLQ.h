#ifndef VLQ_SERIAL_H
#define VLQ_SERIAL_H

#include <cstdint>
#include <string>

struct VLQ {
    static uint8_t *to_seq(uint64_t x, uint8_t *out);
    static uint8_t *from_seq(uint8_t *in, uint64_t &result);
    static uint8_t *string_to_seq(const std::string &s, uint8_t *out);
    static uint8_t *string_from_seq(uint8_t *in, std::string &s);
    static int number_len(uint64_t x);
    static size_t string_len(const std::string &s);
};

#endif //VLQ_SERIAL_H
