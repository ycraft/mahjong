#include <gtest/gtest.h>
#include <stdexcept>

#include <iostream>

#include "src/HandParser.h"

using namespace ydec::mahjong;
using namespace ydec::msc;

using namespace std;

class HandParserTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
  }

  virtual void TearDown() {
  }

  HandParser _handParser;
};

TEST_F(HandParserTest, ParseTest) {
  Hand hand;
  hand.add_closed_tile(PINZU_1);
  hand.add_closed_tile(PINZU_1);
  hand.add_closed_tile(PINZU_1);
  hand.add_closed_tile(PINZU_1);
  hand.add_closed_tile(PINZU_2);
  hand.add_closed_tile(PINZU_2);
  hand.add_closed_tile(PINZU_2);
  hand.add_closed_tile(PINZU_2);
  hand.add_closed_tile(PINZU_3);
  hand.add_closed_tile(PINZU_3);
  hand.add_closed_tile(PINZU_3);
  hand.add_closed_tile(PINZU_3);
  hand.add_closed_tile(PINZU_4);
  hand.set_agari_tile(PINZU_4);
  hand.set_richi_type(RICHI);
  hand.set_agari_type(RON);

  HandParserResult result;
  _handParser.parse(hand, &result);

  for (int i = 0; i < result.parsed_hand_size(); ++i) {
    const ParsedHand& parsedHand = result.parsed_hand(i);
    for (int j = 0; j < parsedHand.element_size(); ++j) {
      const Element& element = parsedHand.element(j);
      for (int k = 0; k < element.element_tile_size(); ++k) {
        const ElementTile &elementTile = element.element_tile(k);
        cout << TileType_Name(elementTile.tile()) << " ";
      }
      cout << ", ";
    }
    cout << endl;
  }

  EXPECT_EQ(3, result.parsed_hand_size());
}
