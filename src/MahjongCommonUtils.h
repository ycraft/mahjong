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

  // Utilities for TileState.
  static bool isTileStateMatched(mahjong::TileState required,
                                 mahjong::TileState actual);

  // Utilities for HandElementType
  static bool isHandElementTypeMatched(mahjong::HandElementType required,
                                       mahjong::HandElementType element_type);

  // Utilities for MachiType.
  static bool isMachiTypeMatched(mahjong::MachiType required, mahjong::MachiType type);
  static bool isMachiTypeMatched(mahjong::MachiType required, mahjong::MachiType type,
                                 mahjong::MachiType mask);

  // Utilities for PlayerType.
  static bool isPlayerTypeMatched(mahjong::PlayerType required, mahjong::PlayerType type);

  // Utilities for AgariType.
  static bool isAgariTypeMatched(mahjong::AgariType required, mahjong::AgariType type);

  // Utilities for AgariState.
  static bool isAgariStateMatched(mahjong::AgariState required, mahjong::AgariState state);

  // Utilities for AgariFormat.
  static bool isAgariFormatMatched(mahjong::AgariFormat required, mahjong::AgariFormat actual);

  // Utilities for RichiType.
  static bool isRichiTypeMatched(mahjong::RichiType required, mahjong::RichiType actual);

  // Return N, where N is N-yakuman equivalent to the given fan.
  // e.g. (fan < 13) -> 0, (13 <= fan < 26) -> 1, ...
  static int toYakuman(int fan);
};

}
}

#endif /* YDEC_MSC_MAHJONGCOMMONUTILS_H_ */
