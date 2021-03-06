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

#include "src/score_calculator.h"

#include <algorithm>
#include <iostream>
#include <utility>

#include "src/hand_parser.h"
#include "src/mahjong_common_util.h"
#include "src/yaku_applier.h"

using std::unique_ptr;

namespace ycraft {
namespace mahjong {

ScoreCalculator::ScoreCalculator(unique_ptr<Rule> rule)
    : rule_(move(rule)),
      hand_parser_(new HandParser),
      yaku_applier_(new YakuApplier(*rule_)) {}

void ScoreCalculator::Calculate(const Field& field, const Player& player,
                                ScoreCalculatorResult* result) {
  HandParserResult hand_parser_result;
  hand_parser_->Parse(player.hand(), &hand_parser_result);

  for (const ParsedHand& parsed_hand : hand_parser_result.parsed_hand()) {
    ScoreCalculatorResult current_result;
    Calculate(field, player, parsed_hand, &current_result);
    if (Compare(*result, current_result) > 0) {
      *result = current_result;
    }
  }
}

int ScoreCalculator::Compare(const ScoreCalculatorResult& left,
                             const ScoreCalculatorResult& right) {
  if (left.yakuman() > 0 || right.yakuman() > 0) {
    if (left.yakuman() > right.yakuman()) {
      return -1;
    } else if (left.yakuman() < right.yakuman()) {
      return 1;
    } else if (left.han() != right.han()) {
      return left.han() > right.han() ? -1 : 1;
    }
  } else {
    int kazoe_yakuman_han = rule_->kazoe_yakuman_han();
    if (kazoe_yakuman_han > 0) {
      bool l_kazoe = left.han() >= kazoe_yakuman_han;
      bool r_kazoe = right.han() >= kazoe_yakuman_han;

      if (l_kazoe || r_kazoe) {
        if (!r_kazoe) {
          return -1;
        } else if (!l_kazoe) {
          return 1;
        } else if (left.han() != right.han()) {
          return left.han() > right.han() ? -1 : 1;
        }
      }
    }
  }

  int l_score = left.fu() * (1 << left.han());
  int r_score = right.fu() * (1 << right.han());

  if (l_score > r_score) {
    return -1;
  } else if (l_score < r_score) {
    return 1;
  }

  return 0;
}

void ScoreCalculator::Calculate(const Field& field, const Player& player,
                                const ParsedHand& parsed_hand,
                                ScoreCalculatorResult* result) {
  YakuApplierResult yaku_applier_result;
  yaku_applier_->Apply(player.hand().richi_type(), field.wind(), player.wind(),
                       parsed_hand, &yaku_applier_result);

  if (yaku_applier_result.yaku_size() == 0) {
    return;
  }

  int han = 0;
  int yakuman = 0;
  bool is_menzen = IsMenzen(parsed_hand);
  for (const Yaku& yaku : yaku_applier_result.yaku()) {
    if (is_menzen) {
      han += yaku.menzen_han();
    } else {
      han += yaku.kuisagari_han();
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
  if (IsRichiTypeMatched(RichiType::RICHI, player.hand().richi_type())) {
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
  result->set_han(han + dora + uradora);
  result->set_yakuman(yakuman);
  *result->mutable_yaku() = yaku_applier_result.yaku();

  result->set_fu(FuCalculator(field.wind(), player.wind())
                     .Calculate(parsed_hand, yaku_applier_result));
}

FuCalculator::FuCalculator(TileType field_wind, TileType player_wind)
    : field_wind_(field_wind), player_wind_(player_wind) {}

int FuCalculator::Calculate(
    const ParsedHand& parsed_hand,
    const YakuApplierResult& yaku_applier_result) const {
  for (const Yaku& yaku : yaku_applier_result.yaku()) {
    if (parsed_hand.agari().type() == AgariType::TSUMO &&
        yaku.fu_override_tsumo() > 0) {
      return yaku.fu_override_tsumo();
    } else if (parsed_hand.agari().type() == AgariType::RON &&
               yaku.fu_override_ron() > 0) {
      return yaku.fu_override_ron();
    }
  }

  bool is_menzen = IsMenzen(parsed_hand);

  // futei is 20.
  int fu = 20;

  fu += GetAgariFu(parsed_hand.agari().type(), is_menzen);

  for (const Element& element : parsed_hand.element()) {
    fu += GetElementFu(element);
  }

  fu += GetMachiFu(parsed_hand.machi_type());

  return ((fu + 9) / 10) * 10;
}

int FuCalculator::GetAgariFu(AgariType agari_type, bool is_menzen) const {
  if (agari_type == AgariType::RON && is_menzen) {
    return 10;
  }
  if (agari_type == AgariType::TSUMO) {
    return 2;
  }
  return 0;
}

int FuCalculator::GetElementFu(const Element& element) const {
  switch (element.type()) {
    case HandElementType::ANKOUTSU:
      return IsYaochuhai(element.tile(0).type()) ? 8 : 4;
    case HandElementType::MINKOUTSU:
      return IsYaochuhai(element.tile(0).type()) ? 4 : 2;
    case HandElementType::ANKANTSU:
      return IsYaochuhai(element.tile(0).type()) ? 32 : 16;
    case HandElementType::MINKANTSU:
      return IsYaochuhai(element.tile(0).type()) ? 16 : 8;
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

int FuCalculator::GetMachiFu(MachiType machi_type) const {
  return IsMachiTypeMatched(MachiType::MACHI_2FU, machi_type,
                            MachiType::MASK_MACHI_FU)
             ? 2
             : 0;
}

}  // namespace mahjong
}  // namespace ycraft
