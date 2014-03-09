#include "YakuApplier.h"

#include <memory>
#include <utility>

#include "MahjongCommonUtils.h"

using google::protobuf::RepeatedPtrField;
using std::make_pair;
using std::vector;
using std::unique_ptr;

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

  for (const Element& element : parsed_hand_.element()) {
    for (const ElementTile element_tile : element.element_tile()) {
      hand_tiles_.push_back(element_tile.tile());
    }
  }
}

bool YakuConditionValidator::validate() {
  // Validate agarikei.
  if (condition_.required_agarikei()) {
    if (!parsed_hand_.is_agarikei()) {
      return false;
    }
  }

  // Validate allowed tile condition
  if (!validateAllowedTileCondition(condition_.allowed_tile_condition(),
                                    hand_tiles_)) {
    return false;
  }

  // Validate disallowed tile condition
  if (!validateDisallowedTileCondition(condition_.disallowed_tile_condition(),
                                       hand_tiles_)) {
    return false;
  }

  // Validate required tile condition
  if (!validateRequiredTileCondition(condition_.required_tile_condition(),
                                     hand_tiles_)) {
    return false;
  }

  // Validate element conditions
  if (!validateRequiredElementCondition(condition_.required_element_condition(),
                                        parsed_hand_.element())) {
    return false;
  }

  return true;
}

bool YakuConditionValidator::validateRequiredElementCondition(
    const RepeatedPtrField<ElementCondition>& conditions,
    const RepeatedPtrField<Element>& elements) {
  unique_ptr<bool[]> used(new bool[elements.size()]);
  memset(used.get(), 0, sizeof(used[0]) * elements.size());
  return false;
}

bool YakuConditionValidator::validateRequiredElementCondition(
    const ElementCondition& condition,
    const Element& element) {
  // Copy TileTypes from element.
  vector<TileType> tiles(element.element_tile_size());
  for (int i = 0; i < tiles.size(); ++i) {
    tiles[i] = element.element_tile(i).tile();
  }

  // Validate allowed_tile_condition.
  if (!validateAllowedTileCondition(condition.allowed_tile_condition(), tiles)) {
    return false;
  }

  // Validate required_tile_condition.
  if (!validateRequiredTileCondition(condition.required_tile_condition(), tiles)) {
    return false;
  }

  // Validate either_tile_condition.
  if (!validateEitherTileCondition(condition.either_tile_condition(), tiles)) {
    return false;
  }

  return false;
}

bool YakuConditionValidator::validateAllowedTileCondition(
    const RepeatedPtrField<TileCondition >& conditions,
    const vector<TileType>& tiles) {
  // If the number of the given conditions is zero, this method construes as
  // there's no restrictions. So it will always return true.
  if (conditions.size() == 0) {
    return true;
  }

  // Search applicable condition without defining a new variable first.
  // If there are no applicable condition, we will allow to define a new variable.
  for (int allow_defining_new_variable = 0;
      allow_defining_new_variable <= 1;
      ++allow_defining_new_variable) {
    for (const TileType& tile : tiles) {
      for (const TileCondition& condition : conditions) {
        if (validateTileCondition(condition, tile, allow_defining_new_variable)) {
          return true;
        }
      }
    }
  }
  return false;
}

bool YakuConditionValidator::validateDisallowedTileCondition(
    const RepeatedPtrField<TileCondition >& conditions,
    const vector<TileType>& tiles) {
  // If the number of the given conditions is zero, this method construes as
  // there's no restrictions. So it will always return true.
  if (conditions.size() == 0) {
    return true;
  }

  for (const TileType& tile : tiles) {
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
    const vector<TileType>& tiles) {
  // If the number of the given conditions is zero, this method construes as
  // there's no restrictions. So it will always return true.
  if (conditions.size() == 0) {
    return true;
  }

  unique_ptr<bool[]> used(new bool[tiles.size()]);
  memset(used.get(), 0, sizeof(used[0]) * tiles.size());
  for (const TileCondition& condition : conditions) {
    bool found = false;
    for (int i = 0; i < tiles.size(); ++i) {
      if (used[i]) {
        continue;
      }
      if (!validateTileCondition(condition, tiles[i],
                                 true /* allow_defining_new_variable */)) {
        continue;
      }

      found = true;
      used[i] = true;
      break;
    }
    if (!found) {
      return false;
    }
  }
  return true;
}

bool YakuConditionValidator::validateEitherTileCondition(
    const RepeatedPtrField<TileCondition >& conditions,
    const vector<TileType>& tiles) {
  // If the number of the given conditions is zero, this method construes as
  // there's no restrictions. So it will always return true.
  if (conditions.size() == 0) {
    return true;
  }

  for (const TileCondition& condition : conditions) {
    for (const TileType& tile : tiles) {
      if (validateTileCondition(condition, tile,
                                false /* allow_defining_new_variable */)) {
        return true;
      }
    }
  }

  return false;
}

bool YakuConditionValidator::validateTileCondition(const TileCondition& condition,
                                                   const TileType& tile,
                                                   bool allow_defining_new_variable) {
  // Check jikaze and bakaze condition.
  // TODO: implement these.

  // Check tile type
  if (condition.has_required_tile()) {
    if (!MahjongCommonUtils::isTileTypeMatched(condition.required_tile(), tile)) {
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
        return setValiableTile(condition.required_variable_tile(), tile);
      } else {
        return false;
      }
    }

    if (!validateVariableTile(condition.required_variable_tile(), iter->second, tile)) {
      return false;
    }
  }

  return true;
}

bool YakuConditionValidator::setValiableTile(
    const TileCondition::VariableTileType& type,
    const TileType& tile) {
  switch (type & TileCondition::MASK_VARIABLE_TYPE) {
    case TileCondition::VARIABLE_TILE:
    case TileCondition::VARIABLE_TILE2:
      return variable_tiles_.insert(make_pair(type, tile)).second;

    case TileCondition::VARIABLE_NUMBER:
      if (!MahjongCommonUtils::isSequentialTileType(tile)) {
        return false;
      } else {
        return variable_tiles_.insert(make_pair(type, tile)).second;
      }

    case TileCondition::VARIABLE_COLOR:
      switch (type) {
        case TileCondition::VARIABLE_COLOR_A:
          if (!MahjongCommonUtils::isSequentialTileType(tile)) {
            return false;
          } else {
            return variable_tiles_.insert(make_pair(type, tile)).second;
          }

        case TileCondition::VARIABLE_COLOR_A_OR_JIHAI:
          if (!MahjongCommonUtils::isSequentialTileType(tile)) {
            // We don't need to store jihai tile to our variable_tile map since
            // it doesn't have any color.
            return true;
          } else {
            return variable_tiles_.insert(make_pair(type, tile)).second;
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
