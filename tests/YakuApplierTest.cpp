#include <google/protobuf/text_format.h>
#include <gtest/gtest.h>
#include <stdexcept>

#include <iostream>

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
  virtual void SetUp() {
  }

  virtual void TearDown() {
  }

  YakuApplier _yakuApplier;
};


/**
 * Unit tests for YakuConditionValidator
 */
class YakuConditionValidatorTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
  }

  virtual void TearDown() {
  }
};

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
      "  type: KOUTSU"
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
      "}"
      "is_agarikei: false",
      &hand));

  YakuConditionValidator validator(condition, hand);
  EXPECT_EQ(YAKU_CONDITION_VALIDATOR_RESULT_NG_ALLOWED_TILE_CONDITION,
            validator.validate());
}


