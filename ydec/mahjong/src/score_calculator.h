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

#ifndef YDEC_MAHJONG_SRC_SCORE_CALCULATOR_H_
#define YDEC_MAHJONG_SRC_SCORE_CALCULATOR_H_

#include <memory>

#include "ydec/mahjong/proto/mahjong_scorecalculator.pb.h"

namespace ydec {
namespace mahjong {

class HandParser;
class YakuApplier;

class ScoreCalculator {
 public:
  explicit ScoreCalculator(std::unique_ptr<Rule> rule);

  void Calculate(const Field& field, const Player& player,
                 ScoreCalculatorResult* result);

 private:
  void Calculate(const Field& field, const Player& player,
                 const ParsedHand& parsed_hand, ScoreCalculatorResult* result);

  std::unique_ptr<Rule> rule_;
  std::unique_ptr<HandParser> hand_parser_;
  std::unique_ptr<YakuApplier> yaku_applier_;
};

class FuCalculator {
 public:
  FuCalculator(TileType field_wind, TileType player_wind);

  int Calculate(const ParsedHand& parsed_hand,
                const YakuApplierResult& yaku_applier_result) const;

 private:
  int GetAgariFu(AgariType agari_type, bool is_menzen) const;
  int GetElementFu(const Element& element) const;
  int GetMachiFu(MachiType machi_type) const;

  const TileType field_wind_;
  const TileType player_wind_;
};

}  // namespace mahjong
}  // namespace ydec

#endif  // YDEC_MAHJONG_SRC_SCORE_CALCULATOR_H_
