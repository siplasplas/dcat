//Variable-length quantity
#include <cstring>
#include "VLQ.h"

uint8_t *VLQ::to_seq(uint64_t x, uint8_t *out)
{
    int i, j;
    for (i = 9; i > 0; i--) {
        if (x & 127ULL << i * 7) break;
    }
    for (j = 0; j <= i; j++)
        out[j] = ((x >> ((i - j) * 7)) & 127) | 128;

    out[i] ^= 128;
    return i + 1 + out;
}

int VLQ::number_len(uint64_t x)
{
    int i;
    for (i = 9; i > 0; i--) {
        if (x & 127ULL << i * 7) break;
    }
    return i + 1;
}

uint8_t *VLQ::from_seq(uint8_t *in, uint64_t &result)
{
    uint64_t r = 0;

    do {
        r = (r << 7) | (uint64_t)(*in & 127);
    } while (*in++ & 128);

    result = r;
    return in;
}

uint8_t *VLQ::string_to_seq(const std::string &s, uint8_t *out) {
    out = to_seq(s.size(), out);
    memcpy(out, s.c_str(), s.size());
    return out + s.size();
}

uint8_t *VLQ::string_from_seq(uint8_t *in, std::string &s) {
    uint64_t len;
    in = from_seq(in, len);
    std::string str1((char*)in, len);
    s = str1;
    return in + len;
}

size_t VLQ::string_len(const std::string &s) {
    return number_len(s.size()) + s.size();
}
