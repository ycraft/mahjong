#ifndef YDEC_MSC_MAHJONGCOMMONUTILS_H_
#define YDEC_MSC_MAHJONGCOMMONUTILS_H_

#include "src-gen/cpp/mahjong-scorecalculator.pb.h"


namespace ydec {
namespace mahjong {

class MahjongCommonUtils {
 public:
  // Utilities for TileType.
  static bool isSequentialTileType(TileType tile);
  static bool isTileTypeMatched(TileType required, TileType tile);
  static bool isTileTypeMatched(TileType required, TileType tile,
                                TileType mask);

  // Utilities for TileState.
  static bool isTileStateMatched(TileState required,
                                 TileState actual);

  // Utilities for HandElementType
  static bool isHandElementTypeMatched(HandElementType required,
                                       HandElementType element_type);

  // Utilities for MachiType.
  static bool isMachiTypeMatched(MachiType required, MachiType type);
  static bool isMachiTypeMatched(MachiType required, MachiType type,
                                 MachiType mask);

  // Utilities for PlayerType.
  static bool isPlayerTypeMatched(PlayerType required, PlayerType type);

  // Utilities for AgariType.
  static bool isAgariTypeMatched(AgariType required, AgariType type);

  // Utilities for AgariState.
  static bool isAgariStateMatched(AgariState required, AgariState state);

  // Utilities for AgariFormat.
  static bool isAgariFormatMatched(AgariFormat required, AgariFormat actual);

  // Utilities for RichiType.
  static bool isRichiTypeMatched(RichiType required, RichiType actual);

  // Return N, where N is N-yakuman equivalent to the given fan.
  // e.g. (fan < 13) -> 0, (13 <= fan < 26) -> 1, ...
  static int toYakuman(int fan);
};

}
}

#endif /* YDEC_MSC_MAHJONGCOMMONUTILS_H_ */
