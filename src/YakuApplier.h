#ifndef YAKUAPPLIER_H_
#define YAKUAPPLIER_H_

#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "proto/mahjong-common.pb.h"
#include "proto/mahjong-rule.pb.h"
#include "proto/mahjong-scorecalculator.pb.h"

namespace ydec {
namespace mahjong {

/**
 * Yaku Applier.
 */
class YakuApplier {
 public:
  YakuApplier(const Rule& rule);
  virtual ~YakuApplier();

  void apply(const RichiType& richi_type,
             const TileType& field_wind,
             const TileType& player_wind,
             const ParsedHand& parsed_hand,
             YakuApplierResult* result) const;

 private:
  const Rule& rule_;
  std::map<const std::string, const Yaku*> yaku_lookup_table_;
  std::map<const std::string, std::vector<std::string>> upper_yaku_lookup_table_;
};


/**
 * YakuConditionValidator
 */
class YakuConditionValidator {
 public:
  YakuConditionValidator(const YakuCondition& condition,
                         const RichiType& richi_type,
                         const TileType& field_wind,
                         const TileType& player_wind,
                         const ParsedHand& parsed_hand);

  YakuConditionValidatorResult::Type validate();
  YakuConditionValidatorResult::Type validate(YakuConditionValidatorResult* result);

 private:
  const YakuCondition& condition_;
  const RichiType& richi_type_;
  const TileType& field_wind_;
  const TileType& player_wind_;
  const ParsedHand& parsed_hand_;

  YakuConditionValidatorResult* result_;

  ::google::protobuf::RepeatedPtrField<Tile> hand_tiles_;
  std::map<TileCondition::VariableTileType, TileType> variable_tiles_;
  std::map<TileCondition::VariableTileType, std::set<TileType>> defined_tiles_;

  // Agari conditions.
  bool validateRequiredAgariCondition(
      const AgariCondition& condition,
      const Agari& agari);

  // Hand Element Type
  bool validateAllowedHandElementType(
      const ::google::protobuf::RepeatedField<int>& allowed_types,
      const HandElementType& type);

  // Element Conditions
  bool validateRequiredElementCondition(
      const ::google::protobuf::RepeatedPtrField<ElementCondition >& conditions,
      const ::google::protobuf::RepeatedPtrField<Element >& elements,
      bool allow_defining_new_variable);

  bool validateElementCondition(
      const ElementCondition& condition,
      const Element& element,
      bool allow_defining_new_variable);

  // TileConditions
  bool validateAllowedTileCondition(
      const ::google::protobuf::RepeatedPtrField<TileCondition >& conditions,
      const ::google::protobuf::RepeatedPtrField<Tile>& tiles,
      bool allow_defining_new_variable);

  bool validateDisallowedTileCondition(
      const ::google::protobuf::RepeatedPtrField<TileCondition >& conditions,
      const ::google::protobuf::RepeatedPtrField<Tile>& tiles);

  bool validateRequiredTileCondition(
      const ::google::protobuf::RepeatedPtrField<TileCondition >& conditions,
      const ::google::protobuf::RepeatedPtrField<Tile>& tiles,
      bool allow_defining_new_variable);

  bool validateEitherTileCondition(
      const ::google::protobuf::RepeatedPtrField<TileCondition >& conditions,
      const ::google::protobuf::RepeatedPtrField<Tile>& tiles);

  bool validateTileCondition(
      const TileCondition& condition,
      const Tile& tile,
      bool allow_defining_new_variable);

  bool setValiableTile(
      const TileCondition::VariableTileType& type,
      const TileType& tile);

  bool validateVariableTile(
      const TileCondition::VariableTileType& type,
      const TileType& required,
      const TileType& tile);
};

} /* namespace msc */
} /* namespace ydec */

#endif /* YAKUAPPLIER_H_ */
