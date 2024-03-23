#include "gtest/gtest.h"
#include "endian_serial.h"

using namespace std;

TEST(Serialization, int16) {
    int16_t n = -1234;
    char buf[10];
    const char *s = serializeBig(n, buf);
    EXPECT_EQ(s-buf, 2);
    int16_t n1;
    s = deserializeBig(n1,buf);
    EXPECT_EQ(n1, n);
    EXPECT_EQ(s-buf, 2);
}

TEST(Serialization, uint16) {
    uint16_t n = 1234;
    char buf[10];
    const char *s = serializeBig(n, buf);
    EXPECT_EQ(s-buf, 2);
    uint16_t n1;
    s = deserializeBig(n1,buf);
    EXPECT_EQ(n1, n);
    EXPECT_EQ(s-buf, 2);
}

TEST(Serialization, int32) {
    int32_t n = -12345678;
    char buf[10];
    const char *s = serializeBig(n, buf);
    EXPECT_EQ(s-buf, 4);
    int32_t n1;
    s = deserializeBig(n1,buf);
    EXPECT_EQ(n1, n);
    EXPECT_EQ(s-buf, 4);
}

TEST(Serialization, uint32) {
    uint32_t n = 12345678;
    char buf[10];
    const char *s = serializeBig(n, buf);
    EXPECT_EQ(s-buf, 4);
    uint32_t n1;
    s = deserializeBig(n1,buf);
    EXPECT_EQ(n1, n);
    EXPECT_EQ(s-buf, 4);
}

TEST(Serialization, int64) {
    int64_t n = -123456781234;
    char buf[10];
    const char *s = serializeBig(n, buf);
    EXPECT_EQ(s-buf, 8);
    int64_t n1;
    s = deserializeBig(n1,buf);
    EXPECT_EQ(n1, n);
    EXPECT_EQ(s-buf, 8);
}


TEST(Serialization, uint64) {
    uint64_t n = 123456781234;
    char buf[10];
    const char *s = serializeBig(n, buf);
    EXPECT_EQ(s-buf, 8);
    uint64_t n1;
    s = deserializeBig(n1,buf);
    EXPECT_EQ(n1, n);
    EXPECT_EQ(s-buf, 8);
}
