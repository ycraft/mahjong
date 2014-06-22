#ifndef YAKUAPPLIER_H_
#define YAKUAPPLIER_H_

#include <vector>
#include <map>
#include <memory>
#include <string>

#include "src-gen/cpp/mahjong-common.pb.h"
#include "src-gen/cpp/mahjong-rule.pb.h"
#include "src-gen/cpp/mahjong-scorecalculator.pb.h"

namespace ydec {
namespace msc{

/**
 * Yaku Applier
 */
class YakuApplier {
 public:
  YakuApplier(std::unique_ptr<mahjong::Rule>&& rule);
  virtual ~YakuApplier();

  void apply(const mahjong::PlayerType& playerType,
             const ParsedHand& parsed_hand,
             YakuApplierResult* result) const;

 private:
  std::unique_ptr<mahjong::Rule> rule_;
};


/**
 * YakuConditionValidator
 */
enum YakuConditionValidatorResult {
  YAKU_CONDITION_VALIDATOR_RESULT_OK = 0,
  YAKU_CONDITION_VALIDATOR_RESULT_NG_ALLOWED_TILE_CONDITION = -1,
  YAKU_CONDITION_VALIDATOR_RESULT_NG_DISALLOWED_TILE_CONDITION = -2,
  YAKU_CONDITION_VALIDATOR_RESULT_NG_REQUIRED_TILE_CONDITION = -3,
  YAKU_CONDITION_VALIDATOR_RESULT_NG_REQUIRED_ELEMENT_CONDITION = -4,
  YAKU_CONDITION_VALIDATOR_RESULT_NG_REQUIRED_MACHI_TYPE = -5,
  YAKU_CONDITION_VALIDATOR_RESULT_NG_REQUIRED_PLAYER_TYPE = -6,
  YAKU_CONDITION_VALIDATOR_RESULT_NG_REQUIRED_AGARI_CONDITION = -7,
};

class YakuConditionValidator {
 public:
  YakuConditionValidator(const mahjong::YakuCondition& condition,
                         const mahjong::PlayerType& playerType,
                         const ParsedHand& parsed_hand);

  YakuConditionValidatorResult validate();
  std::string getErrorMessage(YakuConditionValidatorResult result);

 private:
  const mahjong::YakuCondition& condition_;
  const mahjong::PlayerType& playerType_;
  const ParsedHand& parsed_hand_;

  ::google::protobuf::RepeatedPtrField<mahjong::Tile> hand_tiles_;
  std::map<mahjong::TileCondition::VariableTileType, mahjong::TileType> variable_tiles_;

  // Agari conditions.
  bool validateRequiredAgariCondition(
      const mahjong::AgariCondition& condition,
      const mahjong::Agari& agari);

  // Hand Element Type
  bool validateAllowedHandElementType(
      const ::google::protobuf::RepeatedField<int>& allowed_types,
      const mahjong::HandElementType& type);

  // Element Conditions
  bool validateRequiredElementCondition(
      const ::google::protobuf::RepeatedPtrField<mahjong::ElementCondition >& conditions,
      const ::google::protobuf::RepeatedPtrField<mahjong::Element >& elements,
      bool allow_defining_new_variable);

  bool validateElementCondition(
      const mahjong::ElementCondition& condition,
      const mahjong::Element& element,
      bool allow_defining_new_variable);

  // TileConditions
  bool validateAllowedTileCondition(
      const ::google::protobuf::RepeatedPtrField<mahjong::TileCondition >& conditions,
      const ::google::protobuf::RepeatedPtrField<mahjong::Tile>& tiles,
      bool allow_defining_new_variable);

  bool validateDisallowedTileCondition(
      const ::google::protobuf::RepeatedPtrField<mahjong::TileCondition >& conditions,
      const ::google::protobuf::RepeatedPtrField<mahjong::Tile>& tiles);

  bool validateRequiredTileCondition(
      const ::google::protobuf::RepeatedPtrField<mahjong::TileCondition >& conditions,
      const ::google::protobuf::RepeatedPtrField<mahjong::Tile>& tiles,
      bool allow_defining_new_variable);

  bool validateEitherTileCondition(
      const ::google::protobuf::RepeatedPtrField<mahjong::TileCondition >& conditions,
      const ::google::protobuf::RepeatedPtrField<mahjong::Tile>& tiles);

  bool validateTileCondition(
      const mahjong::TileCondition& condition,
      const mahjong::Tile& tile,
      bool allow_defining_new_variable);

  bool setValiableTile(
      const mahjong::TileCondition::VariableTileType& type,
      const mahjong::TileType& tile);

  bool validateVariableTile(
      const mahjong::TileCondition::VariableTileType& type,
      const mahjong::TileType& required,
      const mahjong::TileType& tile);
};

} /* namespace msc */
} /* namespace ydec */

#endif /* YAKUAPPLIER_H_ */
