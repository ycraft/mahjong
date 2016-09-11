#ifndef YDEC_MAHJONG_SCORE_CALCULATOR_H_
#define YDEC_MAHJONG_SCORE_CALCULATOR_H_

#include <memory>

#include "ydec/mahjong/proto/mahjong-scorecalculator.pb.h"

namespace ydec {
namespace mahjong {

class HandParser;
class YakuApplier;

class ScoreCalculator {
 private:
  HandParser& hand_parser_;
  YakuApplier& yaku_applier_;

 public:
  ScoreCalculator(HandParser& hand_parser,
                  YakuApplier& yaku_applier);

  void calculate(const Field& field,
                 const Player& player,
                 ScoreCalculatorResult* result);

 private:
  void calculate(const Field& field,
                 const Player& player,
                 const ParsedHand& parsed_hand,
                 ScoreCalculatorResult* result);
};

class FuCalculator {
 private:
  const TileType field_wind_;
  const TileType player_wind_;

 public:
  FuCalculator(TileType field_wind,
               TileType player_wind);

  int calculate(const ParsedHand& parsed_hand,
                const YakuApplierResult& yaku_applier_result) const;

 private:
  int getAgariFu(AgariType agari_type,
                 bool is_menzen) const;
  int getElementFu(const Element& element) const;
  int getMachiFu(MachiType machi_type) const;
};

}  // namespace mahjong
}  // namespace ydec

#endif  // YDEC_MAHJONG_SCORE_CALCULATOR_H_
