// Copyright 2016 Yuki Hamada
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef SRC_HAND_PARSER_H_
#define SRC_HAND_PARSER_H_

#include <string>

#include "proto/mahjong_scorecalculator.pb.h"

namespace ycraft {
namespace mahjong {

class HandParser {
 public:
  HandParser();
  ~HandParser();

  /**
   * Parses given hand and saves the result into the given result
   * variable. You can use this method repeatedly.
   */
  void Parse(const Hand& hand, HandParserResult* result);

 private:
  void Setup(const Hand& hand, HandParserResult* result);
  void RunDfs();
  void Dfs(int i, int id, bool has_jantou);
  void CheckChiiToitsu();
  void CheckIrregular();
  void AddAgarikeiResult(int last_id, const AgariFormat& format);
  void DeduplicateResult();

  const Hand* hand_;
  int num_free_tiles_;
  int free_tile_group_ids_[14];
  TileType free_tiles_[14];
  HandElementType free_tile_element_types_[14];

  HandParserResult* result_;
};

}  // namespace mahjong
}  // namespace ycraft

#endif  // SRC_HAND_PARSER_H_
