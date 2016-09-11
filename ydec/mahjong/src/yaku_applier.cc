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

#include "ydec/mahjong/src/yaku_applier.h"

#include <iostream>
#include <memory>
#include <set>
#include <string>
#include <utility>

#include "ydec/mahjong/src/mahjong_common_utils.h"

using std::make_pair;
using std::move;
using std::set;
using std::string;
using std::unique_ptr;
using std::vector;

using google::protobuf::RepeatedField;
using google::protobuf::RepeatedPtrField;

namespace ydec {
namespace mahjong {

/**
 * Implementations for Yaku Applier.
 */
YakuApplier::YakuApplier(const Rule& rule) :
    rule_(rule) {
  for (const Yaku& yaku : rule_.yaku()) {
    if (!yaku_lookup_table_.insert(make_pair(yaku.name(), &yaku)).second) {
      std::cerr << "Duplicated yaku definition found." << std::endl;
      std::abort();
    }
  }

  for (const Yaku& yaku : rule_.yaku()) {
    upper_yaku_lookup_table_.insert(make_pair(yaku.name(), vector<string>()));
    for (const string& upper_yaku_name : yaku.upper_version_yaku_name()) {
      if (yaku_lookup_table_.find(upper_yaku_name) ==
              yaku_lookup_table_.end()) {
        std::abort();
      }
      upper_yaku_lookup_table_[yaku.name()].push_back(upper_yaku_name);
    }

    // Add all yakuman yakus as a upper yaku of this yaku, if this yaku is not a
    // yakuman. For example, if both suanko and tanyao are maiden concurrently,
    // we don't count tanyao because suanko is dominant.
    if (yaku.yakuman() == 0) {
      for (const Yaku& r_yaku : rule_.yaku()) {
        if (r_yaku.yakuman() > 0) {
          upper_yaku_lookup_table_[yaku.name()].push_back(r_yaku.name());
        }
      }
    }
  }
}

YakuApplier::~YakuApplier() {
}

void YakuApplier::Apply(const RichiType& richi_type,
                        const TileType& field_wind,
                        const TileType& player_wind,
                        const ParsedHand& parsed_hand,
                        YakuApplierResult* result) const {
  bool is_menzen = IsMenzen(parsed_hand);

  set<string> applied_yaku_names;
  for (const Yaku& yaku : rule_.yaku()) {
    // Check if hansuu is not zero.
    bool is_applicable =
        yaku.kuisagari_fan() > 0 ||
        yaku.yakuman() > 0 ||
        (is_menzen && yaku.menzen_fan() > 0);
    if (!is_applicable) {
      continue;
    }

    YakuConditionValidator validator(yaku.yaku_condition(),
                                     richi_type,
                                     field_wind,
                                     player_wind,
                                     parsed_hand);
    YakuConditionValidatorResult validator_result;
    validator.Validate(&validator_result);

    if (validator_result.type() == YakuConditionValidatorResult::OK) {
      applied_yaku_names.insert(yaku.name());
    }
  }

  for (const auto& yaku_name : applied_yaku_names) {
    bool upper_yaku_found = false;
    for (const auto& upper_yaku_name : upper_yaku_lookup_table_.at(yaku_name)) {
      upper_yaku_found = (applied_yaku_names.find(upper_yaku_name) !=
          applied_yaku_names.end());
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
                                               const RichiType& richi_type,
                                               const TileType& field_wind,
                                               const TileType& player_wind,
                                               const ParsedHand& parsed_hand)
    : condition_(condition),
      richi_type_(richi_type),
      field_wind_(field_wind),
      player_wind_(player_wind),
      parsed_hand_(parsed_hand),
      result_(nullptr) {
  for (const Element& element : parsed_hand_.element()) {
    for (const Tile& tile : element.tile()) {
      hand_tiles_.Add()->CopyFrom(tile);
    }
  }
}

YakuConditionValidatorResult::Type YakuConditionValidator::Validate() {
  YakuConditionValidatorResult result;
  return Validate(&result);
}

YakuConditionValidatorResult::Type YakuConditionValidator::Validate(
    YakuConditionValidatorResult* result) {
  result_ = result;

  if (!SetValiableTile(TileCondition::VARIABLE_BAKAZE_TILE, field_wind_)) {
    std::cerr << "Failed to set bakaze variable tile." << std::endl;
    result_->set_type(YakuConditionValidatorResult::ERROR_INTERNAL_ERROR);
    return result_->type();
  }

  if (!SetValiableTile(TileCondition::VARIABLE_JIKAZE_TILE, player_wind_)) {
    std::cerr << "Failed to set jikaze variable tile." << std::endl;
    result_->set_type(YakuConditionValidatorResult::ERROR_INTERNAL_ERROR);
    return result_->type();
  }

  // Validate field wind.
  if (condition_.has_required_field_wind()) {
    if (!IsTileTypeMatched(condition_.required_field_wind(), field_wind_)) {
      result_->set_type(YakuConditionValidatorResult::NG_REQUIRED_FIELD_WIND);
      return result_->type();
    }
  }

  // Validate player wind.
  if (condition_.has_required_player_wind()) {
    if (!IsTileTypeMatched(condition_.required_player_wind(), player_wind_)) {
      result_->set_type(YakuConditionValidatorResult::NG_REQUIRED_PLAYER_WIND);
      return result_->type();
    }
  }

  // Validate machi type.
  if (condition_.has_required_machi_type()) {
    if (!IsMachiTypeMatched(condition_.required_machi_type(),
                            parsed_hand_.machi_type())) {
      result_->set_type(YakuConditionValidatorResult::NG_REQUIRED_MACHI_TYPE);
      return result_->type();
    }
  }

  // Validate richi type.
  if (condition_.has_required_richi_type()) {
    if (!IsRichiTypeMatched(condition_.required_richi_type(), richi_type_)) {
      result_->set_type(YakuConditionValidatorResult::NG_REQUIRED_RICHI_TYPE);
      return result_->type();
    }
  }

  // Validate agari condition.
  if (condition_.has_required_agari_condition()) {
    if (!ValidateRequiredAgariCondition(condition_.required_agari_condition(),
                                        parsed_hand_.agari())) {
      result_->set_type(
          YakuConditionValidatorResult::NG_REQUIRED_AGARI_CONDITION);
      return result_->type();
    }
  }

  // Validate allowed tile condition
  if (!ValidateAllowedTileCondition(condition_.allowed_tile_condition(),
                                    hand_tiles_,
                                    true /* allow_defining_new_variable */)) {
    result_->set_type(YakuConditionValidatorResult::NG_ALLOWED_TILE_CONDITION);
    return result_->type();
  }

  // Validate disallowed tile condition
  if (!ValidateDisallowedTileCondition(condition_.disallowed_tile_condition(),
                                       hand_tiles_)) {
    result_->set_type(
        YakuConditionValidatorResult::NG_DISALLOWED_TILE_CONDITION);
    return result_->type();
  }

  // Validate required tile condition
  if (!ValidateRequiredTileCondition(condition_.required_tile_condition(),
                                     hand_tiles_,
                                     true /* allow_defining_new_variable */)) {
    result_->set_type(
        YakuConditionValidatorResult::NG_REQUIRED_TILE_CONDITION);
    return result_->type();
  }

  // Validate element conditions
  if (!ValidateRequiredElementCondition(
      condition_.required_element_condition(),
      parsed_hand_.element(),
      true /* allow_defining_new_variable */)) {
    result_->set_type(
        YakuConditionValidatorResult::NG_REQUIRED_ELEMENT_CONDITION);
    return result_->type();
  }

  result_->set_type(YakuConditionValidatorResult::OK);
  return result_->type();
}

bool YakuConditionValidator::ValidateRequiredAgariCondition(
    const AgariCondition& condition,
    const Agari& agari) {
  // Check type.
  if (condition.has_required_type()) {
    if (!IsAgariTypeMatched(condition.required_type(), agari.type())) {
      return false;
    }
  }

  // Check format.
  if (condition.allowed_format_size() != 0) {
    bool found = false;
    for (const int allowed_format_int : condition.allowed_format()) {
      AgariFormat allowed_format = static_cast<AgariFormat>(allowed_format_int);
      if (IsAgariFormatMatched(allowed_format, agari.format())) {
        found = true;
        break;
      }
    }
    if (!found) {
      return false;
    }
  }

  // Check state.
  if (condition.required_state_size() != 0) {
    unique_ptr<bool[]> used(new bool[agari.state_size()]);
    memset(used.get(), 0, sizeof(used[0]) * agari.state_size());

    for (const int required_state_int : condition.required_state()) {
      const AgariState required_state =
          static_cast<AgariState>(required_state_int);
      bool found = false;
      for (int i = 0; i < agari.state_size(); ++i) {
        if (used[i]) {
          continue;
        }

        if (!IsAgariStateMatched(required_state, agari.state(i))) {
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

  return true;
}

bool YakuConditionValidator::ValidateAllowedHandElementType(
    const RepeatedField<int>& allowed_types,
    const HandElementType& type) {
  // If the number of the given conditions is zero, this method construes as
  // there's no restrictions. So it will always return true.
  if (allowed_types.size() == 0) {
    return true;
  }

  for (auto allowed_type : allowed_types) {
    if (IsHandElementTypeMatched(static_cast<HandElementType>(allowed_type),
                                 type)) {
      return true;
    }
  }

  return false;
}

bool YakuConditionValidator::ValidateRequiredElementCondition(
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
  // If there are no applicable condition, we will allow to define a new
  // variable.
  for (const ElementCondition& condition : conditions) {
    bool found = false;
    for (int new_variable = 0;
        new_variable <= (allow_defining_new_variable ? 1 : 0);
        ++new_variable) {
      for (int i = 0; i < elements.size(); ++i) {
        if (used[i]) {
          continue;
        }
        if (!ValidateElementCondition(condition,
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

bool YakuConditionValidator::ValidateElementCondition(
    const ElementCondition& condition,
    const Element& element,
    bool allow_defining_new_variable) {
  // Check Hand Element Type
  if (!ValidateAllowedHandElementType(condition.allowed_element_type(),
                                      element.type())) {
    return false;
  }

  // Validate allowed_tile_condition.
  if (!ValidateAllowedTileCondition(condition.allowed_tile_condition(),
                                    element.tile(),
                                    allow_defining_new_variable)) {
    return false;
  }

  // Validate required_tile_condition.
  if (!ValidateRequiredTileCondition(condition.required_tile_condition(),
                                     element.tile(),
                                     allow_defining_new_variable)) {
    return false;
  }

  // Validate either_tile_condition.
  if (!ValidateEitherTileCondition(condition.either_tile_condition(),
                                   element.tile())) {
    return false;
  }

  return true;
}

bool YakuConditionValidator::ValidateAllowedTileCondition(
    const RepeatedPtrField<TileCondition >& conditions,
    const RepeatedPtrField<Tile >& tiles,
    bool allow_defining_new_variable) {
  // If the number of the given conditions is zero, this method construes as
  // there's no restrictions. So it will always return true.
  if (conditions.size() == 0) {
    return true;
  }

  // Search applicable condition without defining a new variable first.
  // If there are no applicable condition, we will allow to define a new
  // variable.
  for (const Tile& tile : tiles) {
    bool found = false;
    for (int new_variable = 0;
        new_variable <= (allow_defining_new_variable ? 1 : 0);
        ++new_variable) {
      for (const TileCondition& condition : conditions) {
        if (ValidateTileCondition(condition, tile, new_variable)) {
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

bool YakuConditionValidator::ValidateDisallowedTileCondition(
    const RepeatedPtrField<TileCondition >& conditions,
    const RepeatedPtrField<Tile >& tiles) {
  // If the number of the given conditions is zero, this method construes as
  // there's no restrictions. So it will always return true.
  if (conditions.size() == 0) {
    return true;
  }

  for (const Tile& tile : tiles) {
    for (const TileCondition& condition : conditions) {
      if (ValidateTileCondition(condition, tile,
                                /*allow_defining_new_variable=*/false)) {
        return false;
      }
    }
  }

  return true;
}

bool YakuConditionValidator::ValidateRequiredTileCondition(
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
  // If there are no applicable condition, we will allow to define a new
  // variable.
  for (const TileCondition& condition : conditions) {
    bool found = false;
    for (int new_variable = 0;
      new_variable <= (allow_defining_new_variable ? 1 : 0);
      ++new_variable) {
      for (int i = 0; i < tiles.size(); ++i) {
        if (used[i]) {
          continue;
        }
        if (!ValidateTileCondition(condition, tiles.Get(i),
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

bool YakuConditionValidator::ValidateEitherTileCondition(
    const RepeatedPtrField<TileCondition >& conditions,
    const RepeatedPtrField<Tile >& tiles) {
  // If the number of the given conditions is zero, this method construes as
  // there's no restrictions. So it will always return true.
  if (conditions.size() == 0) {
    return true;
  }

  for (const TileCondition& condition : conditions) {
    for (const Tile& tile : tiles) {
      if (ValidateTileCondition(condition, tile,
                                false /* allow_defining_new_variable */)) {
        return true;
      }
    }
  }

  return false;
}

bool YakuConditionValidator::ValidateTileCondition(
    const TileCondition& condition,
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

        if (!IsTileStateMatched(required_state, tile.state(i))) {
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
      if (IsTileStateMatched(disallowed_state, state)) {
        return false;
      }
    }
  }

  // Check tile type.
  if (condition.has_required_tile()) {
    if (!IsTileTypeMatched(condition.required_tile(), tile.type())) {
      return false;
    }
  }

  // Check variable tile type.
  // We should do this check at the last step of this method because we might
  // define a new variable in this step if allow_defining_new_variable is true.
  if (condition.has_required_variable_tile()) {
    const auto& iter = variable_tiles_.find(condition.required_variable_tile());

    // If the variable tile hasn't defined yet,
    //   - define new variable if allow_defining_new_variable is true.
    //   - return false if allow_defining_new_variable is false.
    if (iter == variable_tiles_.end()) {
      if (allow_defining_new_variable) {
        return SetValiableTile(condition.required_variable_tile(), tile.type());
      } else {
        return false;
      }
    }

    if (!ValidateVariableTile(condition.required_variable_tile(),
                              iter->second, tile.type())) {
      return false;
    }
  }

  return true;
}

bool YakuConditionValidator::SetValiableTile(
    const TileCondition::VariableTileType& type,
    const TileType& tile) {
  // If the given variable_tile_type is already defined with other tile, this
  // method just return false.
  {
    const auto& iter = variable_tiles_.find(type);
    if (iter != variable_tiles_.end()) {
      return ValidateVariableTile(type, iter->second, tile);
    }
  }

  TileCondition::VariableTileType variable_type =
      static_cast<TileCondition::VariableTileType>(
          type & TileCondition::MASK_VARIABLE_TYPE);

  set<TileType>& defined_tiles = defined_tiles_[variable_type];

  // Check whether the given tile is already defined as other variable_tile in
  // the same variable_type.
  if (defined_tiles.find(tile) != defined_tiles.end()) {
    return false;
  }

  switch (variable_type) {
    case TileCondition::VARIABLE_TILE:
    case TileCondition::VARIABLE_TILE2:
    case TileCondition::VARIABLE_CONDITIONAL_YAKUHAI:
    case TileCondition::VARIABLE_CONDITIONAL_YAKUHAI_2:
      defined_tiles.insert(variable_tiles_[type] = tile);
      return true;

    case TileCondition::VARIABLE_NUMBER:
      if (!IsSequentialTileType(tile)) {
        return false;
      } else {
        defined_tiles.insert(variable_tiles_[type] = tile);
        return true;
      }

    case TileCondition::VARIABLE_COLOR:
      switch (type) {
        case TileCondition::VARIABLE_COLOR_A:
          if (!IsSequentialTileType(tile)) {
            return false;
          } else {
            defined_tiles.insert(variable_tiles_[type] = tile);
            return true;
          }

        case TileCondition::VARIABLE_COLOR_A_OR_JIHAI:
          if (!IsSequentialTileType(tile)) {
            // We don't need to store jihai tile to our variable_tile map since
            // it doesn't have any color.
            return true;
          } else {
            defined_tiles.insert(variable_tiles_[type] = tile);
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

bool YakuConditionValidator::ValidateVariableTile(
    const TileCondition::VariableTileType& type,
    const TileType& required,
    const TileType& tile) {
  switch (type & TileCondition::MASK_VARIABLE_TYPE) {
    case TileCondition::VARIABLE_TILE:
    case TileCondition::VARIABLE_TILE2:
    case TileCondition::VARIABLE_CONDITIONAL_YAKUHAI:
    case TileCondition::VARIABLE_CONDITIONAL_YAKUHAI_2:
      return IsTileTypeMatched(required, tile);

    case TileCondition::VARIABLE_NUMBER:
      return
          IsSequentialTileType(tile) &&
          IsTileTypeMatched(required, tile, TileType::MASK_TILE_NUMBER);

    case TileCondition::VARIABLE_COLOR:
      switch (type) {
        case TileCondition::VARIABLE_COLOR_A:
          return
              IsSequentialTileType(tile) &&
              IsTileTypeMatched(required, tile, TileType::MASK_TILE_KIND);

        case TileCondition::VARIABLE_COLOR_A_OR_JIHAI:
          return
              !IsSequentialTileType(tile) ||
              IsTileTypeMatched(required, tile, TileType::MASK_TILE_KIND);

        default:
          return false;
      }
      return false;

    default:
      return false;
  }
}

}  // namespace mahjong
}  // namespace ydec
