#ifndef YDEC_MSC_MAHJONGCOMMONUTILS_H_
#define YDEC_MSC_MAHJONGCOMMONUTILS_H_

#include "src-gen/cpp/mahjong-scorecalculator.pb.h"


namespace ydec {
namespace msc {

class MahjongCommonUtils {
 public:
  static bool isSequentialTileType(mahjong::TileType tile);
  static bool isTileTypeMatched(mahjong::TileType required, mahjong::TileType tile);
  static bool isTileTypeMatched(mahjong::TileType required, mahjong::TileType tile,
                                mahjong::TileType mask);
};

}
}

#endif /* YDEC_MSC_MAHJONGCOMMONUTILS_H_ */
