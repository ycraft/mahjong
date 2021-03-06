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

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "google/protobuf/text_format.h"
#include "gtest/gtest.h"

#include "src/yaku_applier.h"
#include "tests/common_test_util.h"

using google::protobuf::TextFormat;
using std::ifstream;
using std::istream;
using std::ostream_iterator;
using std::string;
using std::stringstream;
using std::vector;

namespace ycraft {
namespace mahjong {

namespace {
string ConcatStrings(const vector<string>& strings) {
  stringstream ss;
  copy(strings.begin(), strings.end(), ostream_iterator<string>(ss, ", "));
  return ss.str();
}

vector<string> GetYakuNames(const YakuApplierResult& result) {
  vector<string> yakus;
  yakus.reserve(result.yaku_size());
  for (const Yaku& yaku : result.yaku()) {
    yakus.push_back(yaku.name());
  }
  return yakus;
}
}  // namespace

/**
 * Unit tests for YakuApplier.
 */
class YakuApplierTest : public testing::Test {
 protected:
  YakuApplierTest() : yaku_applier_(rule_) {}

  static void SetUpTestCase() {
    ifstream rule_file;
    rule_file.open("data/rule.pb", istream::in | istream::binary);
    rule_.ParseFromIstream(&rule_file);
    rule_file.close();
  }

  void AssertEquals(const vector<string>& expected,
                    const YakuApplierResult& actual) {
    vector<string> e = expected, a = GetYakuNames(actual);
    sort(e.begin(), e.end());
    sort(a.begin(), a.end());
    if (e != a) {
      FAIL() << "Expected = {" << ConcatStrings(e) << "}\n"
             << "Actual = {" << ConcatStrings(a) << "}";
    }
  }

  YakuApplier yaku_applier_;

 private:
  static Rule rule_;
};

Rule YakuApplierTest::rule_;

TEST_F(YakuApplierTest, ApplyTest_Chitoitsu_1) {
  ParsedHand parsed_hand;
  CommonTestUtil::CreateAntoitsu(parsed_hand.add_element(), TileType::PINZU_1,
                                 true);
  CommonTestUtil::CreateAntoitsu(parsed_hand.add_element(), TileType::PINZU_3);
  CommonTestUtil::CreateAntoitsu(parsed_hand.add_element(), TileType::PINZU_5);
  CommonTestUtil::CreateAntoitsu(parsed_hand.add_element(), TileType::PINZU_7);
  CommonTestUtil::CreateAntoitsu(parsed_hand.add_element(), TileType::PINZU_9);
  CommonTestUtil::CreateAntoitsu(parsed_hand.add_element(), TileType::SOUZU_1);
  CommonTestUtil::CreateAntoitsu(parsed_hand.add_element(), TileType::SOUZU_3);
  parsed_hand.mutable_agari()->set_format(AgariFormat::CHITOITSU_AGARI);

  YakuApplierResult result;
  yaku_applier_.Apply(RichiType::NO_RICHI, TileType::WIND_TON /* field_wind */,
                      TileType::WIND_NAN /* player_wind */, parsed_hand,
                      &result);
  ASSERT_NO_FATAL_FAILURE(AssertEquals({"七対子"}, result));
}

TEST_F(YakuApplierTest, ApplyTest_Chitoitsu_2) {
  ParsedHand parsed_hand;
  CommonTestUtil::CreateAntoitsu(parsed_hand.add_element(), TileType::PINZU_1,
                                 true);
  CommonTestUtil::CreateAntoitsu(parsed_hand.add_element(), TileType::PINZU_1);
  CommonTestUtil::CreateAntoitsu(parsed_hand.add_element(), TileType::PINZU_5);
  CommonTestUtil::CreateAntoitsu(parsed_hand.add_element(), TileType::PINZU_7);
  CommonTestUtil::CreateAntoitsu(parsed_hand.add_element(), TileType::PINZU_9);
  CommonTestUtil::CreateAntoitsu(parsed_hand.add_element(), TileType::SOUZU_1);
  CommonTestUtil::CreateAntoitsu(parsed_hand.add_element(), TileType::SOUZU_3);
  parsed_hand.mutable_agari()->set_format(AgariFormat::CHITOITSU_AGARI);

  YakuApplierResult result;
  yaku_applier_.Apply(RichiType::NO_RICHI, TileType::WIND_TON /* field_wind */,
                      TileType::WIND_NAN /* player_wind */, parsed_hand,
                      &result);
  // We cannot use same tile kind for two-toitsu.
  ASSERT_NO_FATAL_FAILURE(AssertEquals({}, result));
}

TEST_F(YakuApplierTest, ApplyTest_Regular_1) {
  ParsedHand parsed_hand;
  CommonTestUtil::CreateAnshuntsu(parsed_hand.add_element(), TileType::PINZU_1,
                                  0);
  CommonTestUtil::CreateAnshuntsu(parsed_hand.add_element(), TileType::PINZU_1);
  CommonTestUtil::CreateAnshuntsu(parsed_hand.add_element(), TileType::PINZU_1);
  CommonTestUtil::CreateAnshuntsu(parsed_hand.add_element(), TileType::PINZU_1);
  CommonTestUtil::CreateAntoitsu(parsed_hand.add_element(), TileType::SOUZU_3);
  parsed_hand.mutable_agari()->set_format(AgariFormat::REGULAR_AGARI);
  parsed_hand.mutable_agari()->set_type(AgariType::TSUMO);
  parsed_hand.set_machi_type(MachiType::RYANMEN);

  YakuApplierResult result;
  yaku_applier_.Apply(RichiType::NO_RICHI, TileType::WIND_TON /* field_wind */,
                      TileType::WIND_NAN /* player_wind */, parsed_hand,
                      &result);
  ASSERT_NO_FATAL_FAILURE(
      AssertEquals({"門前清自摸和", "二盃口", "平和"}, result));
}

TEST_F(YakuApplierTest, ApplyTest_Regular_2) {
  ParsedHand parsed_hand;
  CommonTestUtil::CreateMinkoutsu(parsed_hand.add_element(), TileType::SOUZU_1,
                                  true);
  CommonTestUtil::CreateAnkoutsu(parsed_hand.add_element(), TileType::PINZU_2);
  CommonTestUtil::CreateAnkoutsu(parsed_hand.add_element(), TileType::PINZU_4);
  CommonTestUtil::CreateAnkoutsu(parsed_hand.add_element(), TileType::PINZU_6);
  CommonTestUtil::CreateAntoitsu(parsed_hand.add_element(), TileType::SOUZU_3);
  parsed_hand.mutable_agari()->set_format(AgariFormat::REGULAR_AGARI);
  parsed_hand.mutable_agari()->set_type(AgariType::RON);
  parsed_hand.set_machi_type(MachiType::SHABO);

  YakuApplierResult result;
  yaku_applier_.Apply(RichiType::NO_RICHI, TileType::WIND_TON /* field_wind */,
                      TileType::WIND_NAN /* player_wind */, parsed_hand,
                      &result);
  ASSERT_NO_FATAL_FAILURE(AssertEquals({"三暗刻", "対々和"}, result));
}

TEST_F(YakuApplierTest, ApplyTest_Regular_3) {
  ParsedHand parsed_hand;
  CommonTestUtil::CreateAnkoutsu(parsed_hand.add_element(), TileType::SOUZU_1,
                                 true);
  CommonTestUtil::CreateAnkoutsu(parsed_hand.add_element(), TileType::PINZU_2);
  CommonTestUtil::CreateAnkoutsu(parsed_hand.add_element(), TileType::PINZU_4);
  CommonTestUtil::CreateAnkoutsu(parsed_hand.add_element(), TileType::PINZU_6);
  CommonTestUtil::CreateAntoitsu(parsed_hand.add_element(), TileType::SOUZU_3);
  parsed_hand.mutable_agari()->set_format(AgariFormat::REGULAR_AGARI);
  parsed_hand.mutable_agari()->set_type(AgariType::TSUMO);
  parsed_hand.set_machi_type(MachiType::SHABO);

  YakuApplierResult result;
  yaku_applier_.Apply(RichiType::NO_RICHI, TileType::WIND_TON /* field_wind */,
                      TileType::WIND_NAN /* player_wind */, parsed_hand,
                      &result);
  ASSERT_NO_FATAL_FAILURE(AssertEquals({"四暗刻"}, result));
}

TEST_F(YakuApplierTest, ApplyTest_Regular_4) {
  ParsedHand parsed_hand;
  CommonTestUtil::CreateAnkoutsu(parsed_hand.add_element(), TileType::SOUZU_1);
  CommonTestUtil::CreateAnkoutsu(parsed_hand.add_element(), TileType::PINZU_2);
  CommonTestUtil::CreateAnkoutsu(parsed_hand.add_element(), TileType::PINZU_4);
  CommonTestUtil::CreateAnkoutsu(parsed_hand.add_element(), TileType::PINZU_6);
  CommonTestUtil::CreateAntoitsu(parsed_hand.add_element(), TileType::SOUZU_3,
                                 true);
  parsed_hand.mutable_agari()->set_format(AgariFormat::REGULAR_AGARI);
  parsed_hand.mutable_agari()->set_type(AgariType::TSUMO);
  parsed_hand.set_machi_type(MachiType::TANKI);

  YakuApplierResult result;
  yaku_applier_.Apply(RichiType::NO_RICHI, TileType::WIND_TON /* field_wind */,
                      TileType::WIND_NAN /* player_wind */, parsed_hand,
                      &result);
  ASSERT_NO_FATAL_FAILURE(AssertEquals({"四暗刻単騎待ち"}, result));
}

TEST_F(YakuApplierTest, ApplyTest_Regular_5) {
  ParsedHand parsed_hand;
  CommonTestUtil::CreateAnshuntsu(parsed_hand.add_element(), TileType::SOUZU_1,
                                  0);
  CommonTestUtil::CreateAnshuntsu(parsed_hand.add_element(), TileType::SOUZU_4);
  CommonTestUtil::CreateAnshuntsu(parsed_hand.add_element(), TileType::SOUZU_7);
  CommonTestUtil::CreateAnshuntsu(parsed_hand.add_element(), TileType::SOUZU_1);
  CommonTestUtil::CreateAntoitsu(parsed_hand.add_element(), TileType::SOUZU_3);
  parsed_hand.mutable_agari()->set_format(AgariFormat::REGULAR_AGARI);
  parsed_hand.mutable_agari()->set_type(AgariType::RON);
  parsed_hand.set_machi_type(MachiType::RYANMEN);

  YakuApplierResult result;
  yaku_applier_.Apply(RichiType::NO_RICHI, TileType::WIND_TON /* field_wind */,
                      TileType::WIND_NAN /* player_wind */, parsed_hand,
                      &result);
  ASSERT_NO_FATAL_FAILURE(
      AssertEquals({"一気通貫", "一盃口", "平和", "清一色"}, result));
}

TEST_F(YakuApplierTest, ApplyTest_Regular_5_2) {
  ParsedHand parsed_hand;
  CommonTestUtil::CreateMinshuntsu(parsed_hand.add_element(), TileType::SOUZU_1,
                                   0);
  CommonTestUtil::CreateAnshuntsu(parsed_hand.add_element(), TileType::SOUZU_4);
  CommonTestUtil::CreateAnshuntsu(parsed_hand.add_element(), TileType::SOUZU_7);
  CommonTestUtil::CreateAnshuntsu(parsed_hand.add_element(), TileType::SOUZU_1);
  CommonTestUtil::CreateAntoitsu(parsed_hand.add_element(), TileType::SOUZU_3);
  parsed_hand.mutable_agari()->set_format(AgariFormat::REGULAR_AGARI);
  parsed_hand.mutable_agari()->set_type(AgariType::RON);
  parsed_hand.set_machi_type(MachiType::RYANMEN);

  YakuApplierResult result;
  yaku_applier_.Apply(RichiType::NO_RICHI, TileType::WIND_TON /* field_wind */,
                      TileType::WIND_NAN /* player_wind */, parsed_hand,
                      &result);
  // This hand contains min-shuntsu but this is still menzen hand because
  // that min-shuntsu has ron tile, not a chii.
  ASSERT_NO_FATAL_FAILURE(
      AssertEquals({"一気通貫", "一盃口", "平和", "清一色"}, result));
}

TEST_F(YakuApplierTest, ApplyTest_Regular_5_3) {
  ParsedHand parsed_hand;
  CommonTestUtil::CreateMinshuntsu(parsed_hand.add_element(), TileType::SOUZU_1,
                                   0);
  CommonTestUtil::CreateMinshuntsu(parsed_hand.add_element(),
                                   TileType::SOUZU_4);
  CommonTestUtil::CreateAnshuntsu(parsed_hand.add_element(), TileType::SOUZU_7);
  CommonTestUtil::CreateAnshuntsu(parsed_hand.add_element(), TileType::SOUZU_1);
  CommonTestUtil::CreateAntoitsu(parsed_hand.add_element(), TileType::SOUZU_3);
  parsed_hand.mutable_agari()->set_format(AgariFormat::REGULAR_AGARI);
  parsed_hand.mutable_agari()->set_type(AgariType::RON);
  parsed_hand.set_machi_type(MachiType::RYANMEN);

  YakuApplierResult result;
  yaku_applier_.Apply(RichiType::NO_RICHI, TileType::WIND_TON /* field_wind */,
                      TileType::WIND_NAN /* player_wind */, parsed_hand,
                      &result);
  ASSERT_NO_FATAL_FAILURE(AssertEquals({"一気通貫", "清一色"}, result));
}

TEST_F(YakuApplierTest, ApplyTest_Regular_6) {
  ParsedHand parsed_hand;
  CommonTestUtil::CreateAnkantsu(parsed_hand.add_element(), TileType::WIND_TON);
  CommonTestUtil::CreateAnkantsu(parsed_hand.add_element(), TileType::WIND_NAN);
  CommonTestUtil::CreateAnkantsu(parsed_hand.add_element(), TileType::WIND_SHA);
  CommonTestUtil::CreateAnkantsu(parsed_hand.add_element(), TileType::WIND_PE);
  CommonTestUtil::CreateAntoitsu(parsed_hand.add_element(),
                                 TileType::SANGEN_HAKU, true);
  parsed_hand.mutable_agari()->set_format(AgariFormat::REGULAR_AGARI);
  parsed_hand.mutable_agari()->set_type(AgariType::TSUMO);
  parsed_hand.set_machi_type(MachiType::TANKI);

  YakuApplierResult result;
  yaku_applier_.Apply(RichiType::NO_RICHI, TileType::WIND_TON /* field_wind */,
                      TileType::WIND_NAN /* player_wind */, parsed_hand,
                      &result);
  ASSERT_NO_FATAL_FAILURE(
      AssertEquals({"四暗刻単騎待ち", "四槓子", "大四喜", "字一色"}, result));
}

TEST_F(YakuApplierTest, ApplyTest_Regular_7) {
  ParsedHand parsed_hand;
  CommonTestUtil::CreateAnshuntsu(parsed_hand.add_element(), TileType::PINZU_1,
                                  0);
  CommonTestUtil::CreateAnshuntsu(parsed_hand.add_element(), TileType::PINZU_4);
  CommonTestUtil::CreateAnshuntsu(parsed_hand.add_element(), TileType::SOUZU_4);
  CommonTestUtil::CreateAnkoutsu(parsed_hand.add_element(), TileType::WIND_TON);
  CommonTestUtil::CreateAntoitsu(parsed_hand.add_element(), TileType::SOUZU_3);
  parsed_hand.mutable_agari()->set_format(AgariFormat::REGULAR_AGARI);
  parsed_hand.mutable_agari()->set_type(AgariType::TSUMO);
  parsed_hand.set_machi_type(MachiType::RYANMEN);

  YakuApplierResult result;
  yaku_applier_.Apply(RichiType::NO_RICHI, TileType::WIND_TON /* field_wind */,
                      TileType::WIND_NAN /* player_wind */, parsed_hand,
                      &result);
  ASSERT_NO_FATAL_FAILURE(AssertEquals({"門前清自摸和", "場風牌 東"}, result));
}

TEST_F(YakuApplierTest, ApplyTest_Regular_8) {
  ParsedHand parsed_hand;
  CommonTestUtil::CreateAnshuntsu(parsed_hand.add_element(), TileType::PINZU_1,
                                  0);
  CommonTestUtil::CreateAnshuntsu(parsed_hand.add_element(), TileType::PINZU_4);
  CommonTestUtil::CreateAnshuntsu(parsed_hand.add_element(), TileType::SOUZU_4);
  CommonTestUtil::CreateAnkoutsu(parsed_hand.add_element(), TileType::WIND_TON);
  CommonTestUtil::CreateAntoitsu(parsed_hand.add_element(), TileType::SOUZU_3);
  parsed_hand.mutable_agari()->set_format(AgariFormat::REGULAR_AGARI);
  parsed_hand.mutable_agari()->set_type(AgariType::TSUMO);
  parsed_hand.set_machi_type(MachiType::RYANMEN);

  YakuApplierResult result;
  yaku_applier_.Apply(RichiType::NO_RICHI, TileType::WIND_NAN /* field_wind */,
                      TileType::WIND_TON /* player_wind */, parsed_hand,
                      &result);
  ASSERT_NO_FATAL_FAILURE(AssertEquals({"門前清自摸和", "自風牌 東"}, result));
}

TEST_F(YakuApplierTest, ApplyTest_Regular_9) {
  ParsedHand parsed_hand;
  CommonTestUtil::CreateAnshuntsu(parsed_hand.add_element(), TileType::PINZU_1,
                                  0);
  CommonTestUtil::CreateAnshuntsu(parsed_hand.add_element(), TileType::PINZU_4);
  CommonTestUtil::CreateAnshuntsu(parsed_hand.add_element(), TileType::SOUZU_4);
  CommonTestUtil::CreateAnkoutsu(parsed_hand.add_element(), TileType::WIND_NAN);
  CommonTestUtil::CreateAntoitsu(parsed_hand.add_element(), TileType::SOUZU_3);
  parsed_hand.mutable_agari()->set_format(AgariFormat::REGULAR_AGARI);
  parsed_hand.mutable_agari()->set_type(AgariType::TSUMO);
  parsed_hand.set_machi_type(MachiType::RYANMEN);

  YakuApplierResult result;
  yaku_applier_.Apply(RichiType::NO_RICHI, TileType::WIND_NAN /* field_wind */,
                      TileType::WIND_NAN /* player_wind */, parsed_hand,
                      &result);
  ASSERT_NO_FATAL_FAILURE(
      AssertEquals({"門前清自摸和", "自風牌 南", "場風牌 南"}, result));
}

TEST_F(YakuApplierTest, ApplyTest_Regular_Pinhu_1) {
  ParsedHand parsed_hand;
  CommonTestUtil::CreateAnshuntsu(parsed_hand.add_element(), TileType::PINZU_1,
                                  0);
  CommonTestUtil::CreateAnshuntsu(parsed_hand.add_element(), TileType::PINZU_4);
  CommonTestUtil::CreateAnshuntsu(parsed_hand.add_element(), TileType::SOUZU_4);
  CommonTestUtil::CreateAnshuntsu(parsed_hand.add_element(), TileType::SOUZU_7);
  CommonTestUtil::CreateAntoitsu(parsed_hand.add_element(), TileType::MANZU_3);
  parsed_hand.mutable_agari()->set_format(AgariFormat::REGULAR_AGARI);
  parsed_hand.mutable_agari()->set_type(AgariType::TSUMO);
  parsed_hand.set_machi_type(MachiType::RYANMEN);

  YakuApplierResult result;
  yaku_applier_.Apply(RichiType::NO_RICHI, TileType::WIND_TON /* field_wind */,
                      TileType::WIND_NAN /* player_wind */, parsed_hand,
                      &result);
  ASSERT_NO_FATAL_FAILURE(AssertEquals({"門前清自摸和", "平和"}, result));
}

TEST_F(YakuApplierTest, ApplyTest_Regular_Pinhu_2) {
  ParsedHand parsed_hand;
  CommonTestUtil::CreateAnshuntsu(parsed_hand.add_element(), TileType::PINZU_1,
                                  1);
  CommonTestUtil::CreateAnshuntsu(parsed_hand.add_element(), TileType::PINZU_4);
  CommonTestUtil::CreateAnshuntsu(parsed_hand.add_element(), TileType::SOUZU_4);
  CommonTestUtil::CreateAnshuntsu(parsed_hand.add_element(), TileType::SOUZU_7);
  CommonTestUtil::CreateAntoitsu(parsed_hand.add_element(), TileType::MANZU_3);
  parsed_hand.mutable_agari()->set_format(AgariFormat::REGULAR_AGARI);
  parsed_hand.mutable_agari()->set_type(AgariType::TSUMO);
  parsed_hand.set_machi_type(MachiType::KANCHAN);

  YakuApplierResult result;
  yaku_applier_.Apply(RichiType::NO_RICHI, TileType::WIND_TON /* field_wind */,
                      TileType::WIND_NAN /* player_wind */, parsed_hand,
                      &result);
  ASSERT_NO_FATAL_FAILURE(AssertEquals({"門前清自摸和"}, result));
}

TEST_F(YakuApplierTest, ApplyTest_Regular_Pinhu_3) {
  ParsedHand parsed_hand;
  CommonTestUtil::CreateAnshuntsu(parsed_hand.add_element(), TileType::PINZU_1,
                                  0);
  CommonTestUtil::CreateAnshuntsu(parsed_hand.add_element(), TileType::PINZU_4);
  CommonTestUtil::CreateAnshuntsu(parsed_hand.add_element(), TileType::SOUZU_4);
  CommonTestUtil::CreateAnshuntsu(parsed_hand.add_element(), TileType::SOUZU_7);
  CommonTestUtil::CreateAntoitsu(parsed_hand.add_element(), TileType::WIND_TON);
  parsed_hand.mutable_agari()->set_format(AgariFormat::REGULAR_AGARI);
  parsed_hand.mutable_agari()->set_type(AgariType::TSUMO);
  parsed_hand.set_machi_type(MachiType::RYANMEN);

  YakuApplierResult result;
  yaku_applier_.Apply(RichiType::NO_RICHI, TileType::WIND_TON /* field_wind */,
                      TileType::WIND_NAN /* player_wind */, parsed_hand,
                      &result);
  ASSERT_NO_FATAL_FAILURE(AssertEquals({"門前清自摸和"}, result));
}

TEST_F(YakuApplierTest, ApplyTest_Regular_Pinhu_4) {
  ParsedHand parsed_hand;
  CommonTestUtil::CreateAnshuntsu(parsed_hand.add_element(), TileType::PINZU_1,
                                  0);
  CommonTestUtil::CreateAnshuntsu(parsed_hand.add_element(), TileType::PINZU_4);
  CommonTestUtil::CreateAnshuntsu(parsed_hand.add_element(), TileType::SOUZU_4);
  CommonTestUtil::CreateAnshuntsu(parsed_hand.add_element(), TileType::SOUZU_7);
  CommonTestUtil::CreateAntoitsu(parsed_hand.add_element(), TileType::WIND_NAN);
  parsed_hand.mutable_agari()->set_format(AgariFormat::REGULAR_AGARI);
  parsed_hand.mutable_agari()->set_type(AgariType::TSUMO);
  parsed_hand.set_machi_type(MachiType::RYANMEN);

  YakuApplierResult result;
  yaku_applier_.Apply(RichiType::NO_RICHI, TileType::WIND_TON /* field_wind */,
                      TileType::WIND_NAN /* player_wind */, parsed_hand,
                      &result);
  ASSERT_NO_FATAL_FAILURE(AssertEquals({"門前清自摸和"}, result));
}

TEST_F(YakuApplierTest, ApplyTest_Regular_Pinhu_5) {
  ParsedHand parsed_hand;
  CommonTestUtil::CreateAnshuntsu(parsed_hand.add_element(), TileType::PINZU_1,
                                  0);
  CommonTestUtil::CreateAnshuntsu(parsed_hand.add_element(), TileType::PINZU_4);
  CommonTestUtil::CreateAnshuntsu(parsed_hand.add_element(), TileType::SOUZU_4);
  CommonTestUtil::CreateAnshuntsu(parsed_hand.add_element(), TileType::SOUZU_7);
  CommonTestUtil::CreateAntoitsu(parsed_hand.add_element(), TileType::WIND_TON);
  parsed_hand.mutable_agari()->set_format(AgariFormat::REGULAR_AGARI);
  parsed_hand.mutable_agari()->set_type(AgariType::TSUMO);
  parsed_hand.set_machi_type(MachiType::RYANMEN);

  YakuApplierResult result;
  yaku_applier_.Apply(RichiType::NO_RICHI, TileType::WIND_NAN /* field_wind */,
                      TileType::WIND_NAN /* player_wind */, parsed_hand,
                      &result);
  ASSERT_NO_FATAL_FAILURE(AssertEquals({"門前清自摸和", "平和"}, result));
}

TEST_F(YakuApplierTest, ApplyTest_Regular_Ipeko) {
  ParsedHand parsed_hand;
  CommonTestUtil::CreateAnshuntsu(parsed_hand.add_element(), TileType::PINZU_1,
                                  0);
  CommonTestUtil::CreateAnshuntsu(parsed_hand.add_element(), TileType::PINZU_1);
  CommonTestUtil::CreateAnshuntsu(parsed_hand.add_element(), TileType::SOUZU_2);
  CommonTestUtil::CreateAnshuntsu(parsed_hand.add_element(), TileType::SOUZU_5);
  CommonTestUtil::CreateAntoitsu(parsed_hand.add_element(), TileType::MANZU_3);
  parsed_hand.mutable_agari()->set_format(AgariFormat::REGULAR_AGARI);
  parsed_hand.mutable_agari()->set_type(AgariType::TSUMO);
  parsed_hand.set_machi_type(MachiType::RYANMEN);

  YakuApplierResult result;
  yaku_applier_.Apply(RichiType::NO_RICHI, TileType::WIND_TON /* field_wind */,
                      TileType::WIND_NAN /* player_wind */, parsed_hand,
                      &result);
  ASSERT_NO_FATAL_FAILURE(
      AssertEquals({"門前清自摸和", "一盃口", "平和"}, result));
}

TEST_F(YakuApplierTest, ApplyTest_Regular_Ipeko_2) {
  ParsedHand parsed_hand;
  CommonTestUtil::CreateAnshuntsu(parsed_hand.add_element(), TileType::PINZU_1,
                                  0);
  CommonTestUtil::CreateAnshuntsu(parsed_hand.add_element(), TileType::PINZU_1);
  CommonTestUtil::CreateMinshuntsu(parsed_hand.add_element(),
                                   TileType::SOUZU_2);
  CommonTestUtil::CreateAnshuntsu(parsed_hand.add_element(), TileType::SOUZU_5);
  CommonTestUtil::CreateAntoitsu(parsed_hand.add_element(), TileType::MANZU_3);
  parsed_hand.mutable_agari()->set_format(AgariFormat::REGULAR_AGARI);
  parsed_hand.mutable_agari()->set_type(AgariType::TSUMO);
  parsed_hand.set_machi_type(MachiType::RYANMEN);

  YakuApplierResult result;
  yaku_applier_.Apply(RichiType::NO_RICHI, TileType::WIND_TON /* field_wind */,
                      TileType::WIND_NAN /* player_wind */, parsed_hand,
                      &result);
  ASSERT_NO_FATAL_FAILURE(AssertEquals({}, result));
}

/**
 * Unit tests for HandConditionValidator.
 */
class HandConditionValidatorTest : public testing::Test {
 protected:
  HandConditionValidatorResult::Type Validate(const HandCondition& condition,
                                              const RichiType& richi_type,
                                              const TileType& field_wind,
                                              const TileType& player_wind,
                                              const ParsedHand& parsed_hand) {
    return HandConditionValidator(condition, richi_type, field_wind,
                                  player_wind, parsed_hand)
        .Validate();
  }

  HandConditionValidatorResult::Type Validate(const HandCondition& condition,
                                              const ParsedHand& parsed_hand) {
    return Validate(condition, RichiType::NO_RICHI,
                    /*field_wind=*/TileType::WIND_TON,
                    /*player_wind=*/TileType::WIND_NAN, parsed_hand);
  }

  HandConditionValidatorResult::Type Validate(const HandCondition& condition) {
    return Validate(condition, RichiType::NO_RICHI,
                    /*field_wind=*/TileType::WIND_TON,
                    /*player_wind=*/TileType::WIND_NAN,
                    ParsedHand::default_instance());
  }

  HandConditionValidatorResult::Type Validate(const HandCondition& condition,
                                              const Agari& agari) {
    ParsedHand parsed_hand;
    parsed_hand.mutable_agari()->CopyFrom(agari);
    return Validate(condition, RichiType::NO_RICHI,
                    /*field_wind=*/TileType::WIND_TON,
                    /*player_wind=*/TileType::WIND_NAN, parsed_hand);
  }

  HandConditionValidatorResult::Type Validate(const HandCondition& condition,
                                              const RichiType& richi_type) {
    return Validate(condition, richi_type,
                    /*field_wind=*/TileType::WIND_TON,
                    /*player_wind=*/TileType::WIND_NAN,
                    ParsedHand::default_instance());
  }

  HandConditionValidatorResult::Type ValidateFieldWind(
      const HandCondition& condition, const TileType& field_wind) {
    return Validate(condition, RichiType::NO_RICHI, field_wind,
                    /*player_wind=*/TileType::WIND_NAN,
                    ParsedHand::default_instance());
  }

  HandConditionValidatorResult::Type ValidatePlayerWind(
      const HandCondition& condition, const TileType& player_wind) {
    return Validate(condition, RichiType::NO_RICHI,
                    /*field_wind=*/TileType::WIND_TON, player_wind,
                    ParsedHand::default_instance());
  }
};

/**
 * Tests for "Allowed Tile Condition"
 */
TEST_F(HandConditionValidatorTest, ValidateTest_AllowedTileCondition_1) {
  HandCondition condition;
  EXPECT_TRUE(
      TextFormat::ParseFromString("allowed_tile_condition {"
                                  "  allowed_tile_type: PINZU_1"
                                  "}",
                                  &condition));

  EXPECT_EQ(HandConditionValidatorResult_Type_OK,
            Validate(condition, ParsedHand::default_instance()));
}

TEST_F(HandConditionValidatorTest, ValidateTest_AllowedTileCondition_2) {
  HandCondition condition;
  EXPECT_TRUE(
      TextFormat::ParseFromString("allowed_tile_condition {"
                                  "  allowed_tile_type: PINZU_1"
                                  "}",
                                  &condition));

  ParsedHand hand;
  CommonTestUtil::CreateAnkoutsu(hand.add_element(), TileType::PINZU_1);

  EXPECT_EQ(HandConditionValidatorResult_Type_OK, Validate(condition, hand));
}

TEST_F(HandConditionValidatorTest, ValidateTest_AllowedTileCondition_3) {
  HandCondition condition;
  EXPECT_TRUE(
      TextFormat::ParseFromString("allowed_tile_condition {"
                                  "  allowed_tile_type: PINZU_1"
                                  "}",
                                  &condition));

  ParsedHand hand;
  CommonTestUtil::CreateAnshuntsu(hand.add_element(), TileType::PINZU_1);

  EXPECT_EQ(HandConditionValidatorResult_Type_NG_ALLOWED_TILE_CONDITION,
            Validate(condition, hand));
}

TEST_F(HandConditionValidatorTest, ValidateTest_AllowedTileCondition_4) {
  HandCondition condition;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "allowed_tile_condition {"
      "  allowed_tile_type: PINZU_3"
      "}"
      "allowed_tile_condition {"
      "  allowed_tile_type: PINZU_2"
      "}"
      "allowed_tile_condition {"
      "  required_variable_tile_type: VARIABLE_TILE_A"
      "}",
      &condition));

  ParsedHand hand;
  CommonTestUtil::CreateAnshuntsu(hand.add_element(), TileType::PINZU_1);

  EXPECT_EQ(HandConditionValidatorResult_Type_OK, Validate(condition, hand));
}

TEST_F(HandConditionValidatorTest, ValidateTest_AllowedTileCondition_5) {
  HandCondition condition;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "allowed_tile_condition {"
      "  allowed_tile_type: PINZU_3"
      "}"
      "allowed_tile_condition {"
      "  required_variable_tile_type: VARIABLE_TILE_A"
      "}",
      &condition));

  ParsedHand hand;
  CommonTestUtil::CreateAnshuntsu(hand.add_element(), TileType::PINZU_1);

  EXPECT_EQ(HandConditionValidatorResult_Type_NG_ALLOWED_TILE_CONDITION,
            Validate(condition, hand));
}

TEST_F(HandConditionValidatorTest, ValidateTest_AllowedTileCondition_6) {
  HandCondition condition;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "allowed_tile_condition {"
      "  required_variable_tile_type: VARIABLE_NUMBER_A"
      "}",
      &condition));

  ParsedHand hand;
  CommonTestUtil::CreateAntoitsu(hand.add_element(), TileType::PINZU_1);
  CommonTestUtil::CreateAntoitsu(hand.add_element(), TileType::SOUZU_1);
  CommonTestUtil::CreateAntoitsu(hand.add_element(), TileType::MANZU_1);

  EXPECT_EQ(HandConditionValidatorResult_Type_OK, Validate(condition, hand));
}

TEST_F(HandConditionValidatorTest, ValidateTest_AllowedTileCondition_7) {
  HandCondition condition;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "allowed_tile_condition {"
      "  required_variable_tile_type: VARIABLE_COLOR_A"
      "}",
      &condition));

  ParsedHand hand;
  CommonTestUtil::CreateAntoitsu(hand.add_element(), TileType::PINZU_1);
  CommonTestUtil::CreateAntoitsu(hand.add_element(), TileType::PINZU_2);

  EXPECT_EQ(HandConditionValidatorResult_Type_OK, Validate(condition, hand));
}

TEST_F(HandConditionValidatorTest, ValidateTest_AllowedTileCondition_8) {
  HandCondition condition;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "allowed_tile_condition {"
      "  required_variable_tile_type: VARIABLE_COLOR_A"
      "}",
      &condition));

  ParsedHand hand;
  CommonTestUtil::CreateAntoitsu(hand.add_element(), TileType::PINZU_1);
  CommonTestUtil::CreateAntoitsu(hand.add_element(), TileType::SANGEN_HAKU);

  EXPECT_EQ(HandConditionValidatorResult_Type_NG_ALLOWED_TILE_CONDITION,
            Validate(condition, hand));
}

TEST_F(HandConditionValidatorTest, ValidateTest_AllowedTileCondition_9) {
  HandCondition condition;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "allowed_tile_condition {"
      "  required_variable_tile_type: VARIABLE_COLOR_A"
      "}",
      &condition));

  ParsedHand hand;
  CommonTestUtil::CreateAntoitsu(hand.add_element(), TileType::SANGEN_HAKU);

  EXPECT_EQ(HandConditionValidatorResult_Type_NG_ALLOWED_TILE_CONDITION,
            Validate(condition, hand));
}

TEST_F(HandConditionValidatorTest, ValidateTest_AllowedTileCondition_10) {
  HandCondition condition;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "allowed_tile_condition {"
      "  required_variable_tile_type: VARIABLE_COLOR_A_OR_JIHAI"
      "}",
      &condition));

  ParsedHand hand;
  CommonTestUtil::CreateAntoitsu(hand.add_element(), TileType::PINZU_1);
  CommonTestUtil::CreateAntoitsu(hand.add_element(), TileType::PINZU_2);

  EXPECT_EQ(HandConditionValidatorResult_Type_OK, Validate(condition, hand));
}

TEST_F(HandConditionValidatorTest, ValidateTest_AllowedTileCondition_11) {
  HandCondition condition;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "allowed_tile_condition {"
      "  required_variable_tile_type: VARIABLE_COLOR_A_OR_JIHAI"
      "}",
      &condition));

  ParsedHand hand;
  CommonTestUtil::CreateAntoitsu(hand.add_element(), TileType::SANGEN_HAKU);
  CommonTestUtil::CreateAntoitsu(hand.add_element(), TileType::PINZU_1);

  EXPECT_EQ(HandConditionValidatorResult_Type_OK, Validate(condition, hand));
}

TEST_F(HandConditionValidatorTest, ValidateTest_AllowedTileCondition_12) {
  HandCondition condition;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "allowed_tile_condition {"
      "  required_variable_tile_type: VARIABLE_COLOR_A_OR_JIHAI"
      "}",
      &condition));

  ParsedHand hand;
  CommonTestUtil::CreateAntoitsu(hand.add_element(), TileType::SANGEN_HAKU);
  CommonTestUtil::CreateAntoitsu(hand.add_element(), TileType::WIND_TON);

  EXPECT_EQ(HandConditionValidatorResult_Type_OK, Validate(condition, hand));
}

TEST_F(HandConditionValidatorTest, ValidateTest_AllowedTileCondition_13) {
  HandCondition condition;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "allowed_tile_condition {"
      "  required_variable_tile_type: VARIABLE_COLOR_A_OR_JIHAI"
      "}",
      &condition));

  ParsedHand hand;
  CommonTestUtil::CreateAntoitsu(hand.add_element(), TileType::SANGEN_HAKU);
  CommonTestUtil::CreateAntoitsu(hand.add_element(), TileType::PINZU_1);
  CommonTestUtil::CreateAntoitsu(hand.add_element(), TileType::SOUZU_1);

  EXPECT_EQ(HandConditionValidatorResult_Type_NG_ALLOWED_TILE_CONDITION,
            Validate(condition, hand));
}

/**
 * Tests for "Deny tile condition"
 */
TEST_F(HandConditionValidatorTest, ValidateTest_DenyTileCondition_1) {
  HandCondition condition;
  EXPECT_TRUE(
      TextFormat::ParseFromString("deny_tile_condition {"
                                  "  allowed_tile_type: PINZU_8"
                                  "}",
                                  &condition));

  EXPECT_EQ(HandConditionValidatorResult_Type_OK,
            Validate(condition, ParsedHand::default_instance()));
}

TEST_F(HandConditionValidatorTest, ValidateTest_DenyTileCondition_2) {
  HandCondition condition;
  EXPECT_TRUE(
      TextFormat::ParseFromString("deny_tile_condition {"
                                  "  allowed_tile_type: PINZU_8"
                                  "}",
                                  &condition));

  ParsedHand hand;
  CommonTestUtil::CreateAnshuntsu(hand.add_element(), TileType::PINZU_1);

  EXPECT_EQ(HandConditionValidatorResult_Type_OK, Validate(condition, hand));
}

TEST_F(HandConditionValidatorTest, ValidateTest_DenyTileCondition_3) {
  HandCondition condition;
  EXPECT_TRUE(
      TextFormat::ParseFromString("deny_tile_condition {"
                                  "  allowed_tile_type: PINZU_8"
                                  "}",
                                  &condition));

  ParsedHand hand;
  CommonTestUtil::CreateAnshuntsu(hand.add_element(), TileType::PINZU_7);

  EXPECT_EQ(HandConditionValidatorResult_Type_NG_DENY_TILE_CONDITION,
            Validate(condition, hand));
}

/**
 * Tests for "Required tile condition"
 */
TEST_F(HandConditionValidatorTest, ValidateTest_RequiredTileCondition_1) {
  HandCondition condition;
  EXPECT_TRUE(
      TextFormat::ParseFromString("required_tile_condition {"
                                  "  allowed_tile_type: PINZU_8"
                                  "}",
                                  &condition));

  ParsedHand hand;
  CommonTestUtil::CreateAnshuntsu(hand.add_element(), TileType::SOUZU_1);

  EXPECT_EQ(HandConditionValidatorResult_Type_NG_REQUIRED_TILE_CONDITION,
            Validate(condition, hand));
}

TEST_F(HandConditionValidatorTest, ValidateTest_RequiredTileCondition_2) {
  HandCondition condition;
  EXPECT_TRUE(
      TextFormat::ParseFromString("required_tile_condition {"
                                  "  allowed_tile_type: SOUZU_1"
                                  "}"
                                  "required_tile_condition {"
                                  "  allowed_tile_type: SOUZU_3"
                                  "}",
                                  &condition));

  ParsedHand hand;
  CommonTestUtil::CreateAnshuntsu(hand.add_element(), TileType::SOUZU_1);

  EXPECT_EQ(HandConditionValidatorResult_Type_OK, Validate(condition, hand));
}

TEST_F(HandConditionValidatorTest, ValidateTest_RequiredTileCondition_3) {
  HandCondition condition;
  EXPECT_TRUE(
      TextFormat::ParseFromString("required_tile_condition {"
                                  "  required_state: AGARI_HAI"
                                  "}",
                                  &condition));

  ParsedHand hand;
  EXPECT_EQ(HandConditionValidatorResult_Type_NG_REQUIRED_TILE_CONDITION,
            Validate(condition, hand));

  Element* element = hand.add_element();
  Tile* tile = element->add_tile();
  EXPECT_EQ(HandConditionValidatorResult_Type_NG_REQUIRED_TILE_CONDITION,
            Validate(condition, hand));

  tile->add_state(TileState::AGARI_HAI_RON);
  EXPECT_EQ(HandConditionValidatorResult_Type_OK, Validate(condition, hand));
}

TEST_F(HandConditionValidatorTest, ValidateTest_RequiredTileCondition_3_2) {
  HandCondition condition;
  EXPECT_TRUE(
      TextFormat::ParseFromString("required_tile_condition {"
                                  "  required_state: AGARI_HAI_TSUMO"
                                  "}",
                                  &condition));

  ParsedHand hand;
  EXPECT_EQ(HandConditionValidatorResult_Type_NG_REQUIRED_TILE_CONDITION,
            Validate(condition, hand));

  Element* element = hand.add_element();
  Tile* tile = element->add_tile();
  EXPECT_EQ(HandConditionValidatorResult_Type_NG_REQUIRED_TILE_CONDITION,
            Validate(condition, hand));

  tile->add_state(TileState::AGARI_HAI_RON);
  EXPECT_EQ(HandConditionValidatorResult_Type_NG_REQUIRED_TILE_CONDITION,
            Validate(condition, hand));

  tile->add_state(TileState::AGARI_HAI_TSUMO);
  EXPECT_EQ(HandConditionValidatorResult_Type_OK, Validate(condition, hand));
}

TEST_F(HandConditionValidatorTest, ValidateTest_RequiredTileCondition_4) {
  HandCondition condition;
  EXPECT_TRUE(
      TextFormat::ParseFromString("required_tile_condition {"
                                  "  deny_state: AGARI_HAI"
                                  "}",
                                  &condition));

  ParsedHand hand;
  EXPECT_EQ(HandConditionValidatorResult_Type_NG_REQUIRED_TILE_CONDITION,
            Validate(condition, hand));

  Element* element = hand.add_element();
  Tile* tile = element->add_tile();
  EXPECT_EQ(HandConditionValidatorResult_Type_OK, Validate(condition, hand));

  tile->add_state(TileState::AGARI_HAI_RON);
  EXPECT_EQ(HandConditionValidatorResult_Type_NG_REQUIRED_TILE_CONDITION,
            Validate(condition, hand));

  element->add_tile();
  EXPECT_EQ(HandConditionValidatorResult_Type_OK, Validate(condition, hand));
}

TEST_F(HandConditionValidatorTest, ValidateTest_RequiredTileCondition_5) {
  HandCondition condition;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "required_tile_condition {"
      "  required_variable_tile_type: VARIABLE_TILE_A"
      "}"
      "required_tile_condition {"
      "  required_variable_tile_type: VARIABLE_TILE_B"
      "}",
      &condition));

  ParsedHand hand;
  EXPECT_EQ(HandConditionValidatorResult_Type_NG_REQUIRED_TILE_CONDITION,
            Validate(condition, hand));

  CommonTestUtil::CreateAntoitsu(hand.add_element(), TileType::SOUZU_1);
  EXPECT_EQ(HandConditionValidatorResult_Type_NG_REQUIRED_TILE_CONDITION,
            Validate(condition, hand));

  CommonTestUtil::CreateAntoitsu(hand.add_element(), TileType::SOUZU_2);
  EXPECT_EQ(HandConditionValidatorResult_Type_OK, Validate(condition, hand));
}

TEST_F(HandConditionValidatorTest, ValidateTest_RequiredMachiType_1) {
  HandCondition condition;
  EXPECT_TRUE(TextFormat::ParseFromString("required_machi_type: MACHI_2FU ",
                                          &condition));

  ParsedHand hand;
  hand.set_machi_type(MachiType::KANCHAN);

  EXPECT_EQ(HandConditionValidatorResult_Type_OK, Validate(condition, hand));
}

TEST_F(HandConditionValidatorTest, ValidateTest_RequiredMachiType_2) {
  HandCondition condition;
  EXPECT_TRUE(TextFormat::ParseFromString("required_machi_type: MACHI_2FU ",
                                          &condition));

  ParsedHand hand;
  hand.set_machi_type(MachiType::RYANMEN);

  EXPECT_EQ(HandConditionValidatorResult_Type_NG_REQUIRED_MACHI_TYPE,
            Validate(condition, hand));
}

TEST_F(HandConditionValidatorTest, ValidateTest_RequiredAgariCondition_1) {
  HandCondition condition;
  EXPECT_TRUE(
      TextFormat::ParseFromString("required_agari_condition {"
                                  "  required_type: RON"
                                  "}",
                                  &condition));

  Agari agari;
  agari.set_type(AgariType::RON);
  EXPECT_EQ(HandConditionValidatorResult_Type_OK, Validate(condition, agari));

  agari.set_type(AgariType::TSUMO);
  EXPECT_EQ(HandConditionValidatorResult_Type_NG_REQUIRED_AGARI_CONDITION,
            Validate(condition, agari));
}

TEST_F(HandConditionValidatorTest, ValidateTest_RequiredAgariCondition_2) {
  HandCondition condition;
  EXPECT_TRUE(
      TextFormat::ParseFromString("required_agari_condition {"
                                  "  required_state: SOKU"
                                  "  required_state: HAITEI"
                                  "}",
                                  &condition));

  Agari agari;
  EXPECT_EQ(HandConditionValidatorResult_Type_NG_REQUIRED_AGARI_CONDITION,
            Validate(condition, agari));

  agari.add_state(AgariState::SOKU);
  EXPECT_EQ(HandConditionValidatorResult_Type_NG_REQUIRED_AGARI_CONDITION,
            Validate(condition, agari));

  agari.add_state(AgariState::HAITEI);
  EXPECT_EQ(HandConditionValidatorResult_Type_OK, Validate(condition, agari));

  agari.add_state(AgariState::RINSHAN);
  EXPECT_EQ(HandConditionValidatorResult_Type_OK, Validate(condition, agari));
}

TEST_F(HandConditionValidatorTest, ValidateTest_RequiredAgariCondition_3) {
  HandCondition condition;
  EXPECT_TRUE(
      TextFormat::ParseFromString("required_agari_condition {"
                                  "  allowed_format: REGULAR_AGARI"
                                  "  allowed_format: CHITOITSU_AGARI"
                                  "}",
                                  &condition));

  Agari agari;
  EXPECT_EQ(HandConditionValidatorResult_Type_NG_REQUIRED_AGARI_CONDITION,
            Validate(condition, agari));

  agari.set_format(AgariFormat::IRREGULAR_AGARI);
  EXPECT_EQ(HandConditionValidatorResult_Type_NG_REQUIRED_AGARI_CONDITION,
            Validate(condition, agari));

  agari.set_format(AgariFormat::REGULAR_AGARI);
  EXPECT_EQ(HandConditionValidatorResult_Type_OK, Validate(condition, agari));

  agari.set_format(AgariFormat::CHITOITSU_AGARI);
  EXPECT_EQ(HandConditionValidatorResult_Type_OK, Validate(condition, agari));
}

TEST_F(HandConditionValidatorTest, ValidateTest_RequiredRichiType_1) {
  HandCondition condition;
  EXPECT_EQ(HandConditionValidatorResult_Type_OK,
            Validate(condition, RichiType::NO_RICHI));

  condition.set_required_richi_type(RichiType::RICHI);
  EXPECT_EQ(HandConditionValidatorResult_Type_NG_REQUIRED_RICHI_TYPE,
            Validate(condition, RichiType::NO_RICHI));
  EXPECT_EQ(HandConditionValidatorResult_Type_OK,
            Validate(condition, RichiType::RICHI));
  EXPECT_EQ(HandConditionValidatorResult_Type_OK,
            Validate(condition, RichiType::NORMAL_RICHI));
  EXPECT_EQ(HandConditionValidatorResult_Type_OK,
            Validate(condition, RichiType::DOUBLE_RICHI));
  EXPECT_EQ(HandConditionValidatorResult_Type_OK,
            Validate(condition, RichiType::OPEN_RICHI));

  condition.set_required_richi_type(RichiType::DOUBLE_RICHI);
  EXPECT_EQ(HandConditionValidatorResult_Type_NG_REQUIRED_RICHI_TYPE,
            Validate(condition, RichiType::NO_RICHI));
  EXPECT_EQ(HandConditionValidatorResult_Type_NG_REQUIRED_RICHI_TYPE,
            Validate(condition, RichiType::RICHI));
  EXPECT_EQ(HandConditionValidatorResult_Type_NG_REQUIRED_RICHI_TYPE,
            Validate(condition, RichiType::NORMAL_RICHI));
  EXPECT_EQ(HandConditionValidatorResult_Type_OK,
            Validate(condition, RichiType::DOUBLE_RICHI));
  EXPECT_EQ(HandConditionValidatorResult_Type_NG_REQUIRED_RICHI_TYPE,
            Validate(condition, RichiType::OPEN_RICHI));
}

TEST_F(HandConditionValidatorTest, ValidateTest_RequiredFieldWind) {
  HandCondition condition;
  EXPECT_EQ(HandConditionValidatorResult_Type_OK,
            ValidateFieldWind(condition, TileType::WIND_TON));

  condition.set_required_field_wind(TileType::WIND_NAN);
  EXPECT_EQ(HandConditionValidatorResult_Type_NG_REQUIRED_FIELD_WIND,
            ValidateFieldWind(condition, TileType::WIND_TON));
  EXPECT_EQ(HandConditionValidatorResult_Type_OK,
            ValidateFieldWind(condition, TileType::WIND_NAN));
  EXPECT_EQ(HandConditionValidatorResult_Type_NG_REQUIRED_FIELD_WIND,
            ValidateFieldWind(condition, TileType::WIND_SHA));
  EXPECT_EQ(HandConditionValidatorResult_Type_NG_REQUIRED_FIELD_WIND,
            ValidateFieldWind(condition, TileType::WIND_PE));
}

TEST_F(HandConditionValidatorTest, ValidateTest_RequiredPlayerWind) {
  HandCondition condition;
  EXPECT_EQ(HandConditionValidatorResult_Type_OK,
            ValidatePlayerWind(condition, TileType::WIND_TON));

  condition.set_required_player_wind(TileType::WIND_SHA);
  EXPECT_EQ(HandConditionValidatorResult_Type_NG_REQUIRED_PLAYER_WIND,
            ValidatePlayerWind(condition, TileType::WIND_TON));
  EXPECT_EQ(HandConditionValidatorResult_Type_NG_REQUIRED_PLAYER_WIND,
            ValidatePlayerWind(condition, TileType::WIND_NAN));
  EXPECT_EQ(HandConditionValidatorResult_Type_OK,
            ValidatePlayerWind(condition, TileType::WIND_SHA));
  EXPECT_EQ(HandConditionValidatorResult_Type_NG_REQUIRED_PLAYER_WIND,
            ValidatePlayerWind(condition, TileType::WIND_PE));
}

}  // namespace mahjong
}  // namespace ycraft
