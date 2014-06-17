#include <gtest/gtest.h>

#include "src/MahjongCommonUtils.h"

using namespace ydec::mahjong;
using namespace ydec::msc;

using namespace std;

class MahjongCommonUtilsTest : public ::testing::Test {
 protected:
};

TEST_F(MahjongCommonUtilsTest, MachiTypeTest) {
  EXPECT_FALSE(MahjongCommonUtils::isMachiTypeMatched(MachiType::MACHI_0FU, MachiType::KANCHAN));
  EXPECT_FALSE(MahjongCommonUtils::isMachiTypeMatched(MachiType::MACHI_0FU, MachiType::PENCHAN));
  EXPECT_TRUE(MahjongCommonUtils::isMachiTypeMatched(MachiType::MACHI_0FU, MachiType::RYANMEN));
  EXPECT_TRUE(MahjongCommonUtils::isMachiTypeMatched(MachiType::MACHI_0FU, MachiType::SHABO));
  EXPECT_FALSE(MahjongCommonUtils::isMachiTypeMatched(MachiType::MACHI_0FU, MachiType::TANKI));
  EXPECT_FALSE(MahjongCommonUtils::isMachiTypeMatched(MachiType::MACHI_0FU, MachiType::UNKNOWN_MACHI_TYPE));
  EXPECT_TRUE(MahjongCommonUtils::isMachiTypeMatched(MachiType::MACHI_0FU, MachiType::MACHI_0FU));
  EXPECT_FALSE(MahjongCommonUtils::isMachiTypeMatched(MachiType::MACHI_0FU, MachiType::MACHI_2FU));

  EXPECT_TRUE(MahjongCommonUtils::isMachiTypeMatched(MachiType::MACHI_2FU, MachiType::KANCHAN));
  EXPECT_TRUE(MahjongCommonUtils::isMachiTypeMatched(MachiType::MACHI_2FU, MachiType::PENCHAN));
  EXPECT_FALSE(MahjongCommonUtils::isMachiTypeMatched(MachiType::MACHI_2FU, MachiType::RYANMEN));
  EXPECT_FALSE(MahjongCommonUtils::isMachiTypeMatched(MachiType::MACHI_2FU, MachiType::SHABO));
  EXPECT_TRUE(MahjongCommonUtils::isMachiTypeMatched(MachiType::MACHI_2FU, MachiType::TANKI));
  EXPECT_FALSE(MahjongCommonUtils::isMachiTypeMatched(MachiType::MACHI_2FU, MachiType::UNKNOWN_MACHI_TYPE));
  EXPECT_FALSE(MahjongCommonUtils::isMachiTypeMatched(MachiType::MACHI_2FU, MachiType::MACHI_0FU));
  EXPECT_TRUE(MahjongCommonUtils::isMachiTypeMatched(MachiType::MACHI_2FU, MachiType::MACHI_2FU));

  EXPECT_TRUE(MahjongCommonUtils::isMachiTypeMatched(MachiType::KANCHAN, MachiType::KANCHAN));
  EXPECT_FALSE(MahjongCommonUtils::isMachiTypeMatched(MachiType::KANCHAN, MachiType::PENCHAN));
  EXPECT_FALSE(MahjongCommonUtils::isMachiTypeMatched(MachiType::KANCHAN, MachiType::RYANMEN));
  EXPECT_FALSE(MahjongCommonUtils::isMachiTypeMatched(MachiType::KANCHAN, MachiType::SHABO));
  EXPECT_FALSE(MahjongCommonUtils::isMachiTypeMatched(MachiType::KANCHAN, MachiType::TANKI));
  EXPECT_FALSE(MahjongCommonUtils::isMachiTypeMatched(MachiType::KANCHAN, MachiType::UNKNOWN_MACHI_TYPE));
  EXPECT_FALSE(MahjongCommonUtils::isMachiTypeMatched(MachiType::KANCHAN, MachiType::MACHI_0FU));
  EXPECT_FALSE(MahjongCommonUtils::isMachiTypeMatched(MachiType::KANCHAN, MachiType::MACHI_2FU));

  EXPECT_TRUE(MahjongCommonUtils::isMachiTypeMatched(MachiType::UNKNOWN_MACHI_TYPE, MachiType::KANCHAN));
  EXPECT_TRUE(MahjongCommonUtils::isMachiTypeMatched(MachiType::UNKNOWN_MACHI_TYPE, MachiType::PENCHAN));
  EXPECT_TRUE(MahjongCommonUtils::isMachiTypeMatched(MachiType::UNKNOWN_MACHI_TYPE, MachiType::RYANMEN));
  EXPECT_TRUE(MahjongCommonUtils::isMachiTypeMatched(MachiType::UNKNOWN_MACHI_TYPE, MachiType::SHABO));
  EXPECT_TRUE(MahjongCommonUtils::isMachiTypeMatched(MachiType::UNKNOWN_MACHI_TYPE, MachiType::TANKI));
  EXPECT_TRUE(MahjongCommonUtils::isMachiTypeMatched(MachiType::UNKNOWN_MACHI_TYPE, MachiType::UNKNOWN_MACHI_TYPE));
  EXPECT_TRUE(MahjongCommonUtils::isMachiTypeMatched(MachiType::UNKNOWN_MACHI_TYPE, MachiType::MACHI_0FU));
  EXPECT_TRUE(MahjongCommonUtils::isMachiTypeMatched(MachiType::UNKNOWN_MACHI_TYPE, MachiType::MACHI_2FU));
}
