#ifndef YDEC_MAHJONG_HAND_PARSER_H_
#define YDEC_MAHJONG_HAND_PARSER_H_

#include <string>

#include "ydec/mahjong/proto/mahjong-scorecalculator.pb.h"

namespace ydec {
namespace mahjong {

class HandParserResultUtil {
 public:
  static std::string getDebugString(const HandParserResult& result);
};

class HandParser {
 public:
  HandParser();
  ~HandParser();

  /**
   * Parses given hand and saves the result into the given result
   * variable. You can call this method any time.
   */
  void parse(const Hand& hand, HandParserResult* result);

 private:
  const Hand* _hand;
  unsigned int _num_free_tiles;
  unsigned int _free_tile_group_ids[14];
  TileType _free_tiles[14];
  HandElementType _free_tile_element_types[14];

  HandParserResult* _result;

  void setup(const Hand& hand, HandParserResult* result);
  void runDfs();
  void dfs(int i, int id, bool has_jantou);
  void checkChiiToitsu();
  void checkIrregular();
  void addAgarikeiResult(int last_id, const AgariFormat& format);
  void deduplicateResult();
};

}  // namespace mahjong
}  // namespace ydec

#endif  // YDEC_MAHJONG_HAND_PARSER_H_
