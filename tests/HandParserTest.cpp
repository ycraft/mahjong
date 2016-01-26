#include <gtest/gtest.h>
#include <stdexcept>
#include <iostream>

#include <google/protobuf/text_format.h>

#include "CommonTestUtil.h"
#include "src/HandParser.h"

using namespace std;
using namespace ydec::mahjong;
using namespace google::protobuf;


bool checkTile(const Tile& lhs, const Tile& rhs) {
  if (lhs.type() != rhs.type()) return false;
  if (lhs.state_size() != rhs.state_size()) return false;

  vector<int> l_state(lhs.state().begin(), lhs.state().end());
  vector<int> r_state(rhs.state().begin(), rhs.state().end());
  sort(l_state.begin(), l_state.end());
  sort(r_state.begin(), r_state.end());

  return l_state == r_state;
}

bool checkElement(const Element& lhs, const Element& rhs) {
  if (lhs.type() != rhs.type()) return false;
  if (lhs.tile_size() != rhs.tile_size()) return false;

  vector<bool> used(lhs.tile_size(), false);
  for (const Tile& tile : lhs.tile()) {
    bool found = false;
    for (int i = 0; i < rhs.tile_size(); ++i) {
      if (used[i]) {
        continue;
      }
      if (!checkTile(tile, rhs.tile(i))) {
        continue;
      }
      found = used[i] = true;
      break;
    }
    if (!found) {
      return false;
    }
  }

  return true;
}

void verifyAgari(
    const AgariType& expected_agari_type,
    const AgariFormat& expected_agari_format,
    const vector<const AgariState>& expected_agari_state,
    const Agari& actual) {
  ASSERT_EQ(expected_agari_type, actual.type());
  ASSERT_EQ(expected_agari_format, actual.format());

  vector<AgariState> sorted_expected_agari_state(expected_agari_state.begin(),
                                                 expected_agari_state.end());
  vector<AgariState> sorted_actual_state(actual.state().size());
  for (int i = 0; i < sorted_actual_state.size(); ++i) {
    sorted_actual_state[i] = actual.state(i);
  }
  sort(sorted_expected_agari_state.begin(), sorted_expected_agari_state.end());
  sort(sorted_actual_state.begin(), sorted_actual_state.end());
  ASSERT_EQ(sorted_expected_agari_state, sorted_actual_state);
}

void verifyParsedHand(
    const vector<const Element>& expected_elements,
    const MachiType& expected_machi_type,
    const AgariType& expected_agari_type,
    const AgariFormat& expected_agari_format,
    const vector<const AgariState>& expected_agari_state,
    const ParsedHand& actual_parsed_hand) {
  ASSERT_EQ(expected_machi_type, actual_parsed_hand.machi_type());
  ASSERT_NO_FATAL_FAILURE(
      verifyAgari(expected_agari_type,
                  expected_agari_format,
                  expected_agari_state,
                  actual_parsed_hand.agari()));

  ASSERT_EQ(expected_elements.size(), actual_parsed_hand.element_size());
  vector<bool> used(actual_parsed_hand.element_size(), false);
  for (const Element& expected_element : expected_elements) {
    bool found = false;
    for (int i = 0; i < actual_parsed_hand.element_size(); ++i) {
      if (used[i]) {
        continue;
      }
      if (!checkElement(expected_element,
                        actual_parsed_hand.element(i))) {
        continue;
      }
      found = used[i] = true;
      break;
    }
    if (!found) {
      string expected_element_str;
      TextFormat::PrintToString(expected_element,
                                &expected_element_str);
      FAIL() << "Expected element not found: " << endl << expected_element_str;
    }
  }
}

void verifyParsedHandForIrregularAgariFormat(
    const Hand& input_hand,
    const vector<const AgariState>& expected_agari_state,
    const ParsedHand& actual_parsed_hand) {
  Element element;
  element.set_type(HandElementType::UNKNOWN_HAND_ELEMENT_TYPE);
  for (int i = 0; i < input_hand.closed_tile_size(); ++i) {
    Tile* tile = element.add_tile();
    tile->set_type(input_hand.closed_tile(i));
  }
  {
    Tile* tile = element.add_tile();
    tile->set_type(input_hand.agari_tile());
    ASSERT_TRUE(input_hand.agari().type() == AgariType::RON ||
                input_hand.agari().type() == AgariType::TSUMO);
    tile->add_state(
        (input_hand.agari().type() == AgariType::RON) ?
            TileState::AGARI_HAI_RON :
            TileState::AGARI_HAI_TSUMO);
  }
  ASSERT_NO_FATAL_FAILURE(verifyParsedHand(
      {element},
      MachiType::UNKNOWN_MACHI_TYPE,
      input_hand.agari().type(),
      AgariFormat::IRREGULAR_AGARI,
      expected_agari_state,
      actual_parsed_hand));
}


class HandParserTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
  }

  virtual void TearDown() {
  }

  HandParser _handParser;
};

TEST_F(HandParserTest, ParseTest_1) {
  Hand hand;
  hand.add_closed_tile(TileType::PINZU_1);
  hand.add_closed_tile(TileType::PINZU_1);
  hand.add_closed_tile(TileType::PINZU_1);
  hand.add_closed_tile(TileType::PINZU_1);
  hand.add_closed_tile(TileType::PINZU_2);
  hand.add_closed_tile(TileType::PINZU_2);
  hand.add_closed_tile(TileType::PINZU_2);
  hand.add_closed_tile(TileType::PINZU_2);
  hand.add_closed_tile(TileType::PINZU_3);
  hand.add_closed_tile(TileType::PINZU_3);
  hand.add_closed_tile(TileType::PINZU_3);
  hand.add_closed_tile(TileType::PINZU_3);
  hand.add_closed_tile(TileType::PINZU_4);
  hand.set_agari_tile(TileType::PINZU_4);
  hand.mutable_agari()->set_type(AgariType::TSUMO);
  hand.mutable_agari()->add_state(AgariState::MENZEN);

  HandParserResult result;
  _handParser.parse(hand, &result);

  SCOPED_TRACE(HandParserResultUtil::getDebugString(result));

  ASSERT_EQ(4, result.parsed_hand_size());
  ASSERT_NO_FATAL_FAILURE(verifyParsedHand(
      {CommonTestUtil::createAntoitsu(TileType::PINZU_1),
       CommonTestUtil::createAnshuntsu(TileType::PINZU_1),
       CommonTestUtil::createAnshuntsu(TileType::PINZU_1),
       CommonTestUtil::createAnshuntsu(TileType::PINZU_2),
       CommonTestUtil::createAnshuntsu(TileType::PINZU_2, 2)},
      MachiType::RYANMEN,
      AgariType::TSUMO,
      AgariFormat::REGULAR_AGARI,
      {AgariState::MENZEN} /* expected_agari_state */,
      result.parsed_hand(0)));
  ASSERT_NO_FATAL_FAILURE(verifyParsedHand(
      {CommonTestUtil::createAnkoutsu(TileType::PINZU_1),
       CommonTestUtil::createAnkoutsu(TileType::PINZU_2),
       CommonTestUtil::createAnkoutsu(TileType::PINZU_3),
       CommonTestUtil::createAnshuntsu(TileType::PINZU_1),
       CommonTestUtil::createAntoitsu(TileType::PINZU_4, true)},
      MachiType::TANKI,
      AgariType::TSUMO,
      AgariFormat::REGULAR_AGARI,
      {AgariState::MENZEN} /* expected_agari_state */,
      result.parsed_hand(1)));
  ASSERT_NO_FATAL_FAILURE(verifyParsedHand(
      {CommonTestUtil::createAnshuntsu(TileType::PINZU_1),
       CommonTestUtil::createAnshuntsu(TileType::PINZU_1),
       CommonTestUtil::createAnshuntsu(TileType::PINZU_1),
       CommonTestUtil::createAnshuntsu(TileType::PINZU_1),
       CommonTestUtil::createAntoitsu(TileType::PINZU_4, true)},
      MachiType::TANKI,
      AgariType::TSUMO,
      AgariFormat::REGULAR_AGARI,
      {AgariState::MENZEN} /* expected_agari_state */,
      result.parsed_hand(2)));
  ASSERT_NO_FATAL_FAILURE(verifyParsedHandForIrregularAgariFormat(
      hand,
      {AgariState::MENZEN} /* expected_agari_state */,
      result.parsed_hand(3)));
}

TEST_F(HandParserTest, ParseTest_2) {
  Hand hand;
  hand.add_closed_tile(TileType::PINZU_1);
  hand.add_closed_tile(TileType::PINZU_1);
  hand.add_closed_tile(TileType::PINZU_1);
  hand.add_closed_tile(TileType::PINZU_2);
  hand.add_closed_tile(TileType::PINZU_2);
  hand.add_closed_tile(TileType::PINZU_2);
  hand.add_closed_tile(TileType::PINZU_2);
  hand.add_closed_tile(TileType::PINZU_3);
  hand.add_closed_tile(TileType::PINZU_3);
  hand.add_closed_tile(TileType::PINZU_3);
  hand.add_closed_tile(TileType::PINZU_3);
  hand.add_closed_tile(TileType::PINZU_4);
  hand.add_closed_tile(TileType::PINZU_4);
  hand.set_agari_tile(TileType::PINZU_1);
  hand.mutable_agari()->set_type(AgariType::RON);
  hand.mutable_agari()->add_state(AgariState::MENZEN);
  hand.mutable_agari()->add_state(AgariState::SOKU);

  HandParserResult result;
  _handParser.parse(hand, &result);

  SCOPED_TRACE(HandParserResultUtil::getDebugString(result));

  ASSERT_EQ(6, result.parsed_hand_size());
  ASSERT_NO_FATAL_FAILURE(verifyParsedHand(
      {CommonTestUtil::createMintoitsu(TileType::PINZU_1, true),
       CommonTestUtil::createAnshuntsu(TileType::PINZU_1),
       CommonTestUtil::createAnshuntsu(TileType::PINZU_1),
       CommonTestUtil::createAnshuntsu(TileType::PINZU_2),
       CommonTestUtil::createAnshuntsu(TileType::PINZU_2)},
      MachiType::TANKI,
      AgariType::RON,
      AgariFormat::REGULAR_AGARI,
      {AgariState::MENZEN, AgariState::SOKU},
      result.parsed_hand(0)));
  ASSERT_NO_FATAL_FAILURE(verifyParsedHand(
      {CommonTestUtil::createAntoitsu(TileType::PINZU_1),
       CommonTestUtil::createMinshuntsu(TileType::PINZU_1, 0),
       CommonTestUtil::createAnshuntsu(TileType::PINZU_1),
       CommonTestUtil::createAnshuntsu(TileType::PINZU_2),
       CommonTestUtil::createAnshuntsu(TileType::PINZU_2)},
      MachiType::RYANMEN,
      AgariType::RON,
      AgariFormat::REGULAR_AGARI,
      {AgariState::MENZEN, AgariState::SOKU},
      result.parsed_hand(1)));
  ASSERT_NO_FATAL_FAILURE(verifyParsedHand(
      {CommonTestUtil::createMinkoutsu(TileType::PINZU_1, true),
       CommonTestUtil::createAnshuntsu(TileType::PINZU_1),
       CommonTestUtil::createAnkoutsu(TileType::PINZU_2),
       CommonTestUtil::createAnkoutsu(TileType::PINZU_3),
       CommonTestUtil::createAntoitsu(TileType::PINZU_4)},
      MachiType::SHABO,
      AgariType::RON,
      AgariFormat::REGULAR_AGARI,
      {AgariState::MENZEN, AgariState::SOKU},
      result.parsed_hand(2)));
  ASSERT_NO_FATAL_FAILURE(verifyParsedHand(
      {CommonTestUtil::createAnkoutsu(TileType::PINZU_1),
       CommonTestUtil::createMinshuntsu(TileType::PINZU_1, 0),
       CommonTestUtil::createAnkoutsu(TileType::PINZU_2),
       CommonTestUtil::createAnkoutsu(TileType::PINZU_3),
       CommonTestUtil::createAntoitsu(TileType::PINZU_4)},
      MachiType::RYANMEN,
      AgariType::RON,
      AgariFormat::REGULAR_AGARI,
      {AgariState::MENZEN, AgariState::SOKU},
      result.parsed_hand(3)));
  ASSERT_NO_FATAL_FAILURE(verifyParsedHand(
      {CommonTestUtil::createMinshuntsu(TileType::PINZU_1, 0),
       CommonTestUtil::createAnshuntsu(TileType::PINZU_1),
       CommonTestUtil::createAnshuntsu(TileType::PINZU_1),
       CommonTestUtil::createAnshuntsu(TileType::PINZU_1),
       CommonTestUtil::createAntoitsu(TileType::PINZU_4)},
      MachiType::RYANMEN,
      AgariType::RON,
      AgariFormat::REGULAR_AGARI,
      {AgariState::MENZEN, AgariState::SOKU},
      result.parsed_hand(4)));
  ASSERT_NO_FATAL_FAILURE(verifyParsedHandForIrregularAgariFormat(
      hand,
      {AgariState::MENZEN, AgariState::SOKU},
      result.parsed_hand(5)));
}

TEST_F(HandParserTest, ParseTest_3) {
  Hand hand;
  hand.add_closed_tile(TileType::PINZU_1);
  hand.add_closed_tile(TileType::PINZU_2);
  hand.add_closed_tile(TileType::PINZU_3);
  hand.add_closed_tile(TileType::PINZU_5);
  hand.add_closed_tile(TileType::PINZU_6);
  hand.add_closed_tile(TileType::PINZU_7);
  hand.add_closed_tile(TileType::SOUZU_1);
  hand.add_closed_tile(TileType::SOUZU_2);
  hand.add_closed_tile(TileType::SOUZU_3);
  hand.add_closed_tile(TileType::WANZU_1);
  hand.add_closed_tile(TileType::WANZU_2);
  hand.add_closed_tile(TileType::WANZU_3);
  hand.add_closed_tile(TileType::WANZU_9);
  hand.set_agari_tile(TileType::WANZU_9);
  hand.mutable_agari()->set_type(AgariType::TSUMO);

  HandParserResult result;
  _handParser.parse(hand, &result);

  SCOPED_TRACE(HandParserResultUtil::getDebugString(result));

  ASSERT_EQ(2, result.parsed_hand_size());
  ASSERT_NO_FATAL_FAILURE(verifyParsedHand(
      {CommonTestUtil::createAnshuntsu(TileType::PINZU_1),
       CommonTestUtil::createAnshuntsu(TileType::PINZU_5),
       CommonTestUtil::createAnshuntsu(TileType::SOUZU_1),
       CommonTestUtil::createAnshuntsu(TileType::WANZU_1),
       CommonTestUtil::createAntoitsu(TileType::WANZU_9, true)},
      MachiType::TANKI,
      AgariType::TSUMO,
      AgariFormat::REGULAR_AGARI,
      {} /* expected_agari_state */,
      result.parsed_hand(0)));
  ASSERT_NO_FATAL_FAILURE(verifyParsedHandForIrregularAgariFormat(
      hand,
      {} /* expected_agari_state */,
      result.parsed_hand(1)));
}

TEST_F(HandParserTest, ParseTest_4) {
  Hand hand;
  hand.add_closed_tile(TileType::PINZU_1);
  hand.add_closed_tile(TileType::PINZU_2);
  hand.add_closed_tile(TileType::PINZU_3);
  hand.add_closed_tile(TileType::SANGEN_HAKU);
  hand.add_closed_tile(TileType::SANGEN_HATSU);
  hand.add_closed_tile(TileType::SANGEN_CHUN);
  hand.add_closed_tile(TileType::SOUZU_1);
  hand.add_closed_tile(TileType::SOUZU_2);
  hand.add_closed_tile(TileType::SOUZU_3);
  hand.add_closed_tile(TileType::WANZU_1);
  hand.add_closed_tile(TileType::WANZU_2);
  hand.add_closed_tile(TileType::WANZU_3);
  hand.add_closed_tile(TileType::WANZU_9);
  hand.set_agari_tile(TileType::WANZU_9);
  hand.mutable_agari()->set_type(AgariType::TSUMO);

  HandParserResult result;
  _handParser.parse(hand, &result);

  SCOPED_TRACE(HandParserResultUtil::getDebugString(result));

  ASSERT_EQ(1, result.parsed_hand_size());
  ASSERT_NO_FATAL_FAILURE(verifyParsedHandForIrregularAgariFormat(
      hand,
      {} /* expected_agari_state */,
      result.parsed_hand(0)));
}

TEST_F(HandParserTest, ParseTest_5) {
  Hand hand;
  hand.add_closed_tile(TileType::PINZU_1);
  hand.add_closed_tile(TileType::PINZU_2);
  hand.add_closed_tile(TileType::PINZU_3);
  hand.add_closed_tile(TileType::WIND_TON);
  hand.add_closed_tile(TileType::WIND_NAN);
  hand.add_closed_tile(TileType::WIND_SHA);
  hand.add_closed_tile(TileType::SOUZU_1);
  hand.add_closed_tile(TileType::SOUZU_2);
  hand.add_closed_tile(TileType::SOUZU_3);
  hand.add_closed_tile(TileType::WANZU_1);
  hand.add_closed_tile(TileType::WANZU_2);
  hand.add_closed_tile(TileType::WANZU_3);
  hand.add_closed_tile(TileType::WANZU_9);
  hand.set_agari_tile(TileType::WANZU_9);
  hand.mutable_agari()->set_type(AgariType::RON);

  HandParserResult result;
  _handParser.parse(hand, &result);

  SCOPED_TRACE(HandParserResultUtil::getDebugString(result));

  ASSERT_EQ(1, result.parsed_hand_size());
  ASSERT_NO_FATAL_FAILURE(verifyParsedHandForIrregularAgariFormat(
      hand,
      {} /* expected_agari_state */,
      result.parsed_hand(0)));
}

TEST_F(HandParserTest, ParseTest_6) {
  Hand hand;
  hand.add_closed_tile(TileType::PINZU_1);
  hand.add_closed_tile(TileType::PINZU_2);
  hand.add_closed_tile(TileType::PINZU_3);
  hand.add_closed_tile(TileType::PINZU_8);
  hand.add_closed_tile(TileType::PINZU_9);
  hand.add_closed_tile(TileType::PINZU_1);
  hand.add_closed_tile(TileType::SOUZU_1);
  hand.add_closed_tile(TileType::SOUZU_2);
  hand.add_closed_tile(TileType::SOUZU_3);
  hand.add_closed_tile(TileType::WANZU_1);
  hand.add_closed_tile(TileType::WANZU_2);
  hand.add_closed_tile(TileType::WANZU_3);
  hand.add_closed_tile(TileType::WANZU_9);
  hand.set_agari_tile(TileType::WANZU_9);
  hand.mutable_agari()->set_type(AgariType::TSUMO);

  HandParserResult result;
  _handParser.parse(hand, &result);

  SCOPED_TRACE(HandParserResultUtil::getDebugString(result));

  ASSERT_EQ(1, result.parsed_hand_size());
  ASSERT_NO_FATAL_FAILURE(verifyParsedHandForIrregularAgariFormat(
      hand,
      {} /* expected_agari_state */,
      result.parsed_hand(0)));
}

TEST_F(HandParserTest, ParseTest_7) {
  Hand hand;
  hand.add_closed_tile(TileType::PINZU_3);
  hand.add_closed_tile(TileType::PINZU_3);
  hand.add_closed_tile(TileType::PINZU_3);
  hand.add_closed_tile(TileType::PINZU_4);
  Hand::Pon* pon = hand.add_ponned_tile();
  pon->set_tile(TileType::PINZU_2);
  Hand::Kan* kan = hand.add_kanned_tile();
  kan->set_is_closed(true);
  kan->set_tile(TileType::PINZU_1);
  Hand::Chii* chii = hand.add_chiied_tile();
  chii->add_tile(TileType::PINZU_2);
  chii->add_tile(TileType::PINZU_3);
  chii->add_tile(TileType::PINZU_4);
  hand.set_agari_tile(TileType::PINZU_4);
  hand.mutable_agari()->set_type(AgariType::TSUMO);

  HandParserResult result;
  _handParser.parse(hand, &result);

  SCOPED_TRACE(HandParserResultUtil::getDebugString(result));

  ASSERT_EQ(1, result.parsed_hand_size());
  ASSERT_NO_FATAL_FAILURE(verifyParsedHand(
      {CommonTestUtil::createAnkoutsu(TileType::PINZU_3),
       CommonTestUtil::createAntoitsu(TileType::PINZU_4, true),
       CommonTestUtil::createMinkoutsu(TileType::PINZU_2),
       CommonTestUtil::createAnkantsu(TileType::PINZU_1),
       CommonTestUtil::createMinshuntsu(TileType::PINZU_2)},
      MachiType::TANKI,
      AgariType::TSUMO,
      AgariFormat::REGULAR_AGARI,
      {} /* expected_agari_state */,
      result.parsed_hand(0)));
}

TEST_F(HandParserTest, ParseTest_Churen) {
  Hand hand;
  hand.add_closed_tile(TileType::PINZU_1);
  hand.add_closed_tile(TileType::PINZU_1);
  hand.add_closed_tile(TileType::PINZU_1);
  hand.add_closed_tile(TileType::PINZU_2);
  hand.add_closed_tile(TileType::PINZU_3);
  hand.add_closed_tile(TileType::PINZU_4);
  hand.add_closed_tile(TileType::PINZU_5);
  hand.add_closed_tile(TileType::PINZU_6);
  hand.add_closed_tile(TileType::PINZU_7);
  hand.add_closed_tile(TileType::PINZU_8);
  hand.add_closed_tile(TileType::PINZU_9);
  hand.add_closed_tile(TileType::PINZU_9);
  hand.add_closed_tile(TileType::PINZU_9);
  hand.set_agari_tile(TileType::PINZU_9);
  hand.mutable_agari()->set_type(AgariType::RON);

  HandParserResult result;
  _handParser.parse(hand, &result);

  SCOPED_TRACE(HandParserResultUtil::getDebugString(result));

  ASSERT_EQ(3, result.parsed_hand_size());
  ASSERT_NO_FATAL_FAILURE(verifyParsedHand(
      {CommonTestUtil::createAntoitsu(TileType::PINZU_1),
       CommonTestUtil::createAnshuntsu(TileType::PINZU_1),
       CommonTestUtil::createAnshuntsu(TileType::PINZU_4),
       CommonTestUtil::createMinshuntsu(TileType::PINZU_7, 2),
       CommonTestUtil::createAnkoutsu(TileType::PINZU_9)},
      MachiType::RYANMEN,
      AgariType::RON,
      AgariFormat::REGULAR_AGARI,
      {} /* expected_agari_state */,
      result.parsed_hand(0)));
  ASSERT_NO_FATAL_FAILURE(verifyParsedHand(
      {CommonTestUtil::createAntoitsu(TileType::PINZU_1),
       CommonTestUtil::createAnshuntsu(TileType::PINZU_1),
       CommonTestUtil::createAnshuntsu(TileType::PINZU_4),
       CommonTestUtil::createAnshuntsu(TileType::PINZU_7),
       CommonTestUtil::createMinkoutsu(TileType::PINZU_9, true)},
      MachiType::SHABO,
      AgariType::RON,
      AgariFormat::REGULAR_AGARI,
      {} /* expected_agari_state */,
      result.parsed_hand(1)));
  ASSERT_NO_FATAL_FAILURE(verifyParsedHandForIrregularAgariFormat(
      hand,
      {} /* expected_agari_state */,
      result.parsed_hand(2)));
}

TEST_F(HandParserTest, ParseTest_Chitoitsu) {
  Hand hand;
  hand.add_closed_tile(TileType::PINZU_1);
  hand.add_closed_tile(TileType::PINZU_1);
  hand.add_closed_tile(TileType::PINZU_2);
  hand.add_closed_tile(TileType::PINZU_2);
  hand.add_closed_tile(TileType::PINZU_3);
  hand.add_closed_tile(TileType::PINZU_3);
  hand.add_closed_tile(TileType::PINZU_4);
  hand.add_closed_tile(TileType::PINZU_4);
  hand.add_closed_tile(TileType::PINZU_5);
  hand.add_closed_tile(TileType::PINZU_5);
  hand.add_closed_tile(TileType::PINZU_6);
  hand.add_closed_tile(TileType::PINZU_6);
  hand.add_closed_tile(TileType::PINZU_7);
  hand.set_agari_tile(TileType::PINZU_7);
  hand.mutable_agari()->set_type(AgariType::TSUMO);

  HandParserResult result;
  _handParser.parse(hand, &result);

  SCOPED_TRACE(HandParserResultUtil::getDebugString(result));

  ASSERT_EQ(5, result.parsed_hand_size());
  ASSERT_NO_FATAL_FAILURE(verifyParsedHand(
      {CommonTestUtil::createAntoitsu(TileType::PINZU_1),
       CommonTestUtil::createAnshuntsu(TileType::PINZU_2),
       CommonTestUtil::createAnshuntsu(TileType::PINZU_2),
       CommonTestUtil::createAnshuntsu(TileType::PINZU_5),
       CommonTestUtil::createAnshuntsu(TileType::PINZU_5, 2)},
      MachiType::PENCHAN,
      AgariType::TSUMO,
      AgariFormat::REGULAR_AGARI,
      {} /* expected_agari_state */,
      result.parsed_hand(0)));
  ASSERT_NO_FATAL_FAILURE(verifyParsedHand(
      {CommonTestUtil::createAnshuntsu(TileType::PINZU_1),
       CommonTestUtil::createAnshuntsu(TileType::PINZU_1),
       CommonTestUtil::createAntoitsu(TileType::PINZU_4),
       CommonTestUtil::createAnshuntsu(TileType::PINZU_5),
       CommonTestUtil::createAnshuntsu(TileType::PINZU_5, 2)},
      MachiType::PENCHAN,
      AgariType::TSUMO,
      AgariFormat::REGULAR_AGARI,
      {} /* expected_agari_state */,
      result.parsed_hand(1)));
  ASSERT_NO_FATAL_FAILURE(verifyParsedHand(
      {CommonTestUtil::createAnshuntsu(TileType::PINZU_1),
       CommonTestUtil::createAnshuntsu(TileType::PINZU_1),
       CommonTestUtil::createAnshuntsu(TileType::PINZU_4),
       CommonTestUtil::createAnshuntsu(TileType::PINZU_4),
       CommonTestUtil::createAntoitsu(TileType::PINZU_7, true)},
      MachiType::TANKI,
      AgariType::TSUMO,
      AgariFormat::REGULAR_AGARI,
      {} /* expected_agari_state */,
      result.parsed_hand(2)));
  ASSERT_NO_FATAL_FAILURE(verifyParsedHand(
      {CommonTestUtil::createAntoitsu(TileType::PINZU_1),
       CommonTestUtil::createAntoitsu(TileType::PINZU_2),
       CommonTestUtil::createAntoitsu(TileType::PINZU_3),
       CommonTestUtil::createAntoitsu(TileType::PINZU_4),
       CommonTestUtil::createAntoitsu(TileType::PINZU_5),
       CommonTestUtil::createAntoitsu(TileType::PINZU_6),
       CommonTestUtil::createAntoitsu(TileType::PINZU_7, true)},
      MachiType::TANKI,
      AgariType::TSUMO,
      AgariFormat::CHITOITSU_AGARI,
      {} /* expected_agari_state */,
      result.parsed_hand(3)));
  ASSERT_NO_FATAL_FAILURE(verifyParsedHandForIrregularAgariFormat(
      hand,
      {} /* expected_agari_state */,
      result.parsed_hand(4)));
}

TEST_F(HandParserTest, ParseTest_Chitoitsu_2) {
  Hand hand;
  hand.add_closed_tile(TileType::PINZU_1);
  hand.add_closed_tile(TileType::PINZU_1);
  hand.add_closed_tile(TileType::PINZU_2);
  hand.add_closed_tile(TileType::PINZU_2);
  hand.add_closed_tile(TileType::SANGEN_HAKU);
  hand.add_closed_tile(TileType::SANGEN_HAKU);
  hand.add_closed_tile(TileType::SANGEN_CHUN);
  hand.add_closed_tile(TileType::SANGEN_CHUN);
  hand.add_closed_tile(TileType::SANGEN_HATSU);
  hand.add_closed_tile(TileType::SANGEN_HATSU);
  hand.add_closed_tile(TileType::WIND_TON);
  hand.add_closed_tile(TileType::WIND_TON);
  hand.add_closed_tile(TileType::WIND_NAN);
  hand.set_agari_tile(TileType::WIND_NAN);
  hand.mutable_agari()->set_type(AgariType::RON);

  HandParserResult result;
  _handParser.parse(hand, &result);

  SCOPED_TRACE(HandParserResultUtil::getDebugString(result));

  ASSERT_EQ(2, result.parsed_hand_size());
  ASSERT_NO_FATAL_FAILURE(verifyParsedHand(
      {CommonTestUtil::createAntoitsu(TileType::PINZU_1),
       CommonTestUtil::createAntoitsu(TileType::PINZU_2),
       CommonTestUtil::createAntoitsu(TileType::SANGEN_HAKU),
       CommonTestUtil::createAntoitsu(TileType::SANGEN_CHUN),
       CommonTestUtil::createAntoitsu(TileType::SANGEN_HATSU),
       CommonTestUtil::createAntoitsu(TileType::WIND_TON),
       CommonTestUtil::createMintoitsu(TileType::WIND_NAN, true)},
      MachiType::TANKI,
      AgariType::RON,
      AgariFormat::CHITOITSU_AGARI,
      {} /* expected_agari_state */,
      result.parsed_hand(0)));
  ASSERT_NO_FATAL_FAILURE(verifyParsedHandForIrregularAgariFormat(
      hand,
      {} /* expected_agari_state */,
      result.parsed_hand(1)));
}

TEST_F(HandParserTest, ParseTest_NotChitoitsu) {
  Hand hand;
  hand.add_closed_tile(TileType::PINZU_1);
  hand.add_closed_tile(TileType::PINZU_1);
  hand.add_closed_tile(TileType::PINZU_1);
  hand.add_closed_tile(TileType::PINZU_1);
  hand.add_closed_tile(TileType::SANGEN_HAKU);
  hand.add_closed_tile(TileType::SANGEN_HAKU);
  hand.add_closed_tile(TileType::SANGEN_CHUN);
  hand.add_closed_tile(TileType::SANGEN_CHUN);
  hand.add_closed_tile(TileType::SANGEN_HATSU);
  hand.add_closed_tile(TileType::SANGEN_HATSU);
  hand.add_closed_tile(TileType::WIND_TON);
  hand.add_closed_tile(TileType::WIND_TON);
  hand.add_closed_tile(TileType::WIND_NAN);
  hand.set_agari_tile(TileType::WIND_NAN);
  hand.mutable_agari()->set_type(AgariType::RON);

  HandParserResult result;
  _handParser.parse(hand, &result);

  SCOPED_TRACE(HandParserResultUtil::getDebugString(result));

  ASSERT_EQ(1, result.parsed_hand_size());
  ASSERT_NO_FATAL_FAILURE(verifyParsedHandForIrregularAgariFormat(
      hand,
      {} /* expected_agari_state */,
      result.parsed_hand(0)));
}

TEST_F(HandParserTest, ParseTest_Ryanmen) {
  Hand hand;
  hand.add_closed_tile(TileType::PINZU_1);
  hand.add_closed_tile(TileType::PINZU_1);
  hand.add_closed_tile(TileType::SANGEN_HAKU);
  hand.add_closed_tile(TileType::SANGEN_HAKU);
  hand.add_closed_tile(TileType::SANGEN_HAKU);
  hand.add_closed_tile(TileType::SANGEN_HATSU);
  hand.add_closed_tile(TileType::SANGEN_HATSU);
  hand.add_closed_tile(TileType::SANGEN_HATSU);
  hand.add_closed_tile(TileType::SANGEN_CHUN);
  hand.add_closed_tile(TileType::SANGEN_CHUN);
  hand.add_closed_tile(TileType::SANGEN_CHUN);
  hand.add_closed_tile(TileType::SOUZU_2);
  hand.add_closed_tile(TileType::SOUZU_3);
  hand.set_agari_tile(TileType::SOUZU_1);
  hand.mutable_agari()->set_type(AgariType::RON);

  HandParserResult result;
  _handParser.parse(hand, &result);

  SCOPED_TRACE(HandParserResultUtil::getDebugString(result));

  ASSERT_EQ(2, result.parsed_hand_size());
  ASSERT_NO_FATAL_FAILURE(verifyParsedHand(
      {CommonTestUtil::createAntoitsu(TileType::PINZU_1),
       CommonTestUtil::createAnkoutsu(TileType::SANGEN_HAKU),
       CommonTestUtil::createAnkoutsu(TileType::SANGEN_HATSU),
       CommonTestUtil::createAnkoutsu(TileType::SANGEN_CHUN),
       CommonTestUtil::createMinshuntsu(TileType::SOUZU_1, 0)},
      MachiType::RYANMEN,
      AgariType::RON,
      AgariFormat::REGULAR_AGARI,
      {} /* expected_agari_state */,
      result.parsed_hand(0)));
  ASSERT_NO_FATAL_FAILURE(verifyParsedHandForIrregularAgariFormat(
      hand,
      {} /* expected_agari_state */,
      result.parsed_hand(1)));
}

TEST_F(HandParserTest, ParseTest_Penchan_1) {
  Hand hand;
  hand.add_closed_tile(TileType::PINZU_1);
  hand.add_closed_tile(TileType::PINZU_1);
  hand.add_closed_tile(TileType::SANGEN_HAKU);
  hand.add_closed_tile(TileType::SANGEN_HAKU);
  hand.add_closed_tile(TileType::SANGEN_HAKU);
  hand.add_closed_tile(TileType::SANGEN_HATSU);
  hand.add_closed_tile(TileType::SANGEN_HATSU);
  hand.add_closed_tile(TileType::SANGEN_HATSU);
  hand.add_closed_tile(TileType::SANGEN_CHUN);
  hand.add_closed_tile(TileType::SANGEN_CHUN);
  hand.add_closed_tile(TileType::SANGEN_CHUN);
  hand.add_closed_tile(TileType::SOUZU_1);
  hand.add_closed_tile(TileType::SOUZU_2);
  hand.set_agari_tile(TileType::SOUZU_3);
  hand.mutable_agari()->set_type(AgariType::RON);

  HandParserResult result;
  _handParser.parse(hand, &result);

  SCOPED_TRACE(HandParserResultUtil::getDebugString(result));

  ASSERT_EQ(2, result.parsed_hand_size());
  ASSERT_NO_FATAL_FAILURE(verifyParsedHand(
      {CommonTestUtil::createAntoitsu(TileType::PINZU_1),
       CommonTestUtil::createAnkoutsu(TileType::SANGEN_HAKU),
       CommonTestUtil::createAnkoutsu(TileType::SANGEN_HATSU),
       CommonTestUtil::createAnkoutsu(TileType::SANGEN_CHUN),
       CommonTestUtil::createMinshuntsu(TileType::SOUZU_1, 2)},
      MachiType::PENCHAN,
      AgariType::RON,
      AgariFormat::REGULAR_AGARI,
      {} /* expected_agari_state */,
      result.parsed_hand(0)));
  ASSERT_NO_FATAL_FAILURE(verifyParsedHandForIrregularAgariFormat(
      hand,
      {} /* expected_agari_state */,
      result.parsed_hand(1)));
}

TEST_F(HandParserTest, ParseTest_Penchan_2) {
  Hand hand;
  hand.add_closed_tile(TileType::PINZU_1);
  hand.add_closed_tile(TileType::PINZU_1);
  hand.add_closed_tile(TileType::SANGEN_HAKU);
  hand.add_closed_tile(TileType::SANGEN_HAKU);
  hand.add_closed_tile(TileType::SANGEN_HAKU);
  hand.add_closed_tile(TileType::SANGEN_HATSU);
  hand.add_closed_tile(TileType::SANGEN_HATSU);
  hand.add_closed_tile(TileType::SANGEN_HATSU);
  hand.add_closed_tile(TileType::SANGEN_CHUN);
  hand.add_closed_tile(TileType::SANGEN_CHUN);
  hand.add_closed_tile(TileType::SANGEN_CHUN);
  hand.add_closed_tile(TileType::SOUZU_8);
  hand.add_closed_tile(TileType::SOUZU_9);
  hand.set_agari_tile(TileType::SOUZU_7);
  hand.mutable_agari()->set_type(AgariType::RON);

  HandParserResult result;
  _handParser.parse(hand, &result);

  SCOPED_TRACE(HandParserResultUtil::getDebugString(result));

  ASSERT_EQ(2, result.parsed_hand_size());
  ASSERT_NO_FATAL_FAILURE(verifyParsedHand(
      {CommonTestUtil::createAntoitsu(TileType::PINZU_1),
       CommonTestUtil::createAnkoutsu(TileType::SANGEN_HAKU),
       CommonTestUtil::createAnkoutsu(TileType::SANGEN_HATSU),
       CommonTestUtil::createAnkoutsu(TileType::SANGEN_CHUN),
       CommonTestUtil::createMinshuntsu(TileType::SOUZU_7, 0)},
      MachiType::PENCHAN,
      AgariType::RON,
      AgariFormat::REGULAR_AGARI,
      {} /* expected_agari_state */,
      result.parsed_hand(0)));
  ASSERT_NO_FATAL_FAILURE(verifyParsedHandForIrregularAgariFormat(
      hand,
      {} /* expected_agari_state */,
      result.parsed_hand(1)));
}

TEST_F(HandParserTest, ParseTest_Kanchan) {
  Hand hand;
  hand.add_closed_tile(TileType::PINZU_1);
  hand.add_closed_tile(TileType::PINZU_1);
  hand.add_closed_tile(TileType::SANGEN_HAKU);
  hand.add_closed_tile(TileType::SANGEN_HAKU);
  hand.add_closed_tile(TileType::SANGEN_HAKU);
  hand.add_closed_tile(TileType::SANGEN_HATSU);
  hand.add_closed_tile(TileType::SANGEN_HATSU);
  hand.add_closed_tile(TileType::SANGEN_HATSU);
  hand.add_closed_tile(TileType::SANGEN_CHUN);
  hand.add_closed_tile(TileType::SANGEN_CHUN);
  hand.add_closed_tile(TileType::SANGEN_CHUN);
  hand.add_closed_tile(TileType::SOUZU_2);
  hand.add_closed_tile(TileType::SOUZU_4);
  hand.set_agari_tile(TileType::SOUZU_3);
  hand.mutable_agari()->set_type(AgariType::RON);

  HandParserResult result;
  _handParser.parse(hand, &result);

  SCOPED_TRACE(HandParserResultUtil::getDebugString(result));

  ASSERT_EQ(2, result.parsed_hand_size());
  ASSERT_NO_FATAL_FAILURE(verifyParsedHand(
      {CommonTestUtil::createAntoitsu(TileType::PINZU_1),
       CommonTestUtil::createAnkoutsu(TileType::SANGEN_HAKU),
       CommonTestUtil::createAnkoutsu(TileType::SANGEN_HATSU),
       CommonTestUtil::createAnkoutsu(TileType::SANGEN_CHUN),
       CommonTestUtil::createMinshuntsu(TileType::SOUZU_2, 1)},
      MachiType::KANCHAN,
      AgariType::RON,
      AgariFormat::REGULAR_AGARI,
      {} /* expected_agari_state */,
      result.parsed_hand(0)));
  ASSERT_NO_FATAL_FAILURE(verifyParsedHandForIrregularAgariFormat(
      hand,
      {} /* expected_agari_state */,
      result.parsed_hand(1)));
}

TEST_F(HandParserTest, ParseTest_Shabo) {
  Hand hand;
  hand.add_closed_tile(TileType::PINZU_1);
  hand.add_closed_tile(TileType::PINZU_1);
  hand.add_closed_tile(TileType::SANGEN_HAKU);
  hand.add_closed_tile(TileType::SANGEN_HAKU);
  hand.add_closed_tile(TileType::SANGEN_HAKU);
  hand.add_closed_tile(TileType::SANGEN_HATSU);
  hand.add_closed_tile(TileType::SANGEN_HATSU);
  hand.add_closed_tile(TileType::SANGEN_HATSU);
  hand.add_closed_tile(TileType::SANGEN_CHUN);
  hand.add_closed_tile(TileType::SANGEN_CHUN);
  hand.add_closed_tile(TileType::SANGEN_CHUN);
  hand.add_closed_tile(TileType::SOUZU_1);
  hand.add_closed_tile(TileType::SOUZU_1);
  hand.set_agari_tile(TileType::SOUZU_1);
  hand.mutable_agari()->set_type(AgariType::RON);

  HandParserResult result;
  _handParser.parse(hand, &result);

  SCOPED_TRACE(HandParserResultUtil::getDebugString(result));

  ASSERT_EQ(2, result.parsed_hand_size());
  ASSERT_NO_FATAL_FAILURE(verifyParsedHand(
      {CommonTestUtil::createAntoitsu(TileType::PINZU_1),
       CommonTestUtil::createAnkoutsu(TileType::SANGEN_HAKU),
       CommonTestUtil::createAnkoutsu(TileType::SANGEN_HATSU),
       CommonTestUtil::createAnkoutsu(TileType::SANGEN_CHUN),
       CommonTestUtil::createMinkoutsu(TileType::SOUZU_1, true)},
      MachiType::SHABO,
      AgariType::RON,
      AgariFormat::REGULAR_AGARI,
      {} /* expected_agari_state */,
      result.parsed_hand(0)));
  ASSERT_NO_FATAL_FAILURE(verifyParsedHandForIrregularAgariFormat(
      hand,
      {} /* expected_agari_state */,
      result.parsed_hand(1)));
}

TEST_F(HandParserTest, ParseTest_Tanki_1) {
  Hand hand;
  hand.add_closed_tile(TileType::PINZU_1);
  hand.add_closed_tile(TileType::PINZU_1);
  hand.add_closed_tile(TileType::PINZU_1);
  hand.add_closed_tile(TileType::SANGEN_HAKU);
  hand.add_closed_tile(TileType::SANGEN_HAKU);
  hand.add_closed_tile(TileType::SANGEN_HAKU);
  hand.add_closed_tile(TileType::SANGEN_HATSU);
  hand.add_closed_tile(TileType::SANGEN_HATSU);
  hand.add_closed_tile(TileType::SANGEN_HATSU);
  hand.add_closed_tile(TileType::SANGEN_CHUN);
  hand.add_closed_tile(TileType::SANGEN_CHUN);
  hand.add_closed_tile(TileType::SANGEN_CHUN);
  hand.add_closed_tile(TileType::SOUZU_1);
  hand.set_agari_tile(TileType::SOUZU_1);
  hand.mutable_agari()->set_type(AgariType::RON);

  HandParserResult result;
  _handParser.parse(hand, &result);

  SCOPED_TRACE(HandParserResultUtil::getDebugString(result));

  ASSERT_EQ(2, result.parsed_hand_size());
  ASSERT_NO_FATAL_FAILURE(verifyParsedHand(
      {CommonTestUtil::createAnkoutsu(TileType::PINZU_1),
       CommonTestUtil::createAnkoutsu(TileType::SANGEN_HAKU),
       CommonTestUtil::createAnkoutsu(TileType::SANGEN_HATSU),
       CommonTestUtil::createAnkoutsu(TileType::SANGEN_CHUN),
       CommonTestUtil::createMintoitsu(TileType::SOUZU_1, true)},
      MachiType::TANKI,
      AgariType::RON,
      AgariFormat::REGULAR_AGARI,
      {} /* expected_agari_state */,
      result.parsed_hand(0)));
  ASSERT_NO_FATAL_FAILURE(verifyParsedHandForIrregularAgariFormat(
      hand,
      {} /* expected_agari_state */,
      result.parsed_hand(1)));
}

TEST_F(HandParserTest, ParseTest_Tanki_2_Chitoitsu) {
  Hand hand;
  hand.add_closed_tile(TileType::PINZU_1);
  hand.add_closed_tile(TileType::PINZU_1);
  hand.add_closed_tile(TileType::SANGEN_HAKU);
  hand.add_closed_tile(TileType::SANGEN_HAKU);
  hand.add_closed_tile(TileType::SANGEN_HATSU);
  hand.add_closed_tile(TileType::SANGEN_HATSU);
  hand.add_closed_tile(TileType::SANGEN_CHUN);
  hand.add_closed_tile(TileType::SANGEN_CHUN);
  hand.add_closed_tile(TileType::WIND_NAN);
  hand.add_closed_tile(TileType::WIND_NAN);
  hand.add_closed_tile(TileType::WIND_PE);
  hand.add_closed_tile(TileType::WIND_PE);
  hand.add_closed_tile(TileType::SOUZU_1);
  hand.set_agari_tile(TileType::SOUZU_1);
  hand.mutable_agari()->set_type(AgariType::RON);

  HandParserResult result;
  _handParser.parse(hand, &result);

  SCOPED_TRACE(HandParserResultUtil::getDebugString(result));

  ASSERT_EQ(2, result.parsed_hand_size());
  ASSERT_NO_FATAL_FAILURE(verifyParsedHand(
      {CommonTestUtil::createAntoitsu(TileType::PINZU_1),
       CommonTestUtil::createAntoitsu(TileType::SANGEN_HAKU),
       CommonTestUtil::createAntoitsu(TileType::SANGEN_HATSU),
       CommonTestUtil::createAntoitsu(TileType::SANGEN_CHUN),
       CommonTestUtil::createAntoitsu(TileType::WIND_NAN),
       CommonTestUtil::createAntoitsu(TileType::WIND_PE),
       CommonTestUtil::createMintoitsu(TileType::SOUZU_1, true)},
      MachiType::TANKI,
      AgariType::RON,
      AgariFormat::CHITOITSU_AGARI,
      {} /* expected_agari_state */,
      result.parsed_hand(0)));
  ASSERT_NO_FATAL_FAILURE(verifyParsedHandForIrregularAgariFormat(
      hand,
      {} /* expected_agari_state */,
      result.parsed_hand(1)));
}

TEST_F(HandParserTest, ParseTest_Kokushimusou) {
  Hand hand;
  hand.add_closed_tile(TileType::WANZU_1);
  hand.add_closed_tile(TileType::WANZU_9);
  hand.add_closed_tile(TileType::SOUZU_1);
  hand.add_closed_tile(TileType::SOUZU_9);
  hand.add_closed_tile(TileType::PINZU_1);
  hand.add_closed_tile(TileType::PINZU_9);
  hand.add_closed_tile(TileType::WIND_TON);
  hand.add_closed_tile(TileType::WIND_NAN);
  hand.add_closed_tile(TileType::WIND_SHA);
  hand.add_closed_tile(TileType::WIND_PE);
  hand.add_closed_tile(TileType::SANGEN_HAKU);
  hand.add_closed_tile(TileType::SANGEN_HATSU);
  hand.add_closed_tile(TileType::SANGEN_CHUN);
  hand.set_agari_tile(TileType::WANZU_1);
  hand.mutable_agari()->set_type(AgariType::RON);

  HandParserResult result;
  _handParser.parse(hand, &result);

  SCOPED_TRACE(HandParserResultUtil::getDebugString(result));

  ASSERT_EQ(1, result.parsed_hand_size());
  ASSERT_NO_FATAL_FAILURE(verifyParsedHandForIrregularAgariFormat(
      hand,
      {} /* expected_agari_state */,
      result.parsed_hand(0)));
}

TEST_F(HandParserTest, ParseTest_parseTwoHandsWithSingleInstance) {
  // Parse first hand.
  Hand hand;
  hand.add_closed_tile(TileType::PINZU_1);
  hand.add_closed_tile(TileType::PINZU_1);
  hand.add_closed_tile(TileType::SANGEN_HAKU);
  hand.add_closed_tile(TileType::SANGEN_HAKU);
  hand.add_closed_tile(TileType::SANGEN_HATSU);
  hand.add_closed_tile(TileType::SANGEN_HATSU);
  hand.add_closed_tile(TileType::SANGEN_CHUN);
  hand.add_closed_tile(TileType::SANGEN_CHUN);
  hand.add_closed_tile(TileType::WIND_NAN);
  hand.add_closed_tile(TileType::WIND_NAN);
  hand.add_closed_tile(TileType::WIND_PE);
  hand.add_closed_tile(TileType::WIND_PE);
  hand.add_closed_tile(TileType::SOUZU_1);
  hand.set_agari_tile(TileType::SOUZU_1);
  hand.mutable_agari()->set_type(AgariType::RON);

  {
    HandParserResult result;
    _handParser.parse(hand, &result);

    SCOPED_TRACE(HandParserResultUtil::getDebugString(result));

    ASSERT_EQ(2, result.parsed_hand_size());
    ASSERT_NO_FATAL_FAILURE(verifyParsedHand(
        {CommonTestUtil::createAntoitsu(TileType::PINZU_1),
         CommonTestUtil::createAntoitsu(TileType::SANGEN_HAKU),
         CommonTestUtil::createAntoitsu(TileType::SANGEN_HATSU),
         CommonTestUtil::createAntoitsu(TileType::SANGEN_CHUN),
         CommonTestUtil::createAntoitsu(TileType::WIND_NAN),
         CommonTestUtil::createAntoitsu(TileType::WIND_PE),
         CommonTestUtil::createMintoitsu(TileType::SOUZU_1, true)},
        MachiType::TANKI,
        AgariType::RON,
        AgariFormat::CHITOITSU_AGARI,
        {} /* expected_agari_state */,
        result.parsed_hand(0)));
    ASSERT_NO_FATAL_FAILURE(verifyParsedHandForIrregularAgariFormat(
        hand,
        {} /* expected_agari_state */,
        result.parsed_hand(1)));
  }

  {
    // Parse second hand.
    Hand hand2;
    hand2.add_closed_tile(TileType::WANZU_1);
    hand2.add_closed_tile(TileType::WANZU_9);
    hand2.add_closed_tile(TileType::SOUZU_1);
    hand2.add_closed_tile(TileType::SOUZU_9);
    hand2.add_closed_tile(TileType::PINZU_1);
    hand2.add_closed_tile(TileType::PINZU_9);
    hand2.add_closed_tile(TileType::WIND_TON);
    hand2.add_closed_tile(TileType::WIND_NAN);
    hand2.add_closed_tile(TileType::WIND_SHA);
    hand2.add_closed_tile(TileType::WIND_PE);
    hand2.add_closed_tile(TileType::SANGEN_HAKU);
    hand2.add_closed_tile(TileType::SANGEN_HATSU);
    hand2.add_closed_tile(TileType::SANGEN_CHUN);
    hand2.set_agari_tile(TileType::WANZU_1);
    hand2.mutable_agari()->set_type(AgariType::RON);

    HandParserResult result2;
    _handParser.parse(hand2, &result2);

    SCOPED_TRACE(HandParserResultUtil::getDebugString(result2));

    ASSERT_EQ(1, result2.parsed_hand_size());
    ASSERT_NO_FATAL_FAILURE(verifyParsedHandForIrregularAgariFormat(
        hand2,
        {} /* expected_agari_state */,
        result2.parsed_hand(0)));
  }
}
