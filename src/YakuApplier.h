#ifndef YAKUAPPLIER_H_
#define YAKUAPPLIER_H_

#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

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

  void apply(const mahjong::PlayerType& player_type,
             const mahjong::RichiType& richi_type,
             const ParsedHand& parsed_hand,
             YakuApplierResult* result) const;

 private:
  std::unique_ptr<mahjong::Rule> rule_;
};


/**
 * YakuConditionValidator
 */
class YakuConditionValidator {
 public:
  YakuConditionValidator(const mahjong::YakuCondition& condition,
                         const mahjong::PlayerType& player_type,
                         const mahjong::RichiType& richi_type,
                         const ParsedHand& parsed_hand);

  YakuConditionValidatorResult::Type validate();
  YakuConditionValidatorResult::Type validate(YakuConditionValidatorResult* result);

 private:
  const mahjong::YakuCondition& condition_;
  const mahjong::PlayerType& playerType_;
  const mahjong::RichiType& richi_type_;
  const ParsedHand& parsed_hand_;

  YakuConditionValidatorResult* result_;

  ::google::protobuf::RepeatedPtrField<mahjong::Tile> hand_tiles_;
  std::map<mahjong::TileCondition::VariableTileType, mahjong::TileType> variable_tiles_;
  std::map<mahjong::TileCondition::VariableTileType, std::set<mahjong::TileType>> defined_tiles_;

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
