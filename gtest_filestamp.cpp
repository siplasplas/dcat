//see https://www.silisoftware.com/tools/date.php
#include "gtest/gtest.h"
#include "DirEntry.h"

using namespace std;

TEST(Timestamp, Unix0) {
    timespec unixTimeSpec = {0,0};
    uint64_t ft = 116444736000000000;
    uint64_t ftEst = DirEntry::timeNanosecToWindows(unixTimeSpec);
    EXPECT_EQ(ftEst, ft);
}

TEST(Timestamp, today) {
    timespec unixTimeSpec = {1711225175,0};
    uint64_t ft = 133556987750000000;
    uint64_t ftEst = DirEntry::timeNanosecToWindows(unixTimeSpec);
    EXPECT_EQ(ftEst, ft);
}

TEST(Timestamp, todayFractions) {
    timespec unixTimeSpec = {1711225175, 123456789};
    uint64_t ft = 133556987751234567;
    uint64_t ftEst = DirEntry::timeNanosecToWindows(unixTimeSpec);
    EXPECT_EQ(ftEst, ft);
}
