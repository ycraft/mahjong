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

#ifndef SRC_YAKU_APPLIER_H_
#define SRC_YAKU_APPLIER_H_

#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "proto/mahjong_common.pb.h"
#include "proto/mahjong_rule.pb.h"
#include "proto/mahjong_scorecalculator.pb.h"

namespace ycraft {
namespace mahjong {

class YakuApplier {
 public:
  explicit YakuApplier(const Rule& rule);
  virtual ~YakuApplier();

  void Apply(const RichiType& richi_type, const TileType& field_wind,
             const TileType& player_wind, const ParsedHand& parsed_hand,
             YakuApplierResult* result) const;

 private:
  const Rule& rule_;
  std::map<std::string, const Yaku*> yaku_lookup_table_;
  std::map<std::string, std::vector<std::string>> upper_yaku_lookup_table_;
};

class HandConditionValidator {
 public:
  HandConditionValidator(const HandCondition& condition,
                         const RichiType& richi_type,
                         const TileType& field_wind,
                         const TileType& player_wind,
                         const ParsedHand& parsed_hand);

  HandConditionValidatorResult::Type Validate();
  HandConditionValidatorResult::Type Validate(
      HandConditionValidatorResult* result);

 private:
  // Agari conditions.
  bool ValidateRequiredAgariCondition(const AgariCondition& condition,
                                      const Agari& agari);

  // Hand Element Type
  bool ValidateAllowedHandElementType(
      const google::protobuf::RepeatedField<int>& allowed_types,
      const HandElementType& type);

  // Element Conditions
  bool ValidateRequiredElementCondition(
      const google::protobuf::RepeatedPtrField<ElementCondition>& conditions,
      const google::protobuf::RepeatedPtrField<Element>& elements,
      bool allow_defining_new_variable);

  bool ValidateElementCondition(const ElementCondition& condition,
                                const Element& element,
                                bool allow_defining_new_variable);

  // TileConditions
  bool ValidateAllowedTileCondition(
      const google::protobuf::RepeatedPtrField<TileCondition>& conditions,
      const google::protobuf::RepeatedPtrField<Tile>& tiles,
      bool allow_defining_new_variable);

  bool ValidateDenyTileCondition(
      const google::protobuf::RepeatedPtrField<TileCondition>& conditions,
      const google::protobuf::RepeatedPtrField<Tile>& tiles);

  bool ValidateRequiredTileCondition(
      const google::protobuf::RepeatedPtrField<TileCondition>& conditions,
      const google::protobuf::RepeatedPtrField<Tile>& tiles,
      bool allow_defining_new_variable);

  bool ValidateTileCondition(const TileCondition& condition, const Tile& tile,
                             bool allow_defining_new_variable);

  bool SetValiableTile(const TileCondition::VariableTileType& type,
                       const TileType& tile);

  bool ValidateVariableTile(const TileCondition::VariableTileType& type,
                            const TileType& required, const TileType& tile);

  const HandCondition& condition_;
  const RichiType& richi_type_;
  const TileType& field_wind_;
  const TileType& player_wind_;
  const ParsedHand& parsed_hand_;

  HandConditionValidatorResult* result_;

  google::protobuf::RepeatedPtrField<Tile> hand_tiles_;
  std::map<TileCondition::VariableTileType, TileType> variable_tiles_;
  std::map<TileCondition::VariableTileType, std::set<TileType>> defined_tiles_;
};

}  // namespace mahjong
}  // namespace ycraft

#endif  // SRC_YAKU_APPLIER_H_
