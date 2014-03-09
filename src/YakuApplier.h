#ifndef YAKUAPPLIER_H_
#define YAKUAPPLIER_H_

#include <vector>
#include <map>

#include "src-gen/cpp/mahjong-common.pb.h"
#include "src-gen/cpp/mahjong-rule.pb.h"
#include "src-gen/cpp/mahjong-scorecalculator.pb.h"

namespace ydec {
namespace msc{

class YakuApplier {
 public:
  // Take rule's ownership
  YakuApplier(mahjong::Rule* rule);
  virtual ~YakuApplier();

  void apply(const ParsedHand& parsed_hand, YakuApplierResult* result) const;

 private:
  mahjong::Rule* rule_;
};

class YakuConditionValidator {
 public:
  YakuConditionValidator(const mahjong::YakuCondition& condition,
                         const ParsedHand& parsed_hand);
  bool validate();

 private:
  const mahjong::YakuCondition& condition_;
  const ParsedHand& parsed_hand_;

  std::vector<mahjong::TileType> hand_tiles_;

  std::map<mahjong::TileCondition::VariableTileType, mahjong::TileType> variable_tiles_;

  // Element Conditions
  bool validateRequiredElementCondition(
      const ::google::protobuf::RepeatedPtrField<mahjong::ElementCondition >& conditions,
      const ::google::protobuf::RepeatedPtrField<Element >& elements);

  bool validateRequiredElementCondition(
      const mahjong::ElementCondition& condition,
      const Element& element);

  // TileConditions
  bool validateAllowedTileCondition(
      const ::google::protobuf::RepeatedPtrField<mahjong::TileCondition >& conditions,
      const std::vector<mahjong::TileType>& tiles);

  bool validateDisallowedTileCondition(
      const ::google::protobuf::RepeatedPtrField<mahjong::TileCondition >& conditions,
      const std::vector<mahjong::TileType>& tiles);

  bool validateRequiredTileCondition(
      const ::google::protobuf::RepeatedPtrField<mahjong::TileCondition >& conditions,
      const std::vector<mahjong::TileType>& tiles);

  bool validateEitherTileCondition(
      const ::google::protobuf::RepeatedPtrField<mahjong::TileCondition >& conditions,
      const std::vector<mahjong::TileType>& tiles);

  bool validateTileCondition(
      const mahjong::TileCondition& condition,
      const mahjong::TileType& tile,
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
