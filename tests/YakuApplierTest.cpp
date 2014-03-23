#include <gtest/gtest.h>
#include <stdexcept>

#include <iostream>

#include "src/YakuApplier.h"

using namespace ydec::mahjong;
using namespace ydec::msc;

using namespace std;

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

TEST_F(YakuConditionValidatorTest, ValidateTest_EmptyCondition_1) {
  YakuCondition condition;
  ParsedHand hand;
  YakuConditionValidator validator(condition, hand);
  EXPECT_EQ(YAKU_CONDITION_VALIDATOR_RESULT_NG_REQUIRED_AGARIKEI,
            validator.validate());
}

TEST_F(YakuConditionValidatorTest, ValidateTest_EmptyCondition_2) {
  YakuCondition condition;
  condition.set_required_agarikei(false);
  ParsedHand hand;
  YakuConditionValidator validator(condition, hand);
  EXPECT_EQ(YAKU_CONDITION_VALIDATOR_RESULT_OK,
            validator.validate());
}

