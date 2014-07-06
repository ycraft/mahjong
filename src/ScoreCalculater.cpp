#include <src/ScoreCalculater.h>

#include <algorithm>

#include "src/HandParser.h"
#include "src/YakuApplier.h"

using namespace std;

namespace ydec {
namespace msc {

ScoreCalculater::ScoreCalculater(unique_ptr<HandParser>&& hand_parser,
                                 unique_ptr<YakuApplier>&& yaku_applier)
    : hand_parser_(move(hand_parser)),
      yaku_applier_(move(yaku_applier)) {
}

ScoreCalculater::~ScoreCalculater() {
}

} /* namespace msc */
} /* namespace ydec */
