#ifndef YDEC_MSC_MAHJONGCOMMONUTILS_H_
#define YDEC_MSC_MAHJONGCOMMONUTILS_H_

#include "src-gen/cpp/mahjong-scorecalculator.pb.h"


namespace ydec {
namespace msc {

class MahjongCommonUtils {
 public:
  // Utilities for TileType.
  static bool isSequentialTileType(mahjong::TileType tile);
  static bool isTileTypeMatched(mahjong::TileType required, mahjong::TileType tile);
  static bool isTileTypeMatched(mahjong::TileType required, mahjong::TileType tile,
                                mahjong::TileType mask);

  // Utilities for HandElementType
  static bool isHandElementTypeMatched(mahjong::HandElementType required,
                                       mahjong::HandElementType element_type);

  // Utilities for MachiType.
  static bool isMachiTypeMatched(mahjong::MachiType required, mahjong::MachiType type);
  static bool isMachiTypeMatched(mahjong::MachiType required, mahjong::MachiType type,
                                 mahjong::MachiType mask);
};

}
}

#endif /* YDEC_MSC_MAHJONGCOMMONUTILS_H_ */
