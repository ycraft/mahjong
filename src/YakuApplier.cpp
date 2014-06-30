#include "YakuApplier.h"

#include <iostream>
#include <memory>
#include <set>
#include <string>
#include <utility>

#include <glog/logging.h>

#include "MahjongCommonUtils.h"
#include "ValidatorUtils.h"

using google::protobuf::RepeatedField;
using google::protobuf::RepeatedPtrField;

using std::make_pair;
using std::move;
using std::set;
using std::string;
using std::unique_ptr;
using std::vector;

namespace ydec {

using namespace mahjong;

namespace msc{

/**
 * Implementations for Yaku Applier.
 */
YakuApplier::YakuApplier(std::unique_ptr<Rule>&& rule)
    : rule_(std::move(rule)) {
  for (const Yaku& yaku : rule_->yaku()) {
    CHECK(yaku_lookup_table_.insert(make_pair(yaku.name(), &yaku)).second)
        << "Duplicated yaku definition found.";
  }

  for (const Yaku& yaku : rule_->yaku()) {
    upper_yaku_lookup_table_.insert(make_pair(yaku.name(), vector<const string>()));
    for (const string& upper_yaku_name : yaku.upper_version_yaku_name()) {
      CHECK(yaku_lookup_table_.find(upper_yaku_name) != yaku_lookup_table_.end());
      upper_yaku_lookup_table_[yaku.name()].push_back(upper_yaku_name);
    }
    // Add yakuman as a upper yaku, if the current yaku is not a yakuman.
    if (MahjongCommonUtils::toYakuman(yaku.menzen_fan()) == 0) {
      for (const Yaku& r_yaku : rule_->yaku()) {
        if (MahjongCommonUtils::toYakuman(r_yaku.menzen_fan()) > 0) {
          upper_yaku_lookup_table_[yaku.name()].push_back(r_yaku.name());
        }
      }
    }
  }
}

YakuApplier::~YakuApplier() {
}

void YakuApplier::apply(const PlayerType& player_type,
                        const RichiType& richi_type,
                        const ParsedHand& parsed_hand,
                        YakuApplierResult* result) const {
  DLOG(INFO) << "Apply " << parsed_hand.DebugString();

  set<string> applied_yaku_names;
  for (const Yaku& yaku : rule_->yaku()) {
    YakuConditionValidator validator(yaku.yaku_condition(),
                                     player_type,
                                     richi_type,
                                     parsed_hand);
    YakuConditionValidatorResult validator_result;
    validator.validate(&validator_result);
    DLOG(INFO) << yaku.name() << ": " << YakuConditionValidatorResult::Type_Name(validator_result.type());
    if (validator_result.type() == YakuConditionValidatorResult_Type_OK) {
      applied_yaku_names.insert(yaku.name());
    }
  }

  for (const string& yaku_name : applied_yaku_names) {
    bool upper_yaku_found = false;
    for (const string& upper_yaku_name : upper_yaku_lookup_table_.at(yaku_name)) {
      upper_yaku_found = (applied_yaku_names.find(upper_yaku_name) != applied_yaku_names.end());
      if (upper_yaku_found) {
        break;
      }
    }
    if (!upper_yaku_found) {
      result->add_yaku()->CopyFrom(*yaku_lookup_table_.at(yaku_name));
    }
  }
}


/**
 * Implementations for YakuConditionValidator.
 */
YakuConditionValidator::YakuConditionValidator(const YakuCondition& condition,
                                               const mahjong::PlayerType& player_type,
                                               const mahjong::RichiType& richi_type,
                                               const ParsedHand& parsed_hand)
    : condition_(condition),
      playerType_(player_type),
      richi_type_(richi_type),
      parsed_hand_(parsed_hand),
      result_(nullptr) {
  for (const Element& element : parsed_hand_.element()) {
    for (const Tile& tile : element.tile()) {
      hand_tiles_.Add()->CopyFrom(tile);
    }
  }
}

YakuConditionValidatorResult::Type YakuConditionValidator::validate() {
  YakuConditionValidatorResult result;
  return validate(&result);
}

YakuConditionValidatorResult::Type YakuConditionValidator::validate(YakuConditionValidatorResult* result) {

  DLOG(INFO) << "Start YakuConditionValidator::validate";

  result_ = result;

  // Validate machi type.
  if (condition_.has_required_machi_type()) {
    if (!MahjongCommonUtils::isMachiTypeMatched(condition_.required_machi_type(),
                                                parsed_hand_.machi_type())) {
      result_->set_type(YakuConditionValidatorResult_Type_NG_REQUIRED_MACHI_TYPE);
      return result_->type();
    }
  }

  // Validate player type.
  if (condition_.has_required_player_type()) {
    if (!MahjongCommonUtils::isPlayerTypeMatched(condition_.required_player_type(),
                                                 playerType_)) {
      result_->set_type(YakuConditionValidatorResult_Type_NG_REQUIRED_PLAYER_TYPE);
      return result_->type();
    }
  }

  // Validate richi type.
  if (condition_.has_required_richi_type()) {
    if (!MahjongCommonUtils::isRichiTypeMatched(condition_.required_richi_type(),
                                                richi_type_)) {
      result_->set_type(YakuConditionValidatorResult_Type_NG_REQUIRED_RICHI_TYPE);
      return result_->type();
    }
  }

  // Validate agari condition.
  if (condition_.has_required_agari_condition()) {
    if (!validateRequiredAgariCondition(condition_.required_agari_condition(),
                                        parsed_hand_.agari())) {
      result_->set_type(YakuConditionValidatorResult_Type_NG_REQUIRED_AGARI_CONDITION);
      return result_->type();
    }
  }

  // Validate allowed tile condition
  if (!validateAllowedTileCondition(condition_.allowed_tile_condition(),
                                    hand_tiles_,
                                    true /* allow_defining_new_variable */)) {
    result_->set_type(YakuConditionValidatorResult_Type_NG_ALLOWED_TILE_CONDITION);
    return result_->type();
  }

  // Validate disallowed tile condition
  if (!validateDisallowedTileCondition(condition_.disallowed_tile_condition(),
                                       hand_tiles_)) {
    result_->set_type(YakuConditionValidatorResult_Type_NG_DISALLOWED_TILE_CONDITION);
    return result_->type();
  }

  // Validate required tile condition
  if (!validateRequiredTileCondition(condition_.required_tile_condition(),
                                     hand_tiles_,
                                     true /* allow_defining_new_variable */)) {
    result_->set_type(YakuConditionValidatorResult_Type_NG_REQUIRED_TILE_CONDITION);
    return result_->type();
  }

  // Validate element conditions
  if (!validateRequiredElementCondition(condition_.required_element_condition(),
                                        parsed_hand_.element(),
                                        true /* allow_defining_new_variable */)) {
    result_->set_type(YakuConditionValidatorResult_Type_NG_REQUIRED_ELEMENT_CONDITION);
    return result_->type();
  }

  result_->set_type(YakuConditionValidatorResult_Type_OK);
  return result_->type();
}

bool YakuConditionValidator::validateRequiredAgariCondition(
    const AgariCondition& condition,
    const Agari& agari) {
  // Check type.
  if (condition.has_required_type()) {
    if (!MahjongCommonUtils::isAgariTypeMatched(condition.required_type(),
                                                agari.type())) {
      return false;
    }
  }

  // Check format.
  if (condition.allowed_format_size() != 0) {
    bool found = false;
    for (const int allowed_format_int : condition.allowed_format()) {
      AgariFormat allowed_format = static_cast<AgariFormat>(allowed_format_int);
      if (MahjongCommonUtils::isAgariFormatMatched(allowed_format, agari.format())) {
        found = true;
        break;
      }
    }
    if (!found) {
      return false;
    }
  }

  // Check state.
  if (!ValidatorUtils::validateRequiredItems(
      condition.required_state(),
      agari.state(),
      ValidatorUtils::enumComp<AgariState>(MahjongCommonUtils::isAgariStateMatched))) {
    return false;
  }

  return true;
}

bool YakuConditionValidator::validateAllowedHandElementType(
    const RepeatedField<int>& allowed_types,
    const HandElementType& type) {
  // If the number of the given conditions is zero, this method construes as
  // there's no restrictions. So it will always return true.
  if (allowed_types.size() == 0) {
    return true;
  }

  for (const int& allowed_type : allowed_types) {
    if (MahjongCommonUtils::isHandElementTypeMatched(
        (HandElementType) allowed_type,
        type)) {
      return true;
    }
  }

  return false;
}

bool YakuConditionValidator::validateRequiredElementCondition(
    const RepeatedPtrField<ElementCondition>& conditions,
    const RepeatedPtrField<Element>& elements,
    bool allow_defining_new_variable) {
  // If the number of the given conditions is zero, this method construes as
  // there's no restrictions. So it will always return true.
  if (conditions.size() == 0) {
    return true;
  }

  unique_ptr<bool[]> used(new bool[elements.size()]);
  memset(used.get(), 0, sizeof(used[0]) * elements.size());

  // Search applicable condition without defining a new variable first.
  // If there are no applicable condition, we will allow to define a new variable.
  for (const ElementCondition& condition : conditions) {
    bool found = false;
    for (int new_variable = 0;
        new_variable <= (allow_defining_new_variable ? 1 : 0);
        ++new_variable) {
      for (int i = 0; i < elements.size(); ++i) {
        if (used[i]) {
          continue;
        }
        if (!validateElementCondition(condition,
                                      elements.Get(i),
                                      new_variable)) {
          continue;
        }

        found = true;
        used[i] = true;
        break;
      }
      if (found) {
        break;
      }
    }
    if (!found) {
      return false;
    }
  }

  return true;
}

bool YakuConditionValidator::validateElementCondition(
    const ElementCondition& condition,
    const Element& element,
    bool allow_defining_new_variable) {
  // Check Hand Element Type
  if (!validateAllowedHandElementType(condition.allowed_element_type(),
                                      element.type())) {
    return false;
  }

  // Validate allowed_tile_condition.
  if (!validateAllowedTileCondition(condition.allowed_tile_condition(),
                                    element.tile(),
                                    allow_defining_new_variable)) {
    return false;
  }

  // Validate required_tile_condition.
  if (!validateRequiredTileCondition(condition.required_tile_condition(),
                                     element.tile(),
                                     allow_defining_new_variable)) {
    return false;
  }

  // Validate either_tile_condition.
  if (!validateEitherTileCondition(condition.either_tile_condition(),
                                   element.tile())) {
    return false;
  }

  return true;
}

bool YakuConditionValidator::validateAllowedTileCondition(
    const RepeatedPtrField<TileCondition >& conditions,
    const RepeatedPtrField<Tile >& tiles,
    bool allow_defining_new_variable) {
  // If the number of the given conditions is zero, this method construes as
  // there's no restrictions. So it will always return true.
  if (conditions.size() == 0) {
    return true;
  }

  // Search applicable condition without defining a new variable first.
  // If there are no applicable condition, we will allow to define a new variable.
  for (const Tile& tile : tiles) {
    bool found = false;
    for (int new_variable = 0;
        new_variable <= (allow_defining_new_variable ? 1 : 0);
        ++new_variable) {
      for (const TileCondition& condition : conditions) {
        if (validateTileCondition(condition, tile, new_variable)) {
          found = true;
          break;
        }
      }
      if (found) {
        break;
      }
    }
    if (!found) {
      return false;
    }
  }

  return true;
}

bool YakuConditionValidator::validateDisallowedTileCondition(
    const RepeatedPtrField<TileCondition >& conditions,
    const RepeatedPtrField<Tile >& tiles) {
  // If the number of the given conditions is zero, this method construes as
  // there's no restrictions. So it will always return true.
  if (conditions.size() == 0) {
    return true;
  }

  for (const Tile& tile : tiles) {
    for (const TileCondition& condition : conditions) {
      if (validateTileCondition(condition, tile, false /* allow_defining_new_variable */)) {
        return false;
      }
    }
  }
  return true;
}

bool YakuConditionValidator::validateRequiredTileCondition(
    const RepeatedPtrField<TileCondition >& conditions,
    const RepeatedPtrField<Tile >& tiles,
    bool allow_defining_new_variable) {
  // If the number of the given conditions is zero, this method construes as
  // there's no restrictions. So it will always return true.
  if (conditions.size() == 0) {
    return true;
  }

  unique_ptr<bool[]> used(new bool[tiles.size()]);
  memset(used.get(), 0, sizeof(used[0]) * tiles.size());

  // Search applicable condition without defining a new variable first.
  // If there are no applicable condition, we will allow to define a new variable.
  for (const TileCondition& condition : conditions) {
    bool found = false;
    for (int new_variable = 0;
      new_variable <= (allow_defining_new_variable ? 1 : 0);
      ++new_variable) {
      for (int i = 0; i < tiles.size(); ++i) {
        if (used[i]) {
          continue;
        }
        if (!validateTileCondition(condition, tiles.Get(i),
                                   new_variable)) {
          continue;
        }

        found = true;
        used[i] = true;
        break;
      }
      if (found) {
        break;
      }
    }
    if (!found) {
      return false;
    }
  }
  return true;
}

bool YakuConditionValidator::validateEitherTileCondition(
    const RepeatedPtrField<TileCondition >& conditions,
    const RepeatedPtrField<Tile >& tiles) {
  // If the number of the given conditions is zero, this method construes as
  // there's no restrictions. So it will always return true.
  if (conditions.size() == 0) {
    return true;
  }

  for (const TileCondition& condition : conditions) {
    for (const Tile& tile : tiles) {
      if (validateTileCondition(condition, tile,
                                false /* allow_defining_new_variable */)) {
        return true;
      }
    }
  }

  return false;
}

bool YakuConditionValidator::validateTileCondition(const TileCondition& condition,
                                                   const Tile& tile,
                                                   bool allow_defining_new_variable) {
  // Check required tile state.
  {
    vector<bool> used(tile.state_size(), false);
    for (const int required_state_int : condition.required_state()) {
      TileState required_state = static_cast<TileState>(required_state_int);
      bool found = false;
      for (int i = 0; i < tile.state_size(); ++i) {
        if (used[i]) {
          continue;
        }

        if (!MahjongCommonUtils::isTileStateMatched(required_state, tile.state(i))) {
          continue;
        }

        used[i] = true;
        found = true;
        break;
      }
      if (!found) {
        return false;
      }
    }
  }

  // Check disallowed tile state.
  for (const int disallowed_state_int : condition.disallowed_state()) {
    TileState disallowed_state = static_cast<TileState>(disallowed_state_int);
    for (const int state_int : tile.state()) {
      TileState state = static_cast<TileState>(state_int);
      if (MahjongCommonUtils::isTileStateMatched(disallowed_state, state)) {
        return false;
      }
    }
  }

  // Check tile type.
  if (condition.has_required_tile()) {
    if (!MahjongCommonUtils::isTileTypeMatched(condition.required_tile(), tile.type())) {
      return false;
    }
  }

  // Check variable tile type.
  // We should do this check at the last step of this method because we might define
  // a new variable in this step if allow_defining_new_variable is true.
  if (condition.has_required_variable_tile()) {
    const auto& iter = variable_tiles_.find(condition.required_variable_tile());

    // If the variable tile hasn't defined yet,
    //   - define new variable if allow_defining_new_variable is true.
    //   - return false if allow_defining_new_variable is false.
    if (iter == variable_tiles_.end()) {
      if (allow_defining_new_variable) {
        return setValiableTile(condition.required_variable_tile(), tile.type());
      } else {
        return false;
      }
    }

    if (!validateVariableTile(condition.required_variable_tile(), iter->second, tile.type())) {
      return false;
    }
  }

  return true;
}

bool YakuConditionValidator::setValiableTile(
    const TileCondition::VariableTileType& type,
    const TileType& tile) {
  // If the given variable_tile_type is already defined with other tile, this method just return
  // false.
  {
    const auto& iter = variable_tiles_.find(type);
    if (iter != variable_tiles_.end()) {
      return validateVariableTile(type, iter->second, tile);
    }
  }

  TileCondition::VariableTileType variable_type =
      static_cast<TileCondition::VariableTileType>(type & TileCondition_VariableTileType_MASK_VARIABLE_TYPE);

  set<TileType>& defined_tiles = defined_tiles_[variable_type];

  // Check whether the given tile is already defined as other variable_tile in the same variable_type.
  if (defined_tiles.find(tile) != defined_tiles.end()) {
    return false;
  }

  switch (variable_type) {
    case TileCondition_VariableTileType_VARIABLE_TILE:
    case TileCondition_VariableTileType_VARIABLE_TILE2:
      defined_tiles.insert(variable_tiles_[type] = tile);
      DLOG(INFO) << "Register variable tile: " << TileCondition::VariableTileType_Name(type) << " = " << TileType_Name(tile);
      return true;

    case TileCondition_VariableTileType_VARIABLE_NUMBER:
      if (!MahjongCommonUtils::isSequentialTileType(tile)) {
        return false;
      } else {
        defined_tiles.insert(variable_tiles_[type] = tile);
        DLOG(INFO) << "Register variable tile: " << TileCondition::VariableTileType_Name(type) << " = " << TileType_Name(tile);
        return true;
      }

    case TileCondition_VariableTileType_VARIABLE_COLOR:
      switch (type) {
        case TileCondition_VariableTileType_VARIABLE_COLOR_A:
          if (!MahjongCommonUtils::isSequentialTileType(tile)) {
            return false;
          } else {
            defined_tiles.insert(variable_tiles_[type] = tile);
            DLOG(INFO) << "Register variable tile: " << TileCondition::VariableTileType_Name(type) << " = " << TileType_Name(tile);
            return true;
          }

        case TileCondition_VariableTileType_VARIABLE_COLOR_A_OR_JIHAI:
          if (!MahjongCommonUtils::isSequentialTileType(tile)) {
            // We don't need to store jihai tile to our variable_tile map since
            // it doesn't have any color.
            return true;
          } else {
            defined_tiles.insert(variable_tiles_[type] = tile);
            DLOG(INFO) << "Register variable tile: " << TileCondition::VariableTileType_Name(type) << " = " << TileType_Name(tile);
            return true;
          }

        default:
          return false;
      }
      return false;

    default:
      return false;
  }
}

bool YakuConditionValidator::validateVariableTile(
    const TileCondition::VariableTileType& type,
    const TileType& required,
    const TileType& tile) {
  switch (type & TileCondition::MASK_VARIABLE_TYPE) {
    case TileCondition::VARIABLE_TILE:
    case TileCondition::VARIABLE_TILE2:
      return MahjongCommonUtils::isTileTypeMatched(required, tile);

    case TileCondition::VARIABLE_NUMBER:
      return MahjongCommonUtils::isSequentialTileType(tile)
          && MahjongCommonUtils::isTileTypeMatched(required, tile, TileType::MASK_TILE_NUMBER);

    case TileCondition::VARIABLE_COLOR:
      switch (type) {
        case TileCondition::VARIABLE_COLOR_A:
          return MahjongCommonUtils::isSequentialTileType(tile)
              && MahjongCommonUtils::isTileTypeMatched(required, tile, TileType::MASK_TILE_KIND);

        case TileCondition::VARIABLE_COLOR_A_OR_JIHAI:
          return !MahjongCommonUtils::isSequentialTileType(tile)
              || MahjongCommonUtils::isTileTypeMatched(required, tile, TileType::MASK_TILE_KIND);

        default:
          return false;
      }
      return false;

    default:
      return false;
  }
}

} /* namespace msc */
} /* namespace ydec */
