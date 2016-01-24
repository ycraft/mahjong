/*
 * ScoreCalculater.h
 *
 *  Created on: 2014/06/14
 *      Author: yuuki
 */

#ifndef SCORECALCULATER_H_
#define SCORECALCULATER_H_

#include <memory>

#include "src-gen/cpp/mahjong-scorecalculator.pb.h"

namespace ydec {
namespace mahjong {

class HandParser;
class YakuApplier;

class ScoreCalculater {
 private:
  std::unique_ptr<HandParser> hand_parser_;
  std::unique_ptr<YakuApplier> yaku_applier_;

 public:
  ScoreCalculater(std::unique_ptr<HandParser>&& hand_parser,
                  std::unique_ptr<YakuApplier>&& yaku_applier);
  ~ScoreCalculater();

  void calculate(const Hand& hand, ScoreCalculaterResult* result);
};

} /* namespace mahjong */
} /* namespace ydec */

#endif /* SCORECALCULATER_H_ */
