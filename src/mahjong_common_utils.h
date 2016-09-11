#ifndef YDEC_MSC_MAHJONGCOMMONUTILS_H_
#define YDEC_MSC_MAHJONGCOMMONUTILS_H_

#include "proto/mahjong-scorecalculator.pb.h"


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

  // Utilities for AgariType.
  static bool isAgariTypeMatched(AgariType required, AgariType type);

  // Utilities for AgariState.
  static bool isAgariStateMatched(AgariState required, AgariState state);

  // Utilities for AgariFormat.
  static bool isAgariFormatMatched(AgariFormat required, AgariFormat actual);

  // Utilities for RichiType.
  static bool isRichiTypeMatched(RichiType required, RichiType actual);

  static bool isYaochuhai(TileType tile);

  static bool isMenzen(const ParsedHand& parsed_hand);
};

}
}

#endif /* YDEC_MSC_MAHJONGCOMMONUTILS_H_ */
