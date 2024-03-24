#include "gtest/gtest.h"
#include "VLQ.h"
#include "DirEntry.h"

using namespace std;

TEST(VLQ, n4) {
    uint8_t expectedSeq[] = { 0x81, 0xFF, 0xFF, 0x7E };
    uint64_t numberToConvert = 0x3FFFFE;
    uint8_t resultSeq[9];
    long len1 = VLQ::to_seq(numberToConvert, resultSeq) - resultSeq;
    long len1a = VLQ::number_len(numberToConvert);
    EXPECT_EQ(sizeof(expectedSeq), len1);
    EXPECT_EQ(len1a, len1);
    EXPECT_TRUE(std::equal(std::begin(expectedSeq), std::end(expectedSeq), std::begin(resultSeq)));
    uint64_t number;
    long len2 = VLQ::from_seq(resultSeq, number) - resultSeq;
    EXPECT_EQ(len1, len2);
    EXPECT_EQ(numberToConvert, number);
}

TEST(VLQ, string) {
    uint8_t buf[10];
    string s1 = "abc";
    const uint8_t *s = VLQ::string_to_seq(s1, buf);
    size_t len = VLQ::string_len(s1);
    EXPECT_EQ(s - buf, s1.size() + 1);
    EXPECT_EQ(len, s1.size() + 1);
    string s2;
    s = VLQ::string_from_seq(buf, s2);
    EXPECT_EQ(s - buf, s1.size() + 1);
    EXPECT_EQ(s2, s1);
}