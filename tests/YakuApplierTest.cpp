#include <google/protobuf/text_format.h>
#include <gtest/gtest.h>
#include <stdexcept>

#include <iostream>
#include <fstream>

#include "src/YakuApplier.h"
#include "CommonTestUtil.h"

using namespace ydec::mahjong;
using namespace ydec::msc;

using namespace std;

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
};

Rule YakuApplierTest::RULE;

// Disabled since YakuApplier is not implemented yet.
TEST_F(YakuApplierTest, DISABLED_ApplyTest_Chitoitsu) {
  ParsedHand parsed_hand;
  CommonTestUtil::createToitsu(parsed_hand.add_element(), TileType::PINZU_1, true);
  CommonTestUtil::createToitsu(parsed_hand.add_element(), TileType::PINZU_3);
  CommonTestUtil::createToitsu(parsed_hand.add_element(), TileType::PINZU_5);
  CommonTestUtil::createToitsu(parsed_hand.add_element(), TileType::PINZU_7);
  CommonTestUtil::createToitsu(parsed_hand.add_element(), TileType::PINZU_9);
  CommonTestUtil::createToitsu(parsed_hand.add_element(), TileType::SOUZU_1);
  CommonTestUtil::createToitsu(parsed_hand.add_element(), TileType::SOUZU_3);
  parsed_hand.mutable_agari()->set_format(AgariFormat::CHITOITSU_AGARI);

  YakuApplierResult result;
  yaku_applier_.apply(PlayerType::DEALER,
                      parsed_hand,
                      &result);
  ASSERT_EQ(1, result.yaku_size());
  EXPECT_EQ("七対子", result.yaku(0).name());
}


/**
 * Unit tests for YakuConditionValidator
 */
class YakuConditionValidatorTest : public ::testing::Test {
 protected:
  YakuConditionValidatorResult validate(const YakuCondition& condition,
                                        const PlayerType& playerType,
                                        const ParsedHand& parsed_hand) {
    return YakuConditionValidator(condition,
                                  playerType,
                                  parsed_hand).validate();
  }

  YakuConditionValidatorResult validate(const YakuCondition& condition,
                                        const ParsedHand& parsed_hand) {
    return validate(condition,
                    PlayerType::DEALER,
                    parsed_hand);
  }

  YakuConditionValidatorResult validate(const YakuCondition& condition,
                                        const PlayerType& player_type) {
    return validate(condition,
                    player_type,
                    ParsedHand::default_instance());
  }

  YakuConditionValidatorResult validate(const YakuCondition& condition,
                                        const Agari& agari) {
    ParsedHand parsed_hand;
    parsed_hand.mutable_agari()->CopyFrom(agari);
    return validate(condition,
                    PlayerType::DEALER,
                    parsed_hand);
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

  EXPECT_EQ(YAKU_CONDITION_VALIDATOR_RESULT_OK,
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
  CommonTestUtil::createAnkou(hand.add_element(), TileType::PINZU_1);

  EXPECT_EQ(YAKU_CONDITION_VALIDATOR_RESULT_OK,
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

  EXPECT_EQ(YAKU_CONDITION_VALIDATOR_RESULT_NG_ALLOWED_TILE_CONDITION,
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

  EXPECT_EQ(YAKU_CONDITION_VALIDATOR_RESULT_OK,
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

  EXPECT_EQ(YAKU_CONDITION_VALIDATOR_RESULT_NG_ALLOWED_TILE_CONDITION,
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
  CommonTestUtil::createToitsu(hand.add_element(), TileType::PINZU_1);
  CommonTestUtil::createToitsu(hand.add_element(), TileType::SOUZU_1);
  CommonTestUtil::createToitsu(hand.add_element(), TileType::WANZU_1);

  EXPECT_EQ(YAKU_CONDITION_VALIDATOR_RESULT_OK,
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
  CommonTestUtil::createToitsu(hand.add_element(), TileType::PINZU_1);
  CommonTestUtil::createToitsu(hand.add_element(), TileType::PINZU_2);

  EXPECT_EQ(YAKU_CONDITION_VALIDATOR_RESULT_OK,
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
  CommonTestUtil::createToitsu(hand.add_element(), TileType::PINZU_1);
  CommonTestUtil::createToitsu(hand.add_element(), TileType::SANGEN_HAKU);

  EXPECT_EQ(YAKU_CONDITION_VALIDATOR_RESULT_NG_ALLOWED_TILE_CONDITION,
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
  CommonTestUtil::createToitsu(hand.add_element(), TileType::SANGEN_HAKU);

  EXPECT_EQ(YAKU_CONDITION_VALIDATOR_RESULT_NG_ALLOWED_TILE_CONDITION,
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
  CommonTestUtil::createToitsu(hand.add_element(), TileType::PINZU_1);
  CommonTestUtil::createToitsu(hand.add_element(), TileType::PINZU_2);

  EXPECT_EQ(YAKU_CONDITION_VALIDATOR_RESULT_OK,
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
  CommonTestUtil::createToitsu(hand.add_element(), TileType::SANGEN_HAKU);
  CommonTestUtil::createToitsu(hand.add_element(), TileType::PINZU_1);

  EXPECT_EQ(YAKU_CONDITION_VALIDATOR_RESULT_OK,
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
  CommonTestUtil::createToitsu(hand.add_element(), TileType::SANGEN_HAKU);
  CommonTestUtil::createToitsu(hand.add_element(), TileType::WIND_TON);

  EXPECT_EQ(YAKU_CONDITION_VALIDATOR_RESULT_OK,
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
  CommonTestUtil::createToitsu(hand.add_element(), TileType::SANGEN_HAKU);
  CommonTestUtil::createToitsu(hand.add_element(), TileType::PINZU_1);
  CommonTestUtil::createToitsu(hand.add_element(), TileType::SOUZU_1);

  EXPECT_EQ(YAKU_CONDITION_VALIDATOR_RESULT_NG_ALLOWED_TILE_CONDITION,
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

  EXPECT_EQ(YAKU_CONDITION_VALIDATOR_RESULT_OK,
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

  EXPECT_EQ(YAKU_CONDITION_VALIDATOR_RESULT_OK,
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

  EXPECT_EQ(YAKU_CONDITION_VALIDATOR_RESULT_NG_DISALLOWED_TILE_CONDITION,
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

  EXPECT_EQ(YAKU_CONDITION_VALIDATOR_RESULT_NG_REQUIRED_TILE_CONDITION,
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

  EXPECT_EQ(YAKU_CONDITION_VALIDATOR_RESULT_OK,
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
  EXPECT_EQ(YAKU_CONDITION_VALIDATOR_RESULT_NG_REQUIRED_TILE_CONDITION,
            validate(condition, hand));

  Element* element = hand.add_element();
  Tile* tile = element->add_tile();
  tile->add_state(TileState::DORA_HAI);
  EXPECT_EQ(YAKU_CONDITION_VALIDATOR_RESULT_NG_REQUIRED_TILE_CONDITION,
            validate(condition, hand));

  tile->add_state(TileState::AGARI_HAI_RON);
  EXPECT_EQ(YAKU_CONDITION_VALIDATOR_RESULT_OK,
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
  EXPECT_EQ(YAKU_CONDITION_VALIDATOR_RESULT_NG_REQUIRED_TILE_CONDITION,
            validate(condition, hand));

  Element* element = hand.add_element();
  Tile* tile = element->add_tile();
  tile->add_state(TileState::DORA_HAI);
  EXPECT_EQ(YAKU_CONDITION_VALIDATOR_RESULT_OK,
            validate(condition, hand));

  tile->add_state(TileState::AGARI_HAI_RON);
  EXPECT_EQ(YAKU_CONDITION_VALIDATOR_RESULT_NG_REQUIRED_TILE_CONDITION,
            validate(condition, hand));

  element->add_tile();
  EXPECT_EQ(YAKU_CONDITION_VALIDATOR_RESULT_OK,
            validate(condition, hand));
}

TEST_F(YakuConditionValidatorTest, ValidateTest_RequiredMachiType_1) {
  YakuCondition condition;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "required_machi_type: MACHI_2FU ",
      &condition));

  ParsedHand hand;
  hand.set_machi_type(MachiType::KANCHAN);

  EXPECT_EQ(YAKU_CONDITION_VALIDATOR_RESULT_OK,
            validate(condition, hand));
}

TEST_F(YakuConditionValidatorTest, ValidateTest_RequiredMachiType_2) {
  YakuCondition condition;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "required_machi_type: MACHI_2FU ",
      &condition));

  ParsedHand hand;
  hand.set_machi_type(MachiType::RYANMEN);

  EXPECT_EQ(YAKU_CONDITION_VALIDATOR_RESULT_NG_REQUIRED_MACHI_TYPE,
            validate(condition, hand));
}

TEST_F(YakuConditionValidatorTest, ValidateTest_RequiredPlayerType_1) {
  YakuCondition condition;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "required_player_type: DEALER ",
      &condition));

  EXPECT_EQ(YAKU_CONDITION_VALIDATOR_RESULT_OK,
            validate(condition, PlayerType::DEALER));
  EXPECT_EQ(YAKU_CONDITION_VALIDATOR_RESULT_NG_REQUIRED_PLAYER_TYPE,
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
  EXPECT_EQ(YAKU_CONDITION_VALIDATOR_RESULT_OK,
            validate(condition, agari));

  agari.set_type(AgariType::TSUMO);
  EXPECT_EQ(YAKU_CONDITION_VALIDATOR_RESULT_NG_REQUIRED_AGARI_CONDITION,
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
  EXPECT_EQ(YAKU_CONDITION_VALIDATOR_RESULT_NG_REQUIRED_AGARI_CONDITION,
            validate(condition, agari));

  agari.add_state(AgariState::SOKU);
  EXPECT_EQ(YAKU_CONDITION_VALIDATOR_RESULT_NG_REQUIRED_AGARI_CONDITION,
            validate(condition, agari));

  agari.add_state(AgariState::HAITEI);
  EXPECT_EQ(YAKU_CONDITION_VALIDATOR_RESULT_OK,
            validate(condition, agari));

  agari.add_state(AgariState::RINSHAN);
  EXPECT_EQ(YAKU_CONDITION_VALIDATOR_RESULT_OK,
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
  EXPECT_EQ(YAKU_CONDITION_VALIDATOR_RESULT_NG_REQUIRED_AGARI_CONDITION,
            validate(condition, agari));

  agari.set_format(AgariFormat::IRREGULAR_AGARI);
  EXPECT_EQ(YAKU_CONDITION_VALIDATOR_RESULT_NG_REQUIRED_AGARI_CONDITION,
            validate(condition, agari));

  agari.set_format(AgariFormat::REGULAR_AGARI);
  EXPECT_EQ(YAKU_CONDITION_VALIDATOR_RESULT_OK,
            validate(condition, agari));

  agari.set_format(AgariFormat::CHITOITSU_AGARI);
  EXPECT_EQ(YAKU_CONDITION_VALIDATOR_RESULT_OK,
            validate(condition, agari));
}
