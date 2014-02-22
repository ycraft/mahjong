#include "MahjongCommonUtils.h"

using namespace ydec::mahjong;

namespace ydec {
namespace msc {

  bool isSequentialTileType(TileType tile) {
    return (tile & MASK_TILE_SEQUENTIAL) == SEQUENTIAL_TILE;
  }

}
}
