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

#include "ydec/mahjong/src/score_calculator.h"

#include <algorithm>
#include <iostream>
#include <utility>

#include "ydec/mahjong/src/hand_parser.h"
#include "ydec/mahjong/src/yaku_applier.h"
#include "ydec/mahjong/src/mahjong_common_utils.h"
#include "ydec/mahjong/src/mahjong_common_value.h"

using std::unique_ptr;

namespace ydec {
namespace mahjong {

namespace {

  // Returns -1 if left one has better score.
  // Returns 1 if right one has better score.
  // Returns 0 if both ones have the same score.
  int compare(const ScoreCalculatorResult& left,
              const ScoreCalculatorResult& right) {
    if (left.yakuman() > right.yakuman()) {
      return -1;
    } else if (left.yakuman() < right.yakuman()) {
      return 1;
    }

    bool l_kazoe = left.fan() >= MahjongCommonValue::KAZOE_YAKUMAN_FAN;
    bool r_kazoe = right.fan() >= MahjongCommonValue::KAZOE_YAKUMAN_FAN;

    if (l_kazoe && !r_kazoe) {
      return -1;
    } else if (!l_kazoe && r_kazoe) {
      return 1;
    }

    int l_score = left.fu() * (1 << left.fan());
    int r_score = right.fu() * (1 << right.fan());

    if (l_score > r_score) {
      return -1;
    } else if (l_score < r_score) {
      return 1;
    }

    return 0;
  }

}  // namespace

ScoreCalculator::ScoreCalculator(unique_ptr<Rule> rule) :
    rule_(move(rule)),
    hand_parser_(new HandParser),
    yaku_applier_(new YakuApplier(*rule_)) {
}

void ScoreCalculator::calculate(const Field& field,
                                const Player& player,
                                ScoreCalculatorResult* result) {
  HandParserResult hand_parser_result;
  hand_parser_->parse(player.hand(), &hand_parser_result);

  for (const ParsedHand& parsed_hand : hand_parser_result.parsed_hand()) {
    ScoreCalculatorResult current_result;
    calculate(field, player, parsed_hand, &current_result);
    if (compare(*result, current_result) > 0) {
      *result = current_result;
    }
  }
}

void ScoreCalculator::calculate(const Field& field,
                                const Player& player,
                                const ParsedHand& parsed_hand,
                                ScoreCalculatorResult* result) {
  YakuApplierResult yaku_applier_result;
  yaku_applier_->apply(
      player.hand().richi_type(),
      field.wind(),
      player.wind(),
      parsed_hand,
      &yaku_applier_result);

  if (yaku_applier_result.yaku_size() == 0) {
    return;
  }

  int han = 0;
  int yakuman = 0;
  bool is_menzen = MahjongCommonUtils::isMenzen(parsed_hand);
  for (const Yaku& yaku : yaku_applier_result.yaku()) {
    if (is_menzen) {
      han += yaku.menzen_fan();
    } else {
      han += yaku.kuisagari_fan();
    }
    yakuman += yaku.yakuman();
  }

  int dora = 0;
  for (const Element& element : parsed_hand.element()) {
    for (const Tile& tile : element.tile()) {
      for (int dora_tile : field.dora()) {
        if (tile.type() == static_cast<TileType>(dora_tile)) {
          ++dora;
        }
      }
    }
  }

  int uradora = 0;
  if (MahjongCommonUtils::isRichiTypeMatched(
      RichiType::RICHI,
      player.hand().richi_type())) {
    for (const Element& element : parsed_hand.element()) {
      for (const Tile& tile : element.tile()) {
        for (int uradora_tile : field.uradora()) {
          if (tile.type() == static_cast<TileType>(uradora_tile)) {
            ++uradora;
          }
        }
      }
    }
  }

  result->set_dora(dora);
  result->set_uradora(uradora);
  result->set_fan(han + dora + uradora);
  result->set_yakuman(yakuman);
  *result->mutable_yaku() = yaku_applier_result.yaku();

  result->set_fu(FuCalculator(field.wind(), player.wind())
      .calculate(parsed_hand, yaku_applier_result));
}

FuCalculator::FuCalculator(
    TileType field_wind,
    TileType player_wind)
    : field_wind_(field_wind),
      player_wind_(player_wind) {
}

int FuCalculator::calculate(
    const ParsedHand& parsed_hand,
    const YakuApplierResult& yaku_applier_result) const {
  for (const Yaku& yaku : yaku_applier_result.yaku()) {
    if (parsed_hand.agari().type() == AgariType::TSUMO &&
        yaku.has_force_fu_tsumo()) {
      return yaku.force_fu_tsumo();
    } else if (parsed_hand.agari().type() == AgariType::RON &&
               yaku.has_force_fu_ron()) {
      return yaku.force_fu_ron();
    }
  }

  bool is_menzen = MahjongCommonUtils::isMenzen(parsed_hand);

  // futei is 20.
  int fu = 20;

  fu += getAgariFu(parsed_hand.agari().type(), is_menzen);

  for (const Element& element : parsed_hand.element()) {
    fu += getElementFu(element);
  }

  fu += getMachiFu(parsed_hand.machi_type());

  return ((fu + 9) / 10) * 10;
}

int FuCalculator::getAgariFu(AgariType agari_type,
                             bool is_menzen) const {
  if (agari_type == AgariType::RON && is_menzen) {
    return 10;
  }
  if (agari_type == AgariType::TSUMO) {
    return 2;
  }
  return 0;
}

int FuCalculator::getElementFu(const Element& element) const {
  switch (element.type()) {
    case HandElementType::ANKOUTSU:
      return MahjongCommonUtils::isYaochuhai(element.tile(0).type()) ? 8 : 4;
    case HandElementType::MINKOUTSU:
      return MahjongCommonUtils::isYaochuhai(element.tile(0).type()) ? 4 : 2;
    case HandElementType::ANKANTSU:
      return MahjongCommonUtils::isYaochuhai(element.tile(0).type()) ? 32 : 16;
    case HandElementType::MINKANTSU:
      return MahjongCommonUtils::isYaochuhai(element.tile(0).type()) ? 16 : 8;
    case HandElementType::ANTOITSU:
    case HandElementType::MINTOITSU:
      return (element.tile(0).type() == field_wind_ ? 2 : 0) +
             (element.tile(0).type() == player_wind_ ? 2 : 0) +
             (element.tile(0).type() == TileType::SANGEN_HAKU ? 2 : 0) +
             (element.tile(0).type() == TileType::SANGEN_HATSU ? 2 : 0) +
             (element.tile(0).type() == TileType::SANGEN_CHUN ? 2 : 0);
    default:
      return 0;
  }
}

int FuCalculator::getMachiFu(MachiType machi_type) const {
  return MahjongCommonUtils::isMachiTypeMatched(
      MachiType::MACHI_2FU,
      machi_type,
      MachiType::MASK_MACHI_FU) ? 2 : 0;
}

}  // namespace mahjong
}  // namespace ydec
