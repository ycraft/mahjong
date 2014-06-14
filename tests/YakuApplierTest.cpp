#include <google/protobuf/text_format.h>
#include <gtest/gtest.h>
#include <stdexcept>

#include <iostream>
#include <fstream>

#include "src/YakuApplier.h"

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

  void createToitsu(Element* element, const TileType& type, bool is_agari_hai = false) {
    element->set_type(HandElementType::TOITSU);
    for (int i = 0; i < 2; ++i) {
      Tile* tile = element->add_tile();
      tile->set_is_agari_hai(is_agari_hai && i == 0);
      tile->set_is_tsumo(true);
      tile->set_type(type);
    }
  }
};

Rule YakuApplierTest::RULE;

// Disabled since YakuApplier is not implemented yet.
TEST_F(YakuApplierTest, DISABLED_ApplyTest_Chitoitsu) {
  ParsedHand parsed_hand;
  createToitsu(parsed_hand.add_element(), TileType::PINZU_1, true);
  createToitsu(parsed_hand.add_element(), TileType::PINZU_3);
  createToitsu(parsed_hand.add_element(), TileType::PINZU_5);
  createToitsu(parsed_hand.add_element(), TileType::PINZU_7);
  createToitsu(parsed_hand.add_element(), TileType::PINZU_9);
  createToitsu(parsed_hand.add_element(), TileType::SOUZU_1);
  createToitsu(parsed_hand.add_element(), TileType::SOUZU_3);
  parsed_hand.set_is_agarikei(true);

  YakuApplierResult result;
  yaku_applier_.apply(parsed_hand, &result);
  ASSERT_EQ(1, result.yaku_size());
  EXPECT_EQ("七対子", result.yaku(0).name());
}


/**
 * Unit tests for YakuConditionValidator
 */
class YakuConditionValidatorTest : public ::testing::Test {
};

/**
 * Tests for "Required Agari-kei"
 */
TEST_F(YakuConditionValidatorTest, ValidateTest_RequiredAgarikei_1) {
  YakuCondition condition;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "required_agarikei: true",
      &condition));

  ParsedHand hand;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "is_agarikei: false",
      &hand));

  YakuConditionValidator validator(condition, hand);
  EXPECT_EQ(YAKU_CONDITION_VALIDATOR_RESULT_NG_REQUIRED_AGARIKEI,
            validator.validate());
}

TEST_F(YakuConditionValidatorTest, ValidateTest_RequiredAgarikei_2) {
  YakuCondition condition;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "required_agarikei: true",
      &condition));

  ParsedHand hand;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "is_agarikei: true",
      &hand));

  YakuConditionValidator validator(condition, hand);
  EXPECT_EQ(YAKU_CONDITION_VALIDATOR_RESULT_OK,
            validator.validate());
}

TEST_F(YakuConditionValidatorTest, ValidateTest_RequiredAgarikei_3) {
  YakuCondition condition;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "required_agarikei: false",
      &condition));

  ParsedHand hand;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "is_agarikei: false",
      &hand));

  YakuConditionValidator validator(condition, hand);
  EXPECT_EQ(YAKU_CONDITION_VALIDATOR_RESULT_OK,
            validator.validate());
}

TEST_F(YakuConditionValidatorTest, ValidateTest_RequiredAgarikei_4) {
  YakuCondition condition;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "required_agarikei: false",
      &condition));

  ParsedHand hand;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "is_agarikei: true",
      &hand));

  YakuConditionValidator validator(condition, hand);
  EXPECT_EQ(YAKU_CONDITION_VALIDATOR_RESULT_OK,
            validator.validate());
}

/**
 * Tests for "Allowed Tile Condition"
 */
TEST_F(YakuConditionValidatorTest, ValidateTest_AllowedTileCondition_1) {
  YakuCondition condition;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "allowed_tile_condition {"
      "  required_tile: PINZU_1"
      "}"
      "required_agarikei: false",
      &condition));

  ParsedHand hand;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "is_agarikei: false",
      &hand));

  YakuConditionValidator validator(condition, hand);
  EXPECT_EQ(YAKU_CONDITION_VALIDATOR_RESULT_OK,
            validator.validate());
}

TEST_F(YakuConditionValidatorTest, ValidateTest_AllowedTileCondition_2) {
  YakuCondition condition;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "allowed_tile_condition {"
      "  required_tile: PINZU_1"
      "}"
      "required_agarikei: false",
      &condition));

  ParsedHand hand;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "element {"
      "  type: KOUTSU"
      "  tile {"
      "    type: PINZU_1"
      "    is_tsumo: true"
      "    is_agari_hai: false"
      "  }"
      "}"
      "is_agarikei: false",
      &hand));

  YakuConditionValidator validator(condition, hand);
  EXPECT_EQ(YAKU_CONDITION_VALIDATOR_RESULT_OK,
            validator.validate());
}

TEST_F(YakuConditionValidatorTest, ValidateTest_AllowedTileCondition_3) {
  YakuCondition condition;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "allowed_tile_condition {"
      "  required_tile: PINZU_1"
      "}"
      "required_agarikei: false",
      &condition));

  ParsedHand hand;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "element {"
      "  type: SHUNTSU"
      "  tile {"
      "    type: PINZU_1"
      "    is_tsumo: true"
      "    is_agari_hai: false"
      "  }"
      "  tile {"
      "    type: PINZU_2"
      "    is_tsumo: true"
      "    is_agari_hai: false"
      "  }"
      "  tile {"
      "    type: PINZU_3"
      "    is_tsumo: true"
      "    is_agari_hai: false"
      "  }"
      "}"
      "is_agarikei: false",
      &hand));

  YakuConditionValidator validator(condition, hand);
  EXPECT_EQ(YAKU_CONDITION_VALIDATOR_RESULT_NG_ALLOWED_TILE_CONDITION,
            validator.validate());
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
      "}"
      "required_agarikei: false",
      &condition));

  ParsedHand hand;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "element {"
      "  type: SHUNTSU"
      "  tile {"
      "    type: PINZU_1"
      "    is_tsumo: true"
      "    is_agari_hai: false"
      "  }"
      "  tile {"
      "    type: PINZU_2"
      "    is_tsumo: true"
      "    is_agari_hai: false"
      "  }"
      "  tile {"
      "    type: PINZU_3"
      "    is_tsumo: true"
      "    is_agari_hai: false"
      "  }"
      "}"
      "is_agarikei: false",
      &hand));

  YakuConditionValidator validator(condition, hand);
  EXPECT_EQ(YAKU_CONDITION_VALIDATOR_RESULT_OK,
            validator.validate());
}

TEST_F(YakuConditionValidatorTest, ValidateTest_AllowedTileCondition_5) {
  YakuCondition condition;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "allowed_tile_condition {"
      "  required_tile: PINZU_3"
      "}"
      "allowed_tile_condition {"
      "  required_variable_tile: VARIABLE_TILE_A"
      "}"
      "required_agarikei: false",
      &condition));

  ParsedHand hand;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "element {"
      "  type: SHUNTSU"
      "  tile {"
      "    type: PINZU_1"
      "    is_tsumo: true"
      "    is_agari_hai: false"
      "  }"
      "  tile {"
      "    type: PINZU_2"
      "    is_tsumo: true"
      "    is_agari_hai: false"
      "  }"
      "  tile {"
      "    type: PINZU_3"
      "    is_tsumo: true"
      "    is_agari_hai: false"
      "  }"
      "}"
      "is_agarikei: false",
      &hand));

  YakuConditionValidator validator(condition, hand);
  EXPECT_EQ(YAKU_CONDITION_VALIDATOR_RESULT_NG_ALLOWED_TILE_CONDITION,
            validator.validate());
}

TEST_F(YakuConditionValidatorTest, ValidateTest_AllowedTileCondition_6) {
  YakuCondition condition;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "allowed_tile_condition {"
      "  required_variable_tile: VARIABLE_NUMBER_A"
      "}"
      "required_agarikei: false",
      &condition));

  ParsedHand hand;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "element {"
      "  type: TOITSU"
      "  tile {"
      "    type: PINZU_1"
      "    is_tsumo: true"
      "    is_agari_hai: false"
      "  }"
      "  tile {"
      "    type: PINZU_1"
      "    is_tsumo: true"
      "    is_agari_hai: false"
      "  }"
      "}"
      "element {"
      "  type: TOITSU"
      "  tile {"
      "    type: SOUZU_1"
      "    is_tsumo: true"
      "    is_agari_hai: false"
      "  }"
      "  tile {"
      "    type: SOUZU_1"
      "    is_tsumo: true"
      "    is_agari_hai: false"
      "  }"
      "}"
      "element {"
      "  type: TOITSU"
      "  tile {"
      "    type: WANZU_1"
      "    is_tsumo: true"
      "    is_agari_hai: false"
      "  }"
      "  tile {"
      "    type: WANZU_1"
      "    is_tsumo: true"
      "    is_agari_hai: false"
      "  }"
      "}"
      "is_agarikei: false",
      &hand));

  YakuConditionValidator validator(condition, hand);
  EXPECT_EQ(YAKU_CONDITION_VALIDATOR_RESULT_OK,
            validator.validate());
}

TEST_F(YakuConditionValidatorTest, ValidateTest_AllowedTileCondition_7) {
  YakuCondition condition;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "allowed_tile_condition {"
      "  required_variable_tile: VARIABLE_COLOR_A"
      "}"
      "required_agarikei: false",
      &condition));

  ParsedHand hand;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "element {"
      "  type: TOITSU"
      "  tile {"
      "    type: PINZU_1"
      "    is_tsumo: true"
      "    is_agari_hai: false"
      "  }"
      "  tile {"
      "    type: PINZU_1"
      "    is_tsumo: true"
      "    is_agari_hai: false"
      "  }"
      "}"
      "element {"
      "  type: TOITSU"
      "  tile {"
      "    type: PINZU_2"
      "    is_tsumo: true"
      "    is_agari_hai: false"
      "  }"
      "  tile {"
      "    type: PINZU_2"
      "    is_tsumo: true"
      "    is_agari_hai: false"
      "  }"
      "}"
      "is_agarikei: false",
      &hand));

  YakuConditionValidator validator(condition, hand);
  EXPECT_EQ(YAKU_CONDITION_VALIDATOR_RESULT_OK,
            validator.validate());
}

TEST_F(YakuConditionValidatorTest, ValidateTest_AllowedTileCondition_8) {
  YakuCondition condition;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "allowed_tile_condition {"
      "  required_variable_tile: VARIABLE_COLOR_A"
      "}"
      "required_agarikei: false",
      &condition));

  ParsedHand hand;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "element {"
      "  type: TOITSU"
      "  tile {"
      "    type: PINZU_1"
      "    is_tsumo: true"
      "    is_agari_hai: false"
      "  }"
      "  tile {"
      "    type: PINZU_1"
      "    is_tsumo: true"
      "    is_agari_hai: false"
      "  }"
      "}"
      "element {"
      "  type: TOITSU"
      "  tile {"
      "    type: SANGEN_HAKU"
      "    is_tsumo: true"
      "    is_agari_hai: false"
      "  }"
      "  tile {"
      "    type: SANGEN_HAKU"
      "    is_tsumo: true"
      "    is_agari_hai: false"
      "  }"
      "}"
      "is_agarikei: false",
      &hand));

  YakuConditionValidator validator(condition, hand);
  EXPECT_EQ(YAKU_CONDITION_VALIDATOR_RESULT_NG_ALLOWED_TILE_CONDITION,
            validator.validate());
}

TEST_F(YakuConditionValidatorTest, ValidateTest_AllowedTileCondition_9) {
  YakuCondition condition;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "allowed_tile_condition {"
      "  required_variable_tile: VARIABLE_COLOR_A"
      "}"
      "required_agarikei: false",
      &condition));

  ParsedHand hand;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "element {"
      "  type: TOITSU"
      "  tile {"
      "    type: SANGEN_HAKU"
      "    is_tsumo: true"
      "    is_agari_hai: false"
      "  }"
      "  tile {"
      "    type: SANGEN_HAKU"
      "    is_tsumo: true"
      "    is_agari_hai: false"
      "  }"
      "}"
      "is_agarikei: false",
      &hand));

  YakuConditionValidator validator(condition, hand);
  EXPECT_EQ(YAKU_CONDITION_VALIDATOR_RESULT_NG_ALLOWED_TILE_CONDITION,
            validator.validate());
}

TEST_F(YakuConditionValidatorTest, ValidateTest_AllowedTileCondition_10) {
  YakuCondition condition;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "allowed_tile_condition {"
      "  required_variable_tile: VARIABLE_COLOR_A_OR_JIHAI"
      "}"
      "required_agarikei: false",
      &condition));

  ParsedHand hand;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "element {"
      "  type: TOITSU"
      "  tile {"
      "    type: PINZU_1"
      "    is_tsumo: true"
      "    is_agari_hai: false"
      "  }"
      "  tile {"
      "    type: PINZU_1"
      "    is_tsumo: true"
      "    is_agari_hai: false"
      "  }"
      "}"
      "element {"
      "  type: TOITSU"
      "  tile {"
      "    type: PINZU_2"
      "    is_tsumo: true"
      "    is_agari_hai: false"
      "  }"
      "  tile {"
      "    type: PINZU_2"
      "    is_tsumo: true"
      "    is_agari_hai: false"
      "  }"
      "}"
      "is_agarikei: false",
      &hand));

  YakuConditionValidator validator(condition, hand);
  EXPECT_EQ(YAKU_CONDITION_VALIDATOR_RESULT_OK,
            validator.validate());
}

TEST_F(YakuConditionValidatorTest, ValidateTest_AllowedTileCondition_11) {
  YakuCondition condition;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "allowed_tile_condition {"
      "  required_variable_tile: VARIABLE_COLOR_A_OR_JIHAI"
      "}"
      "required_agarikei: false",
      &condition));

  ParsedHand hand;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "element {"
      "  type: TOITSU"
      "  tile {"
      "    type: SANGEN_HAKU"
      "    is_tsumo: true"
      "    is_agari_hai: false"
      "  }"
      "  tile {"
      "    type: SANGEN_HAKU"
      "    is_tsumo: true"
      "    is_agari_hai: false"
      "  }"
      "}"
      "element {"
      "  type: TOITSU"
      "  tile {"
      "    type: PINZU_1"
      "    is_tsumo: true"
      "    is_agari_hai: false"
      "  }"
      "  tile {"
      "    type: PINZU_1"
      "    is_tsumo: true"
      "    is_agari_hai: false"
      "  }"
      "}"
      "is_agarikei: false",
      &hand));

  YakuConditionValidator validator(condition, hand);
  EXPECT_EQ(YAKU_CONDITION_VALIDATOR_RESULT_OK,
            validator.validate());
}

TEST_F(YakuConditionValidatorTest, ValidateTest_AllowedTileCondition_12) {
  YakuCondition condition;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "allowed_tile_condition {"
      "  required_variable_tile: VARIABLE_COLOR_A_OR_JIHAI"
      "}"
      "required_agarikei: false",
      &condition));

  ParsedHand hand;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "element {"
      "  type: TOITSU"
      "  tile {"
      "    type: SANGEN_HAKU"
      "    is_tsumo: true"
      "    is_agari_hai: false"
      "  }"
      "  tile {"
      "    type: SANGEN_HAKU"
      "    is_tsumo: true"
      "    is_agari_hai: false"
      "  }"
      "}"
      "element {"
      "  type: TOITSU"
      "  tile {"
      "    type: WIND_TON"
      "    is_tsumo: true"
      "    is_agari_hai: false"
      "  }"
      "  tile {"
      "    type: WIND_TON"
      "    is_tsumo: true"
      "    is_agari_hai: false"
      "  }"
      "}"
      "is_agarikei: false",
      &hand));

  YakuConditionValidator validator(condition, hand);
  EXPECT_EQ(YAKU_CONDITION_VALIDATOR_RESULT_OK,
            validator.validate());
}

TEST_F(YakuConditionValidatorTest, ValidateTest_AllowedTileCondition_13) {
  YakuCondition condition;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "allowed_tile_condition {"
      "  required_variable_tile: VARIABLE_COLOR_A_OR_JIHAI"
      "}"
      "required_agarikei: false",
      &condition));

  ParsedHand hand;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "element {"
      "  type: TOITSU"
      "  tile {"
      "    type: SANGEN_HAKU"
      "    is_tsumo: true"
      "    is_agari_hai: false"
      "  }"
      "  tile {"
      "    type: SANGEN_HAKU"
      "    is_tsumo: true"
      "    is_agari_hai: false"
      "  }"
      "}"
      "element {"
      "  type: TOITSU"
      "  tile {"
      "    type: PINZU_1"
      "    is_tsumo: true"
      "    is_agari_hai: false"
      "  }"
      "  tile {"
      "    type: PINZU_1"
      "    is_tsumo: true"
      "    is_agari_hai: false"
      "  }"
      "}"
      "element {"
      "  type: TOITSU"
      "  tile {"
      "    type: SOUZU_1"
      "    is_tsumo: true"
      "    is_agari_hai: false"
      "  }"
      "  tile {"
      "    type: SOUZU_1"
      "    is_tsumo: true"
      "    is_agari_hai: false"
      "  }"
      "}"
      "is_agarikei: false",
      &hand));

  YakuConditionValidator validator(condition, hand);
  EXPECT_EQ(YAKU_CONDITION_VALIDATOR_RESULT_NG_ALLOWED_TILE_CONDITION,
            validator.validate());
}

/**
 * Tests for "Disallowed tile condition"
 */
TEST_F(YakuConditionValidatorTest, ValidateTest_DisallowedTileCondition_1) {
  YakuCondition condition;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "disallowed_tile_condition {"
      "  required_tile: PINZU_8"
      "}"
      "required_agarikei: false",
      &condition));

  ParsedHand hand;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "is_agarikei: false",
      &hand));

  YakuConditionValidator validator(condition, hand);
  EXPECT_EQ(YAKU_CONDITION_VALIDATOR_RESULT_OK,
            validator.validate());
}

TEST_F(YakuConditionValidatorTest, ValidateTest_DisallowedTileCondition_2) {
  YakuCondition condition;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "disallowed_tile_condition {"
      "  required_tile: PINZU_8"
      "}"
      "required_agarikei: false",
      &condition));

  ParsedHand hand;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "element {"
      "  type: SHUNTSU"
      "  tile {"
      "    type: PINZU_1"
      "    is_tsumo: true"
      "    is_agari_hai: false"
      "  }"
      "  tile {"
      "    type: PINZU_2"
      "    is_tsumo: true"
      "    is_agari_hai: false"
      "  }"
      "  tile {"
      "    type: PINZU_3"
      "    is_tsumo: true"
      "    is_agari_hai: false"
      "  }"
      "}"
      "is_agarikei: false",
      &hand));

  YakuConditionValidator validator(condition, hand);
  EXPECT_EQ(YAKU_CONDITION_VALIDATOR_RESULT_OK,
            validator.validate());
}

TEST_F(YakuConditionValidatorTest, ValidateTest_DisallowedTileCondition_3) {
  YakuCondition condition;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "disallowed_tile_condition {"
      "  required_tile: PINZU_8"
      "}"
      "required_agarikei: false",
      &condition));

  ParsedHand hand;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "element {"
      "  type: SHUNTSU"
      "  tile {"
      "    type: PINZU_7"
      "    is_tsumo: true"
      "    is_agari_hai: false"
      "  }"
      "  tile {"
      "    type: PINZU_8"
      "    is_tsumo: true"
      "    is_agari_hai: false"
      "  }"
      "  tile {"
      "    type: PINZU_9"
      "    is_tsumo: true"
      "    is_agari_hai: false"
      "  }"
      "}"
      "is_agarikei: false",
      &hand));

  YakuConditionValidator validator(condition, hand);
  EXPECT_EQ(YAKU_CONDITION_VALIDATOR_RESULT_NG_DISALLOWED_TILE_CONDITION,
            validator.validate());
}

/**
 * Tests for "Required tile condition"
 */
TEST_F(YakuConditionValidatorTest, ValidateTest_RequiredTileCondition_1) {
  YakuCondition condition;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "required_tile_condition {"
      "  required_tile: PINZU_8"
      "}"
      "required_agarikei: false",
      &condition));

  ParsedHand hand;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "element {"
      "  type: SHUNTSU"
      "  tile {"
      "    type: SOUZU_1"
      "    is_tsumo: true"
      "    is_agari_hai: false"
      "  }"
      "  tile {"
      "    type: SOUZU_2"
      "    is_tsumo: true"
      "    is_agari_hai: false"
      "  }"
      "  tile {"
      "    type: SOUZU_3"
      "    is_tsumo: true"
      "    is_agari_hai: false"
      "  }"
      "}"
      "is_agarikei: false",
      &hand));

  YakuConditionValidator validator(condition, hand);
  EXPECT_EQ(YAKU_CONDITION_VALIDATOR_RESULT_NG_REQUIRED_TILE_CONDITION,
            validator.validate());
}

TEST_F(YakuConditionValidatorTest, ValidateTest_RequiredTileCondition_2) {
  YakuCondition condition;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "required_tile_condition {"
      "  required_tile: SOUZU_1"
      "}"
      "required_tile_condition {"
      "  required_tile: SOUZU_3"
      "}"
      "required_agarikei: false",
      &condition));

  ParsedHand hand;
  EXPECT_TRUE(TextFormat::ParseFromString(
      "element {"
      "  type: SHUNTSU"
      "  tile {"
      "    type: SOUZU_1"
      "    is_tsumo: true"
      "    is_agari_hai: false"
      "  }"
      "  tile {"
      "    type: SOUZU_2"
      "    is_tsumo: true"
      "    is_agari_hai: false"
      "  }"
      "  tile {"
      "    type: SOUZU_3"
      "    is_tsumo: true"
      "    is_agari_hai: false"
      "  }"
      "}"
      "is_agarikei: false",
      &hand));

  YakuConditionValidator validator(condition, hand);
  EXPECT_EQ(YAKU_CONDITION_VALIDATOR_RESULT_OK,
            validator.validate());
}
