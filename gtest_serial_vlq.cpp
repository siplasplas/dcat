#include "gtest/gtest.h"
#include "VLQ.h"
#include "DirEntry.h"

using namespace std;

TEST(VLQ, n4) {
    char expectedSeq[] = { (char)0x81, -1, -1, (char)0x7E };
    uint64_t numberToConvert = 0x3FFFFE;
    char resultSeq[10];
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


TEST(VLQ, full) {
    uint64_t numberToConvert = 13160044982828396511UL;
    char resultSeq[10];
    long len1 = VLQ::to_seq(numberToConvert, resultSeq) - resultSeq;
    long len1a = VLQ::number_len(numberToConvert);
    EXPECT_EQ(10, len1);
    EXPECT_EQ(len1a, len1);
}

TEST(VLQ, string) {
    char buf[10];
    string s1 = "abc";
    const char *s = VLQ::string_to_seq(s1, buf);
    size_t len = VLQ::string_len(s1);
    EXPECT_EQ(s - buf, s1.size() + 1);
    EXPECT_EQ(len, s1.size() + 1);
    string s2;
    s = VLQ::string_from_seq(buf, s2);
    EXPECT_EQ(s - buf, s1.size() + 1);
    EXPECT_EQ(s2, s1);
}

TEST(VLQ, DirEntry) {
    DirEntry de1;
    de1.size = 1234;
    de1.name = "name";
    de1.serialize();
    DirEntry de2;
    de2.deserialize(de1.serialized);
    EXPECT_EQ(de2, de1);
}

TEST(VLQ, DirContent) {
    DirContent content1;
    DirEntry de1;
    de1.size = 1234;
    de1.name = "name1";
    content1.entries.push_back(de1);
    de1.size = 5678;
    de1.name = "name2";
    content1.entries.push_back(de1);

    content1.serialize();

    DirContent content2;
    content2.deserialize(string_view(content1.serialized, content1.serialLen));

    EXPECT_EQ(content2.entries.size(), content1.entries.size());
    for (int i=0; i<content2.entries.size(); i++) {
        EXPECT_EQ(content2.entries[i], content1.entries[i]);
    }
}