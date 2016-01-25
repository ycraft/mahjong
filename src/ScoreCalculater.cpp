#include "src/ScoreCalculater.h"

#include <algorithm>

#include "src/HandParser.h"
#include "src/YakuApplier.h"

using namespace std;

namespace ydec {
namespace mahjong {

ScoreCalculater::ScoreCalculater(unique_ptr<HandParser>&& hand_parser,
                                 unique_ptr<YakuApplier>&& yaku_applier)
    : hand_parser_(move(hand_parser)),
      yaku_applier_(move(yaku_applier)) {
}

ScoreCalculater::~ScoreCalculater() {
}

void ScoreCalculater::calculate(const Field& hand,
                                const Player& player,
                                ScoreCalculaterResult* result) {
  HandParserResult hand_parser_result;
  hand_parser_->parse(player.hand(), &hand_parser_result);
  for (const ParsedHand& parsed_hand : hand_parser_result.parsed_hand()) {

  }
}

} /* namespace mahjong */
} /* namespace ydec */
