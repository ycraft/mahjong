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

#include "gtest/gtest.h"

#include "ydec/mahjong/src/hand_parser.h"
#include "ydec/mahjong/src/score_calculator.h"
#include "ydec/mahjong/src/yaku_applier.h"

using std::copy;
using std::ifstream;
using std::istream;
using std::ostream_iterator;
using std::string;
using std::stringstream;
using std::unique_ptr;
using std::vector;

namespace ydec {
namespace mahjong {

namespace {
  string ConcatStrings(const vector<string>& strings) {
    stringstream ss;
    copy(strings.begin(), strings.end(), ostream_iterator<string>(ss, ", "));
    return ss.str();
  }
}  // namespace

/**
 * Unit tests for ScoreCalculator.
 */
class ScoreCalculatorTest : public testing::Test {
 protected:
  ScoreCalculatorTest() :
      score_calculator_(unique_ptr<Rule>(new Rule(rule_))) {
  }

  static void SetUpTestCase() {
    ifstream rule_file;
    rule_file.open("ydec/mahjong/data/rule.pb", istream::in | istream::binary);
    rule_.ParseFromIstream(&rule_file);
    rule_file.close();
  }

  void Verify(vector<string> expected_yaku,
              int expected_fu,
              int expected_han,
              int expected_yakuman,
              int expected_dora,
              int expected_uradora,
              const ScoreCalculatorResult& actual) {
    SCOPED_TRACE(actual.Utf8DebugString());

    sort(expected_yaku.begin(), expected_yaku.end());

    vector<string> actual_yaku;
    for (const Yaku& yaku : actual.yaku()) {
      actual_yaku.push_back(yaku.name());
    }
    sort(actual_yaku.begin(), actual_yaku.end());

    if (expected_yaku != actual_yaku) {
      FAIL() << "Expected = {" << ConcatStrings(expected_yaku) << "}\n"
             << "Actual = {" << ConcatStrings(actual_yaku) << "}";
    }

    ASSERT_EQ(expected_han, actual.fan());
    ASSERT_EQ(expected_fu, actual.fu());
    ASSERT_EQ(expected_yakuman, actual.yakuman());
    ASSERT_EQ(expected_dora, actual.dora());
    ASSERT_EQ(expected_uradora, actual.uradora());
  }

  ScoreCalculator score_calculator_;

 private:
  static Rule rule_;
};

Rule ScoreCalculatorTest::rule_;

TEST_F(ScoreCalculatorTest, TestCalculate) {
  Field field;
  field.set_wind(TileType::WIND_TON);
  field.add_dora(TileType::WIND_NAN);
  field.add_uradora(TileType::WIND_SHA);
  field.set_honba(0);

  Player player;
  player.set_wind(TileType::WIND_TON);

  Hand* hand = player.mutable_hand();
  hand->add_closed_tile(TileType::PINZU_1);
  hand->add_closed_tile(TileType::PINZU_1);
  hand->add_closed_tile(TileType::PINZU_1);
  hand->add_closed_tile(TileType::PINZU_2);
  hand->add_closed_tile(TileType::PINZU_2);
  hand->add_closed_tile(TileType::PINZU_2);
  hand->add_closed_tile(TileType::PINZU_2);
  hand->add_closed_tile(TileType::PINZU_3);
  hand->add_closed_tile(TileType::PINZU_3);
  hand->add_closed_tile(TileType::PINZU_3);
  hand->add_closed_tile(TileType::PINZU_3);
  hand->add_closed_tile(TileType::PINZU_4);
  hand->add_closed_tile(TileType::PINZU_4);
  hand->set_agari_tile(TileType::PINZU_1);
  hand->mutable_agari()->set_type(AgariType::RON);

  ScoreCalculatorResult result;
  score_calculator_.Calculate(field, player, &result);

  ASSERT_NO_FATAL_FAILURE(Verify({"二盃口", "清一色", "平和"},
                                 30 /* fu */,
                                 10 /* han */,
                                  0 /* yakuman */,
                                  0 /* dora */,
                                  0 /* uradora */,
                                 result));
}

TEST_F(ScoreCalculatorTest, TestCalculate_2) {
  Field field;
  field.set_wind(TileType::WIND_TON);
  field.add_dora(TileType::WIND_NAN);
  field.add_uradora(TileType::WIND_SHA);
  field.set_honba(0);

  Player player;
  player.set_wind(TileType::WIND_TON);

  Hand* hand = player.mutable_hand();
  hand->add_closed_tile(TileType::WIND_TON);
  hand->add_closed_tile(TileType::WIND_TON);
  hand->add_closed_tile(TileType::WIND_TON);
  hand->add_closed_tile(TileType::WIND_NAN);
  hand->add_closed_tile(TileType::WIND_NAN);
  hand->add_closed_tile(TileType::WIND_NAN);
  hand->add_closed_tile(TileType::WIND_SHA);
  hand->add_closed_tile(TileType::WIND_SHA);
  hand->add_closed_tile(TileType::WIND_SHA);
  hand->add_closed_tile(TileType::PINZU_2);
  hand->add_closed_tile(TileType::PINZU_3);
  hand->add_closed_tile(TileType::PINZU_4);
  hand->add_closed_tile(TileType::SOUZU_8);
  hand->set_agari_tile(TileType::SOUZU_8);
  hand->mutable_agari()->set_type(AgariType::RON);
  hand->set_richi_type(RichiType::NORMAL_RICHI);

  ScoreCalculatorResult result;
  score_calculator_.Calculate(field, player, &result);

  ASSERT_NO_FATAL_FAILURE(Verify({"立直", "三暗刻", "場風牌 東", "自風牌 東"},
                                 60 /* fu */,
                                 11 /* han */,
                                 0 /* yakuman */,
                                 3 /* dora */,
                                 3 /* uradora */,
                                 result));
}

TEST_F(ScoreCalculatorTest, TestCalculate_3) {
  Field field;
  field.set_wind(TileType::WIND_TON);
  field.add_dora(TileType::SOUZU_3);
  field.add_dora(TileType::WANZU_8);
  field.set_honba(0);

  Player player;
  player.set_wind(TileType::WIND_TON);

  Hand* hand = player.mutable_hand();
  hand->add_closed_tile(TileType::PINZU_2);
  hand->add_closed_tile(TileType::PINZU_3);
  hand->add_closed_tile(TileType::PINZU_4);
  hand->add_closed_tile(TileType::SOUZU_3);
  hand->add_closed_tile(TileType::SOUZU_5);
  hand->add_closed_tile(TileType::PINZU_8);
  hand->add_closed_tile(TileType::PINZU_8);

  Hand::Chii* chii = hand->add_chiied_tile();
  chii->add_tile(TileType::WANZU_7);
  chii->add_tile(TileType::WANZU_8);
  chii->add_tile(TileType::WANZU_9);

  Hand::Kan* kan = hand->add_kanned_tile();
  kan->set_tile(TileType::SANGEN_CHUN);
  kan->set_is_closed(true);

  hand->set_agari_tile(TileType::SOUZU_4);
  hand->mutable_agari()->set_type(AgariType::RON);

  ScoreCalculatorResult result;
  score_calculator_.Calculate(field, player, &result);

  ASSERT_NO_FATAL_FAILURE(Verify({"役牌 中"},
                                 60 /* fu */,
                                 3 /* han */,
                                 0 /* yakuman */,
                                 2 /* dora */,
                                 0 /* uradora */,
                                 result));
}

}  // namespace mahjong
}  // namespace ydec
