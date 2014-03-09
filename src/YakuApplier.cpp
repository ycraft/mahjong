#include "YakuApplier.h"

using google::protobuf::RepeatedPtrField;

namespace ydec {

using namespace mahjong;

namespace msc{

YakuApplier::YakuApplier(Rule* rule)
    : rule_(rule){
}

YakuApplier::~YakuApplier() {
}

void YakuApplier::apply(const ParsedHand& parsed_hand, YakuApplierResult* result) const {
  for (const Yaku& yaku : rule_->yaku()) {
  }
}


YakuConditionValidator::YakuConditionValidator(const YakuCondition& condition,
                                               const ParsedHand& parsed_hand)
    : condition_(condition),
      parsed_hand_(parsed_hand){
}

bool YakuConditionValidator::validate() {
  return true;
}

bool YakuConditionValidator::validateAllowedTileCondition(
    const RepeatedPtrField<TileCondition >& conditions,
    const TileType& tile) {
  // Check un-variable tile conditions first.
  for (const TileCondition& condition : conditions) {
    if (condition.has_required_variable_tile()) {
      continue;
    }
    if (condition.has_required_tile()) {
      if ((tile & condition.required_tile()) == condition.required_tile()) {
        return true;
      }
    }
  }

  // Check variable tile conditions
  for (const TileCondition& condition : conditions) {
    if (!condition.has_required_variable_tile()) {
      continue;
    }
    // We can combine normal tile restriction with variable tile restriction.
    // So, we need to check normal tile restriction prior to variable tile check.
    if (condition.has_required_tile()) {
      if ((tile & condition.required_tile()) == condition.required_tile()) {
        return true;
      }
    }
    // hmm...
  }

  return false;
}

} /* namespace msc */
} /* namespace ydec */
