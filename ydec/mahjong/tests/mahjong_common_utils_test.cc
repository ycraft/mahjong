// Copyright 2016 Yuki Hamada
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "gtest/gtest.h"

#include "ydec/mahjong/src/mahjong_common_utils.h"

namespace ydec {
namespace mahjong {

class MahjongCommonUtilsTest : public testing::Test {};

TEST_F(MahjongCommonUtilsTest, TileStateMatchedTest) {
  EXPECT_TRUE(IsTileStateMatched(
      TileState::AGARI_HAI, TileState::AGARI_HAI));
  EXPECT_TRUE(IsTileStateMatched(
      TileState::AGARI_HAI, TileState::AGARI_HAI_RON));
  EXPECT_TRUE(IsTileStateMatched(
      TileState::AGARI_HAI, TileState::AGARI_HAI_TSUMO));
  EXPECT_FALSE(IsTileStateMatched(
      TileState::AGARI_HAI, TileState::UNKNOWN_TILE_STATE));
  EXPECT_FALSE(IsTileStateMatched(
      TileState::AGARI_HAI_RON, TileState::AGARI_HAI));
}

TEST_F(MahjongCommonUtilsTest, HandElementTypeMatchedTest) {
  EXPECT_TRUE(IsHandElementTypeMatched(
      HandElementType::SHUNTSU, HandElementType::SHUNTSU));
  EXPECT_TRUE(IsHandElementTypeMatched(
      HandElementType::SHUNTSU, HandElementType::ANSHUNTSU));
  EXPECT_TRUE(IsHandElementTypeMatched(
      HandElementType::SHUNTSU, HandElementType::MINSHUNTSU));
  EXPECT_FALSE(IsHandElementTypeMatched(
      HandElementType::KOUTSU, HandElementType::KANTSU));
}

TEST_F(MahjongCommonUtilsTest, MachiTypeTest) {
  EXPECT_FALSE(IsMachiTypeMatched(
      MachiType::MACHI_0FU, MachiType::KANCHAN));
  EXPECT_FALSE(IsMachiTypeMatched(
      MachiType::MACHI_0FU, MachiType::PENCHAN));
  EXPECT_TRUE(IsMachiTypeMatched(
      MachiType::MACHI_0FU, MachiType::RYANMEN));
  EXPECT_TRUE(IsMachiTypeMatched(
      MachiType::MACHI_0FU, MachiType::SHABO));
  EXPECT_FALSE(IsMachiTypeMatched(
      MachiType::MACHI_0FU, MachiType::TANKI));
  EXPECT_FALSE(IsMachiTypeMatched(
      MachiType::MACHI_0FU, MachiType::UNKNOWN_MACHI_TYPE));
  EXPECT_TRUE(IsMachiTypeMatched(
      MachiType::MACHI_0FU, MachiType::MACHI_0FU));
  EXPECT_FALSE(IsMachiTypeMatched(
      MachiType::MACHI_0FU, MachiType::MACHI_2FU));

  EXPECT_TRUE(IsMachiTypeMatched(
      MachiType::MACHI_2FU, MachiType::KANCHAN));
  EXPECT_TRUE(IsMachiTypeMatched(
      MachiType::MACHI_2FU, MachiType::PENCHAN));
  EXPECT_FALSE(IsMachiTypeMatched(
      MachiType::MACHI_2FU, MachiType::RYANMEN));
  EXPECT_FALSE(IsMachiTypeMatched(
      MachiType::MACHI_2FU, MachiType::SHABO));
  EXPECT_TRUE(IsMachiTypeMatched(
      MachiType::MACHI_2FU, MachiType::TANKI));
  EXPECT_FALSE(IsMachiTypeMatched(
      MachiType::MACHI_2FU, MachiType::UNKNOWN_MACHI_TYPE));
  EXPECT_FALSE(IsMachiTypeMatched(
      MachiType::MACHI_2FU, MachiType::MACHI_0FU));
  EXPECT_TRUE(IsMachiTypeMatched(
      MachiType::MACHI_2FU, MachiType::MACHI_2FU));

  EXPECT_TRUE(IsMachiTypeMatched(
      MachiType::KANCHAN, MachiType::KANCHAN));
  EXPECT_FALSE(IsMachiTypeMatched(
      MachiType::KANCHAN, MachiType::PENCHAN));
  EXPECT_FALSE(IsMachiTypeMatched(
      MachiType::KANCHAN, MachiType::RYANMEN));
  EXPECT_FALSE(IsMachiTypeMatched(
      MachiType::KANCHAN, MachiType::SHABO));
  EXPECT_FALSE(IsMachiTypeMatched(
      MachiType::KANCHAN, MachiType::TANKI));
  EXPECT_FALSE(IsMachiTypeMatched(
      MachiType::KANCHAN, MachiType::UNKNOWN_MACHI_TYPE));
  EXPECT_FALSE(IsMachiTypeMatched(
      MachiType::KANCHAN, MachiType::MACHI_0FU));
  EXPECT_FALSE(IsMachiTypeMatched(
      MachiType::KANCHAN, MachiType::MACHI_2FU));

  EXPECT_TRUE(IsMachiTypeMatched(
      MachiType::UNKNOWN_MACHI_TYPE, MachiType::KANCHAN));
  EXPECT_TRUE(IsMachiTypeMatched(
      MachiType::UNKNOWN_MACHI_TYPE, MachiType::PENCHAN));
  EXPECT_TRUE(IsMachiTypeMatched(
      MachiType::UNKNOWN_MACHI_TYPE, MachiType::RYANMEN));
  EXPECT_TRUE(IsMachiTypeMatched(
      MachiType::UNKNOWN_MACHI_TYPE, MachiType::SHABO));
  EXPECT_TRUE(IsMachiTypeMatched(
      MachiType::UNKNOWN_MACHI_TYPE, MachiType::TANKI));
  EXPECT_TRUE(IsMachiTypeMatched(
      MachiType::UNKNOWN_MACHI_TYPE, MachiType::UNKNOWN_MACHI_TYPE));
  EXPECT_TRUE(IsMachiTypeMatched(
      MachiType::UNKNOWN_MACHI_TYPE, MachiType::MACHI_0FU));
  EXPECT_TRUE(IsMachiTypeMatched(
      MachiType::UNKNOWN_MACHI_TYPE, MachiType::MACHI_2FU));
}

}  // namespace mahjong
}  // namespace ydec
