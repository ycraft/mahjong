/*
 * HandParser.h
 *
 *  Created on: 2014/02/16
 *      Author: yuuki
 */

#ifndef YDEC_MSC_HANDPARSER_H_
#define YDEC_MSC_HANDPARSER_H_

#include "src-gen/cpp/mahjong-scorecalculator.pb.h"

#include <string>

namespace ydec {
namespace msc {

class HandParserResultUtil {
 public:
  static std::string getDebugString(const HandParserResult& result);
};

class HandParser {
 public:
  HandParser();
  ~HandParser();

  void parse(const mahjong::Hand& hand, HandParserResult* result);

 private:
  const mahjong::Hand* _hand;
  unsigned int _num_free_tiles;
  unsigned int _free_tile_group_ids[14];
  mahjong::TileType _free_tiles[14];
  mahjong::HandElementType _free_tile_element_types[14];

  HandParserResult* _result;

  void setup(const mahjong::Hand& hand, HandParserResult* result);
  void runDfs();
  void dfs(int i, int id, bool has_jantou);
  void checkChiiToitsu();
  void checkIrregular();
  void addAgarikeiResult(int last_id);
  void deduplicateResult();
};

}  // msc
}  // ydec

#endif /* YDEC_MSC_HANDPARSER_H_ */
