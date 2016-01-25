#include <google/protobuf/text_format.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "src/YakuApplier.h"
#include "CommonTestUtil.h"

using namespace std;
using namespace ydec::mahjong;

using google::protobuf::TextFormat;

/**
 * Unit tests for YakuApplier
 */
class YakuApplierTest : public ::testing::Test {
 protected:
  static Rule RULE;
  YakuApplier yaku_applier_;

  static void SetUpTestCase() {
    ifstream rule_file;
    rule_file.open("res/raw/rule.pb", istream::in | istream::binary);
    RULE.ParseFromIstream(&rule_file);
    rule_file.close();
  }

  YakuApplierTest() : yaku_applier_(unique_ptr<Rule>(new Rule(RULE))) {}

 private:
  static string concatStrings(const vector<string>& strings) {
    stringstream ss;
    copy(strings.begin(), strings.end(), ostream_iterator<string>(ss, ", "));
    return ss.str();
  }

  static vector<string> getYakuNames(const YakuApplierResult& result) {
    vector<string> yakus;
    yakus.reserve(result.yaku_size());
    for (const Yaku& yaku : result.yaku()) {
      yakus.push_back(yaku.name());
    }
    return yakus;
  }

 protected:
  static void assertEquals(const vector<string>& expected,
                           const YakuApplierResult& actual) {
    vector<string> e = expected, a = getYakuNames(actual);
    sort(e.begin(), e.end());
    sort(a.begin(), a.end());
    if (e != a) {
      FAIL() << "Expected = {" << concatStrings(e) << "}\n"
          << "Actual = {" << concatStrings(a) << "}";
    }
  }
};

Rule YakuApplierTest::RULE;

TEST_F(YakuApplierTest, ApplyTest_Chitoitsu_1) {
  ParsedHand parsed_hand;
  CommonTestUtil::createAntoitsu(parsed_hand.add_element(), TileType::PINZU_1, true);
  CommonTestUtil::createAntoitsu(parsed_hand.add_element(), TileType::PINZU_3);
  CommonTestUtil::createAntoitsu(parsed_hand.add_element(), TileType::PINZU_5);
  CommonTestUtil::createAntoitsu(parsed_hand.add_element(), TileType::PINZU_7);
  CommonTestUtil::createAntoitsu(parsed_hand.add_element(), TileType::PINZU_9);
  CommonTestUtil::createAntoitsu(parsed_hand.add_element(), TileType::SOUZU_1);
  CommonTestUtil::createAntoitsu(parsed_hand.add_element(), TileType::SOUZU_3);
  parsed_hand.mutable_agari()->set_format(AgariFormat::CHITOITSU_AGARI);

  YakuApplierResult result;
  yaku_applier_.apply(PlayerType::DEALER,
                      RichiType::NO_RICHI,
                      TileType::WIND_TON /* field_wind */,
                      TileType::WIND_NAN /* player_wind */,
                      parsed_hand,
                      &result);
  assertEquals({"七対子"}, result);
}

TEST_F(YakuApplierTest, ApplyTest_Chitoitsu_2) {
  ParsedHand parsed_hand;
  CommonTestUtil::createAntoitsu(parsed_hand.add_element(), TileType::PINZU_1, true);
  CommonTestUtil::createAntoitsu(parsed_hand.add_element(), TileType::PINZU_1);
  CommonTestUtil::createAntoitsu(parsed_hand.add_element(), TileType::PINZU_5);
  CommonTestUtil::createAntoitsu(parsed_hand.add_element(), TileType::PINZU_7);
  CommonTestUtil::createAntoitsu(parsed_hand.add_element(), TileType::PINZU_9);
  CommonTestUtil::createAntoitsu(parsed_hand.add_element(), TileType::SOUZU_1);
  CommonTestUtil::createAntoitsu(parsed_hand.add_element(), TileType::SOUZU_3);
  parsed_hand.mutable_agari()->set_format(AgariFormat::CHITOITSU_AGARI);

  YakuApplierResult result;
  yaku_applier_.apply(PlayerType::DEALER,
                      RichiType::NO_RICHI,
                      TileType::WIND_TON /* field_wind */,
                      TileType::WIND_NAN /* player_wind */,
                      parsed_hand,
                      &result);
  assertEquals({}, result);  // We cannot use same tile kind for two-toitsu.
}

TEST_F(YakuApplierTest, ApplyTest_Regular_1) {
  ParsedHand parsed_hand;
  CommonTestUtil::createAnshuntsu(parsed_hand.add_element(), TileType::PINZU_1, 0);
  CommonTestUtil::createAnshuntsu(parsed_hand.add_element(), TileType::PINZU_1);
  CommonTestUtil::createAnshuntsu(parsed_hand.add_element(), TileType::PINZU_1);
  CommonTestUtil::createAnshuntsu(parsed_hand.add_element(), TileType::PINZU_1);
  CommonTestUtil::createAntoitsu(parsed_hand.add_element(), TileType::SOUZU_3);
  parsed_hand.mutable_agari()->set_format(AgariFormat::REGULAR_AGARI);
  parsed_hand.mutable_agari()->set_type(AgariType::TSUMO);
  parsed_hand.set_machi_type(MachiType::RYANMEN);

  YakuApplierResult result;
  yaku_applier_.apply(PlayerType::DEALER,
                      RichiType::NO_RICHI,
                      TileType::WIND_TON /* field_wind */,
                      TileType::WIND_NAN /* player_wind */,
                      parsed_hand,
                      &result);
  assertEquals({"門前清自摸和", "二盃口", "平和"}, result);
}

TEST_F(YakuApplierTest, ApplyTest_Regular_2) {
  ParsedHand parsed_hand;
  CommonTestUtil::createMinkoutsu(parsed_hand.add_element(), TileType::SOUZU_1, true);
  CommonTestUtil::createAnkoutsu(parsed_hand.add_element(), TileType::PINZU_2);
  CommonTestUtil::createAnkoutsu(parsed_hand.add_element(), TileType::PINZU_4);
  CommonTestUtil::createAnkoutsu(parsed_hand.add_element(), TileType::PINZU_6);
  CommonTestUtil::createAntoitsu(parsed_hand.add_element(), TileType::SOUZU_3);
  parsed_hand.mutable_agari()->set_format(AgariFormat::REGULAR_AGARI);
  parsed_hand.mutable_agari()->set_type(AgariType::RON);
  parsed_hand.set_machi_type(MachiType::SHABO);

  YakuApplierResult result;
  yaku_applier_.apply(PlayerType::DEALER,
                      RichiType::NO_RICHI,
                      TileType::WIND_TON /* field_wind */,
                      TileType::WIND_NAN /* player_wind */,
                      parsed_hand,
                      &result);
  assertEquals({"三暗刻", "対々和"}, result);
}

TEST_F(YakuApplierTest, ApplyTest_Regular_3) {
  ParsedHand parsed_hand;
  CommonTestUtil::createAnkoutsu(parsed_hand.add_element(), TileType::SOUZU_1, true);
  CommonTestUtil::createAnkoutsu(parsed_hand.add_element(), TileType::PINZU_2);
  CommonTestUtil::createAnkoutsu(parsed_hand.add_element(), TileType::PINZU_4);
  CommonTestUtil::createAnkoutsu(parsed_hand.add_element(), TileType::PINZU_6);
  CommonTestUtil::createAntoitsu(parsed_hand.add_element(), TileType::SOUZU_3);
  parsed_hand.mutable_agari()->set_format(AgariFormat::REGULAR_AGARI);
  parsed_hand.mutable_agari()->set_type(AgariType::TSUMO);
  parsed_hand.set_machi_type(MachiType::SHABO);

  YakuApplierResult result;
  yaku_applier_.apply(PlayerType::DEALER,
                      RichiType::NO_RICHI,
                      TileType::WIND_TON /* field_wind */,
                      TileType::WIND_NAN /* player_wind */,
                      parsed_hand,
                      &result);
  assertEquals({"四暗刻"}, result);
}

TEST_F(YakuApplierTest, ApplyTest_Regular_4) {
  ParsedHand parsed_hand;
  CommonTestUtil::createAnkoutsu(parsed_hand.add_element(), TileType::SOUZU_1);
  CommonTestUtil::createAnkoutsu(parsed_hand.add_element(), TileType::PINZU_2);
  CommonTestUtil::createAnkoutsu(parsed_hand.add_element(), TileType::PINZU_4);
  CommonTestUtil::createAnkoutsu(parsed_hand.add_element(), TileType::PINZU_6);
  CommonTestUtil::createAntoitsu(parsed_hand.add_element(), TileType::SOUZU_3, true);
  parsed_hand.mutable_agari()->set_format(AgariFormat::REGULAR_AGARI);
  parsed_hand.mutable_agari()->set_type(AgariType::TSUMO);
  parsed_hand.set_machi_type(MachiType::TANKI);

  YakuApplierResult result;
  yaku_applier_.apply(PlayerType::DEALER,
                      RichiType::NO_RICHI,
                      TileType::WIND_TON /* field_wind */,
                      TileType::WIND_NAN /* player_wind */,
                      parsed_hand,
                      &result);
  assertEquals({"四暗刻単騎待ち"}, result);
}

TEST_F(YakuApplierTest, ApplyTest_Regular_5) {
  ParsedHand parsed_hand;
  CommonTestUtil::createMinshuntsu(parsed_hand.add_element(), TileType::SOUZU_1, 0);
  CommonTestUtil::createAnshuntsu(parsed_hand.add_element(), TileType::SOUZU_4);
  CommonTestUtil::createAnshuntsu(parsed_hand.add_element(), TileType::SOUZU_7);
  CommonTestUtil::createAnshuntsu(parsed_hand.add_element(), TileType::SOUZU_1);
  CommonTestUtil::createAntoitsu(parsed_hand.add_element(), TileType::SOUZU_3);
  parsed_hand.mutable_agari()->set_format(AgariFormat::REGULAR_AGARI);
  parsed_hand.mutable_agari()->set_type(AgariType::RON);
  parsed_hand.set_machi_type(MachiType::RYANMEN);

  YakuApplierResult result;
  yaku_applier_.apply(PlayerType::DEALER,
                      RichiType::NO_RICHI,
                      TileType::WIND_TON /* field_wind */,
                      TileType::WIND_NAN /* player_wind */,
                      parsed_hand,
                      &result);
  assertEquals({"一気通貫", "一盃口", "平和", "清一色"}, result);
}

TEST_F(YakuApplierTest, ApplyTest_Regular_6) {
  ParsedHand parsed_hand;
  CommonTestUtil::createAnkantsu(parsed_hand.add_element(), TileType::WIND_TON);
  CommonTestUtil::createAnkantsu(parsed_hand.add_element(), TileType::WIND_NAN);
  CommonTestUtil::createAnkantsu(parsed_hand.add_element(), TileType::WIND_SHA);
  CommonTestUtil::createAnkantsu(parsed_hand.add_element(), TileType::WIND_PE);
  CommonTestUtil::createAntoitsu(parsed_hand.add_element(), TileType::SANGEN_HAKU, true);
  parsed_hand.mutable_agari()->set_format(AgariFormat::REGULAR_AGARI);
  parsed_hand.mutable_agari()->set_type(AgariType::TSUMO);
  parsed_hand.set_machi_type(MachiType::TANKI);

  YakuApplierResult result;
  yaku_applier_.apply(PlayerType::DEALER,
                      RichiType::NO_RICHI,
                      TileType::WIND_TON /* field_wind */,
                      TileType::WIND_NAN /* player_wind */,
                      parsed_hand,
                      &result);
  assertEquals({"四暗刻単騎待ち", "四槓子", "大四喜", "字一色"}, result);
}

TEST_F(YakuApplierTest, ApplyTest_Regular_7) {
  ParsedHand parsed_hand;
  CommonTestUtil::createAnshuntsu(parsed_hand.add_element(), TileType::PINZU_1, 0);
  CommonTestUtil::createAnshuntsu(parsed_hand.add_element(), TileType::PINZU_4);
  CommonTestUtil::createAnshuntsu(parsed_hand.add_element(), TileType::SOUZU_4);
  CommonTestUtil::createAnkoutsu(parsed_hand.add_element(), TileType::WIND_TON);
  CommonTestUtil::createAntoitsu(parsed_hand.add_element(), TileType::SOUZU_3);
  parsed_hand.mutable_agari()->set_format(AgariFormat::REGULAR_AGARI);
  parsed_hand.mutable_agari()->set_type(AgariType::TSUMO);
  parsed_hand.set_machi_type(MachiType::RYANMEN);

  YakuApplierResult result;
  yaku_applier_.apply(PlayerType::DEALER,
                      RichiType::NO_RICHI,
                      TileType::WIND_TON /* field_wind */,
                      TileType::WIND_NAN /* player_wind */,
                      parsed_hand,
                      &result);
  assertEquals({"門前清自摸和", "場風牌 東"}, result);
}

TEST_F(YakuApplierTest, ApplyTest_Regular_8) {
  ParsedHand parsed_hand;
  CommonTestUtil::createAnshuntsu(parsed_hand.add_element(), TileType::PINZU_1, 0);
  CommonTestUtil::createAnshuntsu(parsed_hand.add_element(), TileType::PINZU_4);
  CommonTestUtil::createAnshuntsu(parsed_hand.add_element(), TileType::SOUZU_4);
  CommonTestUtil::createAnkoutsu(parsed_hand.add_element(), TileType::WIND_TON);
  CommonTestUtil::createAntoitsu(parsed_hand.add_element(), TileType::SOUZU_3);
  parsed_hand.mutable_agari()->set_format(AgariFormat::REGULAR_AGARI);
  parsed_hand.mutable_agari()->set_type(AgariType::TSUMO);
  parsed_hand.set_machi_type(MachiType::RYANMEN);

  YakuApplierResult result;
  yaku_applier_.apply(PlayerType::DEALER,
                      RichiType::NO_RICHI,
                      TileType::WIND_NAN /* field_wind */,
                      TileType::WIND_TON /* player_wind */,
                      parsed_hand,
                      &result);
  assertEquals({"門前清自摸和", "自風牌 東"}, result);
}

TEST_F(YakuApplierTest, ApplyTest_Regular_9) {
  ParsedHand parsed_hand;
  CommonTestUtil::createAnshuntsu(parsed_hand.add_element(), TileType::PINZU_1, 0);
  CommonTestUtil::createAnshuntsu(parsed_hand.add_element(), TileType::PINZU_4);
  CommonTestUtil::createAnshuntsu(parsed_hand.add_element(), TileType::SOUZU_4);
  CommonTestUtil::createAnkoutsu(parsed_hand.add_element(), TileType::WIND_NAN);
  CommonTestUtil::createAntoitsu(parsed_hand.add_element(), TileType::SOUZU_3);
  parsed_hand.mutable_agari()->set_format(AgariFormat::REGULAR_AGARI);
  parsed_hand.mutable_agari()->set_type(AgariType::TSUMO);
  parsed_hand.set_machi_type(MachiType::RYANMEN);

  YakuApplierResult result;
  yaku_applier_.apply(PlayerType::DEALER,
                      RichiType::NO_RICHI,
                      TileType::WIND_NAN /* field_wind */,
                      TileType::WIND_NAN /* player_wind */,
                      parsed_hand,
                      &result);
  assertEquals({"門前清自摸和", "自風牌 南", "場風牌 南"}, result);
}

TEST_F(YakuApplierTest, ApplyTest_Regular_Pinhu_1) {
  ParsedHand parsed_hand;
  CommonTestUtil::createAnshuntsu(parsed_hand.add_element(), TileType::PINZU_1, 0);
  CommonTestUtil::createAnshuntsu(parsed_hand.add_element(), TileType::PINZU_4);
  CommonTestUtil::createAnshuntsu(parsed_hand.add_element(), TileType::SOUZU_4);
  CommonTestUtil::createAnshuntsu(parsed_hand.add_element(), TileType::SOUZU_7);
  CommonTestUtil::createAntoitsu(parsed_hand.add_element(), TileType::WANZU_3);
  parsed_hand.mutable_agari()->set_format(AgariFormat::REGULAR_AGARI);
  parsed_hand.mutable_agari()->set_type(AgariType::TSUMO);
  parsed_hand.set_machi_type(MachiType::RYANMEN);

  YakuApplierResult result;
  yaku_applier_.apply(PlayerType::DEALER,
                      RichiType::NO_RICHI,
                      TileType::WIND_TON /* field_wind */,
                      TileType::WIND_NAN /* player_wind */,
                      parsed_hand,
                      &result);
  assertEquals({"門前清自摸和", "平和"}, result);
}

TEST_F(YakuApplierTest, ApplyTest_Regular_Pinhu_2) {
  ParsedHand parsed_hand;
  CommonTestUtil::createAnshuntsu(parsed_hand.add_element(), TileType::PINZU_1, 1);
  CommonTestUtil::createAnshuntsu(parsed_hand.add_element(), TileType::PINZU_4);
  CommonTestUtil::createAnshuntsu(parsed_hand.add_element(), TileType::SOUZU_4);
  CommonTestUtil::createAnshuntsu(parsed_hand.add_element(), TileType::SOUZU_7);
  CommonTestUtil::createAntoitsu(parsed_hand.add_element(), TileType::WANZU_3);
  parsed_hand.mutable_agari()->set_format(AgariFormat::REGULAR_AGARI);
  parsed_hand.mutable_agari()->set_type(AgariType::TSUMO);
  parsed_hand.set_machi_type(MachiType::KANCHAN);

  YakuApplierResult result;
  yaku_applier_.apply(PlayerType::DEALER,
                      RichiType::NO_RICHI,
                      TileType::WIND_TON /* field_wind */,
                      TileType::WIND_NAN /* player_wind */,
                      parsed_hand,
                      &result);
  assertEquals({"門前清自摸和"}, result);
}

TEST_F(YakuApplierTest, ApplyTest_Regular_Pinhu_3) {
  ParsedHand parsed_hand;
  CommonTestUtil::createAnshuntsu(parsed_hand.add_element(), TileType::PINZU_1, 0);
  CommonTestUtil::createAnshuntsu(parsed_hand.add_element(), TileType::PINZU_4);
  CommonTestUtil::createAnshuntsu(parsed_hand.add_element(), TileType::SOUZU_4);
  CommonTestUtil::createAnshuntsu(parsed_hand.add_element(), TileType::SOUZU_7);
  CommonTestUtil::createAntoitsu(parsed_hand.add_element(), TileType::WIND_TON);
  parsed_hand.mutable_agari()->set_format(AgariFormat::REGULAR_AGARI);
  parsed_hand.mutable_agari()->set_type(AgariType::TSUMO);
  parsed_hand.set_machi_type(MachiType::RYANMEN);

  YakuApplierResult result;
  yaku_applier_.apply(PlayerType::DEALER,
                      RichiType::NO_RICHI,
                      TileType::WIND_TON /* field_wind */,
                      TileType::WIND_NAN /* player_wind */,
                      parsed_hand,
                      &result);
  assertEquals({"門前清自摸和"}, result);
}

TEST_F(YakuApplierTest, ApplyTest_Regular_Pinhu_4) {
  ParsedHand parsed_hand;
  CommonTestUtil::createAnshuntsu(parsed_hand.add_element(), TileType::PINZU_1, 0);
  CommonTestUtil::createAnshuntsu(parsed_hand.add_element(), TileType::PINZU_4);
  CommonTestUtil::createAnshuntsu(parsed_hand.add_element(), TileType::SOUZU_4);
  CommonTestUtil::createAnshuntsu(parsed_hand.add_element(), TileType::SOUZU_7);
  CommonTestUtil::createAntoitsu(parsed_hand.add_element(), TileType::WIND_NAN);
  parsed_hand.mutable_agari()->set_format(AgariFormat::REGULAR_AGARI);
  parsed_hand.mutable_agari()->set_type(AgariType::TSUMO);
  parsed_hand.set_machi_type(MachiType::RYANMEN);

  YakuApplierResult result;
  yaku_applier_.apply(PlayerType::DEALER,
                      RichiType::NO_RICHI,
                      TileType::WIND_TON /* field_wind */,
                      TileType::WIND_NAN /* player_wind */,
                      parsed_hand,
                      &result);
  assertEquals({"門前清自摸和"}, result);
}

TEST_F(YakuApplierTest, ApplyTest_Regular_Pinhu_5) {
  ParsedHand parsed_hand;
  CommonTestUtil::createAnshuntsu(parsed_hand.add_element(), TileType::PINZU_1, 0);
  CommonTestUtil::createAnshuntsu(parsed_hand.add_element(), TileType::PINZU_4);
  CommonTestUtil::createAnshuntsu(parsed_hand.add_element(), TileType::SOUZU_4);
  CommonTestUtil::createAnshuntsu(parsed_hand.add_element(), TileType::SOUZU_7);
  CommonTestUtil::createAntoitsu(parsed_hand.add_element(), TileType::WIND_TON);
  parsed_hand.mutable_agari()->set_format(AgariFormat::REGULAR_AGARI);
  parsed_hand.mutable_agari()->set_type(AgariType::TSUMO);
  parsed_hand.set_machi_type(MachiType::RYANMEN);

  YakuApplierResult result;
  yaku_applier_.apply(PlayerType::DEALER,
                      RichiType::NO_RICHI,
                      TileType::WIND_NAN /* field_wind */,
                      TileType::WIND_NAN /* player_wind */,
                      parsed_hand,
                      &result);
  assertEquals({"門前清自摸和", "平和"}, result);
}

/**
 * Unit tests for YakuConditionValidator
 */
class YakuConditionValidatorTest : public ::testing::Test {
 protected:
  YakuConditionValidatorResult::Type validate(const YakuCondition& condition,
                                              const PlayerType& player_type,
                                              const RichiType& richi_type,
                                              const TileType& field_wind,
                                              const TileType& player_wind,
                                              const ParsedHand& parsed_hand) {
    return YakuConditionValidator(condition,
                                  player_type,
                                  richi_type,
                                  field_wind,
                                  player_wind,
                                  parsed_hand).validate();
  }

  YakuConditionValidatorResult::Type validate(const YakuCondition& condition,
                                              const ParsedHand& parsed_hand) {
    return validate(condition,
                    PlayerType::DEALER,
                    RichiType::NO_RICHI,
                    TileType::WIND_TON /* field_wind */,
                    TileType::WIND_NAN /* player_wind */,
                    parsed_hand);
  }

  YakuConditionValidatorResult::Type validate(const YakuCondition& condition,
                                              const PlayerType& player_type) {
    return validate(condition,
                    player_type,
                    RichiType::NO_RICHI,
                    TileType::WIND_TON /* field_wind */,
                    TileType::WIND_NAN /* player_wind */,
                    ParsedHand::default_instance());
  }

  YakuConditionValidatorResult::Type validate(const YakuCondition& condition,
                                              const Agari& agari) {
    ParsedHand parsed_hand;
    parsed_hand.mutable_agari()->CopyFrom(agari);
    return validate(condition,
                    PlayerType::DEALER,
                    RichiType::NO_RICHI,
                    TileType::WIND_TON /* field_wind */,
                    TileType::WIND_NAN /* player_wind */,
                    parsed_hand);
  }

  YakuConditionValidatorResult::Type validate(const YakuCondition& condition,
                                              const RichiType& richi_type) {
      return validate(condition,
                      PlayerType::DEALER,
                      richi_type,
                      TileType::WIND_TON /* field_wind */,
                      TileType::WIND_NAN /* player_wind */,
                      ParsedHand::default_instance());
  }

  YakuConditionValidatorResult::Type validateFieldWind(const YakuCondition& condition,
                                                       const TileType& field_wind) {
      return validate(condition,
                      PlayerType::DEALER,
                      RichiType::NO_RICHI,
                      field_wind,
                      TileType::WIND_NAN /* player_wind */,
                      ParsedHand::default_instance());
  }

  YakuConditionValidatorResult::Type validatePlayerWind(const YakuCondition& condition,
                                                        const TileType& player_wind) {
      return validate(condition,
                      PlayerType::DEALER,
                      RichiType::NO_RICHI,
                      TileType::WIND_TON /* field_wind */,
                      player_wind,
                      ParsedHand::default_instance());
  }
};

/**
 * Tests for "Allowed Tile Condition"
 */
TEST_F(YakuConditionValidatorTest, ValidateTest_AllowedTileCondition_1) {
  YakuCondition condition;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "allowed_tile_condition {"
      "  required_tile: PINZU_1"
      "}",
      &condition));

  EXPECT_EQ(YakuConditionValidatorResult_Type_OK,
            validate(condition, ParsedHand::default_instance()));
}

TEST_F(YakuConditionValidatorTest, ValidateTest_AllowedTileCondition_2) {
  YakuCondition condition;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "allowed_tile_condition {"
      "  required_tile: PINZU_1"
      "}",
      &condition));

  ParsedHand hand;
  CommonTestUtil::createAnkoutsu(hand.add_element(), TileType::PINZU_1);

  EXPECT_EQ(YakuConditionValidatorResult_Type_OK,
            validate(condition, hand));
}

TEST_F(YakuConditionValidatorTest, ValidateTest_AllowedTileCondition_3) {
  YakuCondition condition;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "allowed_tile_condition {"
      "  required_tile: PINZU_1"
      "}",
      &condition));

  ParsedHand hand;
  CommonTestUtil::createAnshuntsu(hand.add_element(), TileType::PINZU_1);

  EXPECT_EQ(YakuConditionValidatorResult_Type_NG_ALLOWED_TILE_CONDITION,
            validate(condition, hand));
}

TEST_F(YakuConditionValidatorTest, ValidateTest_AllowedTileCondition_4) {
  YakuCondition condition;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "allowed_tile_condition {"
      "  required_tile: PINZU_3"
      "}"
      "allowed_tile_condition {"
      "  required_tile: PINZU_2"
      "}"
      "allowed_tile_condition {"
      "  required_variable_tile: VARIABLE_TILE_A"
      "}",
      &condition));

  ParsedHand hand;
  CommonTestUtil::createAnshuntsu(hand.add_element(), TileType::PINZU_1);

  EXPECT_EQ(YakuConditionValidatorResult_Type_OK,
            validate(condition, hand));
}

TEST_F(YakuConditionValidatorTest, ValidateTest_AllowedTileCondition_5) {
  YakuCondition condition;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "allowed_tile_condition {"
      "  required_tile: PINZU_3"
      "}"
      "allowed_tile_condition {"
      "  required_variable_tile: VARIABLE_TILE_A"
      "}",
      &condition));

  ParsedHand hand;
  CommonTestUtil::createAnshuntsu(hand.add_element(), TileType::PINZU_1);

  EXPECT_EQ(YakuConditionValidatorResult_Type_NG_ALLOWED_TILE_CONDITION,
            validate(condition, hand));
}

TEST_F(YakuConditionValidatorTest, ValidateTest_AllowedTileCondition_6) {
  YakuCondition condition;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "allowed_tile_condition {"
      "  required_variable_tile: VARIABLE_NUMBER_A"
      "}",
      &condition));

  ParsedHand hand;
  CommonTestUtil::createAntoitsu(hand.add_element(), TileType::PINZU_1);
  CommonTestUtil::createAntoitsu(hand.add_element(), TileType::SOUZU_1);
  CommonTestUtil::createAntoitsu(hand.add_element(), TileType::WANZU_1);

  EXPECT_EQ(YakuConditionValidatorResult_Type_OK,
            validate(condition, hand));
}

TEST_F(YakuConditionValidatorTest, ValidateTest_AllowedTileCondition_7) {
  YakuCondition condition;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "allowed_tile_condition {"
      "  required_variable_tile: VARIABLE_COLOR_A"
      "}",
      &condition));

  ParsedHand hand;
  CommonTestUtil::createAntoitsu(hand.add_element(), TileType::PINZU_1);
  CommonTestUtil::createAntoitsu(hand.add_element(), TileType::PINZU_2);

  EXPECT_EQ(YakuConditionValidatorResult_Type_OK,
            validate(condition, hand));
}

TEST_F(YakuConditionValidatorTest, ValidateTest_AllowedTileCondition_8) {
  YakuCondition condition;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "allowed_tile_condition {"
      "  required_variable_tile: VARIABLE_COLOR_A"
      "}",
      &condition));

  ParsedHand hand;
  CommonTestUtil::createAntoitsu(hand.add_element(), TileType::PINZU_1);
  CommonTestUtil::createAntoitsu(hand.add_element(), TileType::SANGEN_HAKU);

  EXPECT_EQ(YakuConditionValidatorResult_Type_NG_ALLOWED_TILE_CONDITION,
            validate(condition, hand));
}

TEST_F(YakuConditionValidatorTest, ValidateTest_AllowedTileCondition_9) {
  YakuCondition condition;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "allowed_tile_condition {"
      "  required_variable_tile: VARIABLE_COLOR_A"
      "}",
      &condition));

  ParsedHand hand;
  CommonTestUtil::createAntoitsu(hand.add_element(), TileType::SANGEN_HAKU);

  EXPECT_EQ(YakuConditionValidatorResult_Type_NG_ALLOWED_TILE_CONDITION,
            validate(condition, hand));
}

TEST_F(YakuConditionValidatorTest, ValidateTest_AllowedTileCondition_10) {
  YakuCondition condition;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "allowed_tile_condition {"
      "  required_variable_tile: VARIABLE_COLOR_A_OR_JIHAI"
      "}",
      &condition));

  ParsedHand hand;
  CommonTestUtil::createAntoitsu(hand.add_element(), TileType::PINZU_1);
  CommonTestUtil::createAntoitsu(hand.add_element(), TileType::PINZU_2);

  EXPECT_EQ(YakuConditionValidatorResult_Type_OK,
            validate(condition, hand));
}

TEST_F(YakuConditionValidatorTest, ValidateTest_AllowedTileCondition_11) {
  YakuCondition condition;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "allowed_tile_condition {"
      "  required_variable_tile: VARIABLE_COLOR_A_OR_JIHAI"
      "}",
      &condition));

  ParsedHand hand;
  CommonTestUtil::createAntoitsu(hand.add_element(), TileType::SANGEN_HAKU);
  CommonTestUtil::createAntoitsu(hand.add_element(), TileType::PINZU_1);

  EXPECT_EQ(YakuConditionValidatorResult_Type_OK,
            validate(condition, hand));
}

TEST_F(YakuConditionValidatorTest, ValidateTest_AllowedTileCondition_12) {
  YakuCondition condition;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "allowed_tile_condition {"
      "  required_variable_tile: VARIABLE_COLOR_A_OR_JIHAI"
      "}",
      &condition));

  ParsedHand hand;
  CommonTestUtil::createAntoitsu(hand.add_element(), TileType::SANGEN_HAKU);
  CommonTestUtil::createAntoitsu(hand.add_element(), TileType::WIND_TON);

  EXPECT_EQ(YakuConditionValidatorResult_Type_OK,
            validate(condition, hand));
}

TEST_F(YakuConditionValidatorTest, ValidateTest_AllowedTileCondition_13) {
  YakuCondition condition;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "allowed_tile_condition {"
      "  required_variable_tile: VARIABLE_COLOR_A_OR_JIHAI"
      "}",
      &condition));

  ParsedHand hand;
  CommonTestUtil::createAntoitsu(hand.add_element(), TileType::SANGEN_HAKU);
  CommonTestUtil::createAntoitsu(hand.add_element(), TileType::PINZU_1);
  CommonTestUtil::createAntoitsu(hand.add_element(), TileType::SOUZU_1);

  EXPECT_EQ(YakuConditionValidatorResult_Type_NG_ALLOWED_TILE_CONDITION,
            validate(condition, hand));
}

/**
 * Tests for "Disallowed tile condition"
 */
TEST_F(YakuConditionValidatorTest, ValidateTest_DisallowedTileCondition_1) {
  YakuCondition condition;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "disallowed_tile_condition {"
      "  required_tile: PINZU_8"
      "}",
      &condition));

  EXPECT_EQ(YakuConditionValidatorResult_Type_OK,
            validate(condition, ParsedHand::default_instance()));
}

TEST_F(YakuConditionValidatorTest, ValidateTest_DisallowedTileCondition_2) {
  YakuCondition condition;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "disallowed_tile_condition {"
      "  required_tile: PINZU_8"
      "}",
      &condition));

  ParsedHand hand;
  CommonTestUtil::createAnshuntsu(hand.add_element(), TileType::PINZU_1);

  EXPECT_EQ(YakuConditionValidatorResult_Type_OK,
            validate(condition, hand));
}

TEST_F(YakuConditionValidatorTest, ValidateTest_DisallowedTileCondition_3) {
  YakuCondition condition;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "disallowed_tile_condition {"
      "  required_tile: PINZU_8"
      "}",
      &condition));

  ParsedHand hand;
  CommonTestUtil::createAnshuntsu(hand.add_element(), TileType::PINZU_7);

  EXPECT_EQ(YakuConditionValidatorResult_Type_NG_DISALLOWED_TILE_CONDITION,
            validate(condition, hand));
}

/**
 * Tests for "Required tile condition"
 */
TEST_F(YakuConditionValidatorTest, ValidateTest_RequiredTileCondition_1) {
  YakuCondition condition;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "required_tile_condition {"
      "  required_tile: PINZU_8"
      "}",
      &condition));

  ParsedHand hand;
  CommonTestUtil::createAnshuntsu(hand.add_element(), TileType::SOUZU_1);

  EXPECT_EQ(YakuConditionValidatorResult_Type_NG_REQUIRED_TILE_CONDITION,
            validate(condition, hand));
}

TEST_F(YakuConditionValidatorTest, ValidateTest_RequiredTileCondition_2) {
  YakuCondition condition;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "required_tile_condition {"
      "  required_tile: SOUZU_1"
      "}"
      "required_tile_condition {"
      "  required_tile: SOUZU_3"
      "}",
      &condition));

  ParsedHand hand;
  CommonTestUtil::createAnshuntsu(hand.add_element(), TileType::SOUZU_1);

  EXPECT_EQ(YakuConditionValidatorResult_Type_OK,
            validate(condition, hand));
}

TEST_F(YakuConditionValidatorTest, ValidateTest_RequiredTileCondition_3) {
  YakuCondition condition;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "required_tile_condition {"
      "  required_state: AGARI_HAI"
      "}",
      &condition));

  ParsedHand hand;
  EXPECT_EQ(YakuConditionValidatorResult_Type_NG_REQUIRED_TILE_CONDITION,
            validate(condition, hand));

  Element* element = hand.add_element();
  Tile* tile = element->add_tile();
  EXPECT_EQ(YakuConditionValidatorResult_Type_NG_REQUIRED_TILE_CONDITION,
            validate(condition, hand));

  tile->add_state(TileState::AGARI_HAI_RON);
  EXPECT_EQ(YakuConditionValidatorResult_Type_OK,
            validate(condition, hand));
}

TEST_F(YakuConditionValidatorTest, ValidateTest_RequiredTileCondition_3_2) {
  YakuCondition condition;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "required_tile_condition {"
      "  required_state: AGARI_HAI_TSUMO"
      "}",
      &condition));

  ParsedHand hand;
  EXPECT_EQ(YakuConditionValidatorResult_Type_NG_REQUIRED_TILE_CONDITION,
            validate(condition, hand));

  Element* element = hand.add_element();
  Tile* tile = element->add_tile();
  EXPECT_EQ(YakuConditionValidatorResult_Type_NG_REQUIRED_TILE_CONDITION,
            validate(condition, hand));

  tile->add_state(TileState::AGARI_HAI_RON);
  EXPECT_EQ(YakuConditionValidatorResult_Type_NG_REQUIRED_TILE_CONDITION,
            validate(condition, hand));

  tile->add_state(TileState::AGARI_HAI_TSUMO);
  EXPECT_EQ(YakuConditionValidatorResult_Type_OK,
            validate(condition, hand));
}

TEST_F(YakuConditionValidatorTest, ValidateTest_RequiredTileCondition_4) {
  YakuCondition condition;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "required_tile_condition {"
      "  disallowed_state: AGARI_HAI"
      "}",
      &condition));

  ParsedHand hand;
  EXPECT_EQ(YakuConditionValidatorResult_Type_NG_REQUIRED_TILE_CONDITION,
            validate(condition, hand));

  Element* element = hand.add_element();
  Tile* tile = element->add_tile();
  EXPECT_EQ(YakuConditionValidatorResult_Type_OK,
            validate(condition, hand));

  tile->add_state(TileState::AGARI_HAI_RON);
  EXPECT_EQ(YakuConditionValidatorResult_Type_NG_REQUIRED_TILE_CONDITION,
            validate(condition, hand));

  element->add_tile();
  EXPECT_EQ(YakuConditionValidatorResult_Type_OK,
            validate(condition, hand));
}

TEST_F(YakuConditionValidatorTest, ValidateTest_RequiredTileCondition_5) {
  YakuCondition condition;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "required_tile_condition {"
      "  required_variable_tile: VARIABLE_TILE_A"
      "}"
      "required_tile_condition {"
      "  required_variable_tile: VARIABLE_TILE_B"
      "}",
      &condition));

  ParsedHand hand;
  EXPECT_EQ(YakuConditionValidatorResult_Type_NG_REQUIRED_TILE_CONDITION,
            validate(condition, hand));

  CommonTestUtil::createAntoitsu(hand.add_element(), TileType::SOUZU_1);
  EXPECT_EQ(YakuConditionValidatorResult_Type_NG_REQUIRED_TILE_CONDITION,
            validate(condition, hand));

  CommonTestUtil::createAntoitsu(hand.add_element(), TileType::SOUZU_2);
  EXPECT_EQ(YakuConditionValidatorResult_Type_OK,
            validate(condition, hand));
}

TEST_F(YakuConditionValidatorTest, ValidateTest_RequiredMachiType_1) {
  YakuCondition condition;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "required_machi_type: MACHI_2FU ",
      &condition));

  ParsedHand hand;
  hand.set_machi_type(MachiType::KANCHAN);

  EXPECT_EQ(YakuConditionValidatorResult_Type_OK,
            validate(condition, hand));
}

TEST_F(YakuConditionValidatorTest, ValidateTest_RequiredMachiType_2) {
  YakuCondition condition;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "required_machi_type: MACHI_2FU ",
      &condition));

  ParsedHand hand;
  hand.set_machi_type(MachiType::RYANMEN);

  EXPECT_EQ(YakuConditionValidatorResult_Type_NG_REQUIRED_MACHI_TYPE,
            validate(condition, hand));
}

TEST_F(YakuConditionValidatorTest, ValidateTest_RequiredPlayerType_1) {
  YakuCondition condition;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "required_player_type: DEALER ",
      &condition));

  EXPECT_EQ(YakuConditionValidatorResult_Type_OK,
            validate(condition, PlayerType::DEALER));
  EXPECT_EQ(YakuConditionValidatorResult_Type_NG_REQUIRED_PLAYER_TYPE,
            validate(condition, PlayerType::NON_DEALER));
}

TEST_F(YakuConditionValidatorTest, ValidateTest_RequiredAgariCondition_1) {
  YakuCondition condition;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "required_agari_condition {"
      "  required_type: RON"
      "}",
      &condition));

  Agari agari;
  agari.set_type(AgariType::RON);
  EXPECT_EQ(YakuConditionValidatorResult_Type_OK,
            validate(condition, agari));

  agari.set_type(AgariType::TSUMO);
  EXPECT_EQ(YakuConditionValidatorResult_Type_NG_REQUIRED_AGARI_CONDITION,
            validate(condition, agari));
}

TEST_F(YakuConditionValidatorTest, ValidateTest_RequiredAgariCondition_2) {
  YakuCondition condition;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "required_agari_condition {"
      "  required_state: SOKU"
      "  required_state: HAITEI"
      "}",
      &condition));

  Agari agari;
  EXPECT_EQ(YakuConditionValidatorResult_Type_NG_REQUIRED_AGARI_CONDITION,
            validate(condition, agari));

  agari.add_state(AgariState::SOKU);
  EXPECT_EQ(YakuConditionValidatorResult_Type_NG_REQUIRED_AGARI_CONDITION,
            validate(condition, agari));

  agari.add_state(AgariState::HAITEI);
  EXPECT_EQ(YakuConditionValidatorResult_Type_OK,
            validate(condition, agari));

  agari.add_state(AgariState::RINSHAN);
  EXPECT_EQ(YakuConditionValidatorResult_Type_OK,
            validate(condition, agari));
}

TEST_F(YakuConditionValidatorTest, ValidateTest_RequiredAgariCondition_3) {
  YakuCondition condition;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "required_agari_condition {"
      "  allowed_format: REGULAR_AGARI"
      "  allowed_format: CHITOITSU_AGARI"
      "}",
      &condition));

  Agari agari;
  EXPECT_EQ(YakuConditionValidatorResult_Type_NG_REQUIRED_AGARI_CONDITION,
            validate(condition, agari));

  agari.set_format(AgariFormat::IRREGULAR_AGARI);
  EXPECT_EQ(YakuConditionValidatorResult_Type_NG_REQUIRED_AGARI_CONDITION,
            validate(condition, agari));

  agari.set_format(AgariFormat::REGULAR_AGARI);
  EXPECT_EQ(YakuConditionValidatorResult_Type_OK,
            validate(condition, agari));

  agari.set_format(AgariFormat::CHITOITSU_AGARI);
  EXPECT_EQ(YakuConditionValidatorResult_Type_OK,
            validate(condition, agari));
}

TEST_F(YakuConditionValidatorTest, ValidateTest_RequiredRichiType_1) {
  YakuCondition condition;
  EXPECT_EQ(YakuConditionValidatorResult_Type_OK,
            validate(condition, RichiType::NO_RICHI));

  condition.set_required_richi_type(RichiType::RICHI);
  EXPECT_EQ(YakuConditionValidatorResult_Type_NG_REQUIRED_RICHI_TYPE,
            validate(condition, RichiType::NO_RICHI));
  EXPECT_EQ(YakuConditionValidatorResult_Type_OK,
            validate(condition, RichiType::RICHI));
  EXPECT_EQ(YakuConditionValidatorResult_Type_OK,
            validate(condition, RichiType::NORMAL_RICHI));
  EXPECT_EQ(YakuConditionValidatorResult_Type_OK,
            validate(condition, RichiType::DOUBLE_RICHI));
  EXPECT_EQ(YakuConditionValidatorResult_Type_OK,
            validate(condition, RichiType::OPENED_RICHI));

  condition.set_required_richi_type(RichiType::DOUBLE_RICHI);
  EXPECT_EQ(YakuConditionValidatorResult_Type_NG_REQUIRED_RICHI_TYPE,
            validate(condition, RichiType::NO_RICHI));
  EXPECT_EQ(YakuConditionValidatorResult_Type_NG_REQUIRED_RICHI_TYPE,
            validate(condition, RichiType::RICHI));
  EXPECT_EQ(YakuConditionValidatorResult_Type_NG_REQUIRED_RICHI_TYPE,
            validate(condition, RichiType::NORMAL_RICHI));
  EXPECT_EQ(YakuConditionValidatorResult_Type_OK,
            validate(condition, RichiType::DOUBLE_RICHI));
  EXPECT_EQ(YakuConditionValidatorResult_Type_NG_REQUIRED_RICHI_TYPE,
            validate(condition, RichiType::OPENED_RICHI));
}

TEST_F(YakuConditionValidatorTest, ValidateTest_RequiredFieldWind) {
  YakuCondition condition;
  EXPECT_EQ(YakuConditionValidatorResult_Type_OK,
            validateFieldWind(condition, TileType::WIND_TON));

  condition.set_required_field_wind(TileType::WIND_NAN);
  EXPECT_EQ(YakuConditionValidatorResult_Type_NG_REQUIRED_FIELD_WIND,
            validateFieldWind(condition, TileType::WIND_TON));
  EXPECT_EQ(YakuConditionValidatorResult_Type_OK,
            validateFieldWind(condition, TileType::WIND_NAN));
  EXPECT_EQ(YakuConditionValidatorResult_Type_NG_REQUIRED_FIELD_WIND,
            validateFieldWind(condition, TileType::WIND_SHA));
  EXPECT_EQ(YakuConditionValidatorResult_Type_NG_REQUIRED_FIELD_WIND,
            validateFieldWind(condition, TileType::WIND_PE));
}

TEST_F(YakuConditionValidatorTest, ValidateTest_RequiredPlayerWind) {
  YakuCondition condition;
  EXPECT_EQ(YakuConditionValidatorResult_Type_OK,
            validatePlayerWind(condition, TileType::WIND_TON));

  condition.set_required_player_wind(TileType::WIND_SHA);
  EXPECT_EQ(YakuConditionValidatorResult_Type_NG_REQUIRED_PLAYER_WIND,
            validatePlayerWind(condition, TileType::WIND_TON));
  EXPECT_EQ(YakuConditionValidatorResult_Type_NG_REQUIRED_PLAYER_WIND,
            validatePlayerWind(condition, TileType::WIND_NAN));
  EXPECT_EQ(YakuConditionValidatorResult_Type_OK,
            validatePlayerWind(condition, TileType::WIND_SHA));
  EXPECT_EQ(YakuConditionValidatorResult_Type_NG_REQUIRED_PLAYER_WIND,
            validatePlayerWind(condition, TileType::WIND_PE));
}
