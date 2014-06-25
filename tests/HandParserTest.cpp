#include <gtest/gtest.h>
#include <stdexcept>

#include <iostream>

#include "CommonTestUtil.h"
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

  bool check(const Tile& lhs, const Tile& rhs) {
    if (lhs.type() != rhs.type()) return false;
    if (lhs.state_size() != rhs.state_size()) return false;
    vector<int> l_state(lhs.state().begin(), lhs.state().end());
    vector<int> r_state(rhs.state().begin(), rhs.state().end());
    sort(l_state.begin(), l_state.end());
    sort(r_state.begin(), r_state.end());
    return l_state == r_state;
  }

  bool check(const Element& lhs, const Element& rhs) {
    if (lhs.type() != rhs.type()) return false;
    if (lhs.tile_size() != rhs.tile_size()) return false;
    vector<bool> used(lhs.tile_size(), false);
    for (const Tile& tile : lhs.tile()) {
      bool found = false;
      for (int i = 0; i < rhs.tile_size(); ++i) {
        if (used[i]) {
          continue;
        }
        if (!check(tile, rhs.tile(i))) {
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

  bool check(vector<const Element> expected_elements, const MachiType& expected_machi_type,
             const AgariFormat& expected_agari_format,
             const ParsedHand& actual_parsed_hand) {
    if (expected_elements.size() != actual_parsed_hand.element_size()) return false;
    if (expected_machi_type != actual_parsed_hand.machi_type()) return false;
    if (actual_parsed_hand.agari().format() != expected_agari_format) return false;
    vector<bool> used(actual_parsed_hand.element_size(), false);
    for (const Element& expected_element : expected_elements) {
      bool found = false;
      for (int i = 0; i < actual_parsed_hand.element_size(); ++i) {
        if (used[i]) {
          continue;
        }
        if (!check(expected_element, actual_parsed_hand.element(i))) {
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

  bool check(vector<const Element> expected_elements, const MachiType& expected_machi_type,
             const AgariFormat& expected_agari_format,
             const HandParserResult& actual) {
    for (const ParsedHand& parsed_hand : actual.parsed_hand()) {
      if (check(expected_elements, expected_machi_type, expected_agari_format, parsed_hand)) {
        return true;
      }
    }
    return false;
  }

  bool checkIrregularAgariFormatIsContained(const HandParserResult& actual) {
    for (const ParsedHand& parsed_hand : actual.parsed_hand()) {
      if (parsed_hand.agari().format() == AgariFormat::IRREGULAR_AGARI
          && parsed_hand.machi_type() == MachiType::UNKNOWN_MACHI_TYPE) {
        return true;
      }
    }
    return false;
  }
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

  HandParserResult result;
  _handParser.parse(hand, &result);

  // cout << HandParserResultUtil::getDebugString(result) << endl;

  EXPECT_EQ(4, result.parsed_hand_size());
  EXPECT_TRUE(check(
      {CommonTestUtil::createAntoitsu(TileType::PINZU_1),
       CommonTestUtil::createAnshuntsu(TileType::PINZU_1),
       CommonTestUtil::createAnshuntsu(TileType::PINZU_1),
       CommonTestUtil::createAnshuntsu(TileType::PINZU_2),
       CommonTestUtil::createAnshuntsu(TileType::PINZU_2, 2)},
      MachiType::RYANMEN,
      AgariFormat::REGULAR_AGARI,
      result));
  EXPECT_TRUE(check(
      {CommonTestUtil::createAnkoutsu(TileType::PINZU_1),
       CommonTestUtil::createAnkoutsu(TileType::PINZU_2),
       CommonTestUtil::createAnkoutsu(TileType::PINZU_3),
       CommonTestUtil::createAnshuntsu(TileType::PINZU_1),
       CommonTestUtil::createAntoitsu(TileType::PINZU_4, true)},
      MachiType::TANKI,
      AgariFormat::REGULAR_AGARI,
      result));
  EXPECT_TRUE(check(
      {CommonTestUtil::createAnshuntsu(TileType::PINZU_1),
       CommonTestUtil::createAnshuntsu(TileType::PINZU_1),
       CommonTestUtil::createAnshuntsu(TileType::PINZU_1),
       CommonTestUtil::createAnshuntsu(TileType::PINZU_1),
       CommonTestUtil::createAntoitsu(TileType::PINZU_4, true)},
      MachiType::TANKI,
      AgariFormat::REGULAR_AGARI,
      result));
  EXPECT_TRUE(checkIrregularAgariFormatIsContained(result));
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

  HandParserResult result;
  _handParser.parse(hand, &result);

  // cout << HandParserResultUtil::getDebugString(result) << endl;

  EXPECT_EQ(6, result.parsed_hand_size());
  EXPECT_TRUE(check(
      {CommonTestUtil::createMintoitsu(TileType::PINZU_1, true),
       CommonTestUtil::createAnshuntsu(TileType::PINZU_1),
       CommonTestUtil::createAnshuntsu(TileType::PINZU_1),
       CommonTestUtil::createAnshuntsu(TileType::PINZU_2),
       CommonTestUtil::createAnshuntsu(TileType::PINZU_2)},
      MachiType::TANKI,
      AgariFormat::REGULAR_AGARI,
      result));
  EXPECT_TRUE(check(
      {CommonTestUtil::createAntoitsu(TileType::PINZU_1),
       CommonTestUtil::createMinshuntsu(TileType::PINZU_1, 0),
       CommonTestUtil::createAnshuntsu(TileType::PINZU_1),
       CommonTestUtil::createAnshuntsu(TileType::PINZU_2),
       CommonTestUtil::createAnshuntsu(TileType::PINZU_2)},
      MachiType::RYANMEN,
      AgariFormat::REGULAR_AGARI,
      result));
  EXPECT_TRUE(check(
      {CommonTestUtil::createMinkoutsu(TileType::PINZU_1, true),
       CommonTestUtil::createAnshuntsu(TileType::PINZU_1),
       CommonTestUtil::createAnkoutsu(TileType::PINZU_2),
       CommonTestUtil::createAnkoutsu(TileType::PINZU_3),
       CommonTestUtil::createAntoitsu(TileType::PINZU_4)},
      MachiType::SHABO,
      AgariFormat::REGULAR_AGARI,
      result));
  EXPECT_TRUE(check(
      {CommonTestUtil::createAnkoutsu(TileType::PINZU_1),
       CommonTestUtil::createMinshuntsu(TileType::PINZU_1, 0),
       CommonTestUtil::createAnkoutsu(TileType::PINZU_2),
       CommonTestUtil::createAnkoutsu(TileType::PINZU_3),
       CommonTestUtil::createAntoitsu(TileType::PINZU_4)},
      MachiType::RYANMEN,
      AgariFormat::REGULAR_AGARI,
      result));
  EXPECT_TRUE(check(
      {CommonTestUtil::createMinshuntsu(TileType::PINZU_1, 0),
       CommonTestUtil::createAnshuntsu(TileType::PINZU_1),
       CommonTestUtil::createAnshuntsu(TileType::PINZU_1),
       CommonTestUtil::createAnshuntsu(TileType::PINZU_1),
       CommonTestUtil::createAntoitsu(TileType::PINZU_4)},
      MachiType::RYANMEN,
      AgariFormat::REGULAR_AGARI,
      result));
  EXPECT_TRUE(checkIrregularAgariFormatIsContained(result));
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

  // cout << HandParserResultUtil::getDebugString(result) << endl;

  EXPECT_EQ(2, result.parsed_hand_size());
  EXPECT_TRUE(check(
      {CommonTestUtil::createAnshuntsu(TileType::PINZU_1),
       CommonTestUtil::createAnshuntsu(TileType::PINZU_5),
       CommonTestUtil::createAnshuntsu(TileType::SOUZU_1),
       CommonTestUtil::createAnshuntsu(TileType::WANZU_1),
       CommonTestUtil::createAntoitsu(TileType::WANZU_9, true)},
      MachiType::TANKI,
      AgariFormat::REGULAR_AGARI,
      result));
  EXPECT_TRUE(checkIrregularAgariFormatIsContained(result));
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

  // cout << HandParserResultUtil::getDebugString(result) << endl;

  EXPECT_EQ(1, result.parsed_hand_size());
  EXPECT_TRUE(checkIrregularAgariFormatIsContained(result));
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

  // cout << HandParserResultUtil::getDebugString(result) << endl;

  EXPECT_EQ(1, result.parsed_hand_size());
  EXPECT_TRUE(checkIrregularAgariFormatIsContained(result));
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

  // cout << HandParserResultUtil::getDebugString(result) << endl;

  EXPECT_EQ(1, result.parsed_hand_size());
  EXPECT_TRUE(checkIrregularAgariFormatIsContained(result));
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

  // cout << HandParserResultUtil::getDebugString(result) << endl;

  EXPECT_EQ(1, result.parsed_hand_size());
  EXPECT_TRUE(check(
      {CommonTestUtil::createAnkoutsu(TileType::PINZU_3),
       CommonTestUtil::createAntoitsu(TileType::PINZU_4, true),
       CommonTestUtil::createMinkoutsu(TileType::PINZU_2),
       CommonTestUtil::createAnkantsu(TileType::PINZU_1),
       CommonTestUtil::createMinshuntsu(TileType::PINZU_2)},
      MachiType::TANKI,
      AgariFormat::REGULAR_AGARI,
      result));
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

  // cout << HandParserResultUtil::getDebugString(result) << endl;

  EXPECT_EQ(3, result.parsed_hand_size());
  EXPECT_TRUE(check(
      {CommonTestUtil::createAntoitsu(TileType::PINZU_1),
       CommonTestUtil::createAnshuntsu(TileType::PINZU_1),
       CommonTestUtil::createAnshuntsu(TileType::PINZU_4),
       CommonTestUtil::createAnshuntsu(TileType::PINZU_7),
       CommonTestUtil::createMinkoutsu(TileType::PINZU_9, true)},
      MachiType::SHABO,
      AgariFormat::REGULAR_AGARI,
      result));
  EXPECT_TRUE(check(
      {CommonTestUtil::createAntoitsu(TileType::PINZU_1),
       CommonTestUtil::createAnshuntsu(TileType::PINZU_1),
       CommonTestUtil::createAnshuntsu(TileType::PINZU_4),
       CommonTestUtil::createMinshuntsu(TileType::PINZU_7, 2),
       CommonTestUtil::createAnkoutsu(TileType::PINZU_9)},
      MachiType::RYANMEN,
      AgariFormat::REGULAR_AGARI,
      result));
  EXPECT_TRUE(checkIrregularAgariFormatIsContained(result));
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

  // cout << HandParserResultUtil::getDebugString(result) << endl;

  EXPECT_EQ(5, result.parsed_hand_size());
  EXPECT_TRUE(check(
      {CommonTestUtil::createAntoitsu(TileType::PINZU_1),
       CommonTestUtil::createAnshuntsu(TileType::PINZU_2),
       CommonTestUtil::createAnshuntsu(TileType::PINZU_2),
       CommonTestUtil::createAnshuntsu(TileType::PINZU_5),
       CommonTestUtil::createAnshuntsu(TileType::PINZU_5, 2)},
      MachiType::PENCHAN,
      AgariFormat::REGULAR_AGARI,
      result));
  EXPECT_TRUE(check(
      {CommonTestUtil::createAnshuntsu(TileType::PINZU_1),
       CommonTestUtil::createAnshuntsu(TileType::PINZU_1),
       CommonTestUtil::createAntoitsu(TileType::PINZU_4),
       CommonTestUtil::createAnshuntsu(TileType::PINZU_5),
       CommonTestUtil::createAnshuntsu(TileType::PINZU_5, 2)},
      MachiType::PENCHAN,
      AgariFormat::REGULAR_AGARI,
      result));
  EXPECT_TRUE(check(
      {CommonTestUtil::createAnshuntsu(TileType::PINZU_1),
       CommonTestUtil::createAnshuntsu(TileType::PINZU_1),
       CommonTestUtil::createAnshuntsu(TileType::PINZU_4),
       CommonTestUtil::createAnshuntsu(TileType::PINZU_4),
       CommonTestUtil::createAntoitsu(TileType::PINZU_7, true)},
      MachiType::TANKI,
      AgariFormat::REGULAR_AGARI,
      result));
  EXPECT_TRUE(check(
      {CommonTestUtil::createAntoitsu(TileType::PINZU_1),
       CommonTestUtil::createAntoitsu(TileType::PINZU_2),
       CommonTestUtil::createAntoitsu(TileType::PINZU_3),
       CommonTestUtil::createAntoitsu(TileType::PINZU_4),
       CommonTestUtil::createAntoitsu(TileType::PINZU_5),
       CommonTestUtil::createAntoitsu(TileType::PINZU_6),
       CommonTestUtil::createAntoitsu(TileType::PINZU_7, true)},
      MachiType::TANKI,
      AgariFormat::CHITOITSU_AGARI,
      result));
  EXPECT_TRUE(checkIrregularAgariFormatIsContained(result));
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

  // cout << HandParserResultUtil::getDebugString(result) << endl;

  EXPECT_EQ(2, result.parsed_hand_size());
  EXPECT_TRUE(check(
      {CommonTestUtil::createAntoitsu(TileType::PINZU_1),
       CommonTestUtil::createAntoitsu(TileType::PINZU_2),
       CommonTestUtil::createAntoitsu(TileType::SANGEN_HAKU),
       CommonTestUtil::createAntoitsu(TileType::SANGEN_CHUN),
       CommonTestUtil::createAntoitsu(TileType::SANGEN_HATSU),
       CommonTestUtil::createAntoitsu(TileType::WIND_TON),
       CommonTestUtil::createMintoitsu(TileType::WIND_NAN, true)},
      MachiType::TANKI,
      AgariFormat::CHITOITSU_AGARI,
      result));
  EXPECT_TRUE(checkIrregularAgariFormatIsContained(result));
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

  // cout << HandParserResultUtil::getDebugString(result) << endl;

  EXPECT_EQ(1, result.parsed_hand_size());
  EXPECT_TRUE(checkIrregularAgariFormatIsContained(result));
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

  // cout << HandParserResultUtil::getDebugString(result) << endl;

  EXPECT_EQ(2, result.parsed_hand_size());
  EXPECT_TRUE(check(
      {CommonTestUtil::createAntoitsu(TileType::PINZU_1),
       CommonTestUtil::createAnkoutsu(TileType::SANGEN_HAKU),
       CommonTestUtil::createAnkoutsu(TileType::SANGEN_HATSU),
       CommonTestUtil::createAnkoutsu(TileType::SANGEN_CHUN),
       CommonTestUtil::createMinshuntsu(TileType::SOUZU_1, 0)},
      MachiType::RYANMEN,
      AgariFormat::REGULAR_AGARI,
      result));
  EXPECT_TRUE(checkIrregularAgariFormatIsContained(result));
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

  // cout << HandParserResultUtil::getDebugString(result) << endl;

  EXPECT_EQ(2, result.parsed_hand_size());
  EXPECT_TRUE(check(
      {CommonTestUtil::createAntoitsu(TileType::PINZU_1),
       CommonTestUtil::createAnkoutsu(TileType::SANGEN_HAKU),
       CommonTestUtil::createAnkoutsu(TileType::SANGEN_HATSU),
       CommonTestUtil::createAnkoutsu(TileType::SANGEN_CHUN),
       CommonTestUtil::createMinshuntsu(TileType::SOUZU_1, 2)},
      MachiType::PENCHAN,
      AgariFormat::REGULAR_AGARI,
      result));
  EXPECT_TRUE(checkIrregularAgariFormatIsContained(result));
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

  // cout << HandParserResultUtil::getDebugString(result) << endl;

  EXPECT_EQ(2, result.parsed_hand_size());
  EXPECT_TRUE(check(
      {CommonTestUtil::createAntoitsu(TileType::PINZU_1),
       CommonTestUtil::createAnkoutsu(TileType::SANGEN_HAKU),
       CommonTestUtil::createAnkoutsu(TileType::SANGEN_HATSU),
       CommonTestUtil::createAnkoutsu(TileType::SANGEN_CHUN),
       CommonTestUtil::createMinshuntsu(TileType::SOUZU_7, 0)},
      MachiType::PENCHAN,
      AgariFormat::REGULAR_AGARI,
      result));
  EXPECT_TRUE(checkIrregularAgariFormatIsContained(result));
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

  // cout << HandParserResultUtil::getDebugString(result) << endl;

  EXPECT_EQ(2, result.parsed_hand_size());
  EXPECT_TRUE(check(
      {CommonTestUtil::createAntoitsu(TileType::PINZU_1),
       CommonTestUtil::createAnkoutsu(TileType::SANGEN_HAKU),
       CommonTestUtil::createAnkoutsu(TileType::SANGEN_HATSU),
       CommonTestUtil::createAnkoutsu(TileType::SANGEN_CHUN),
       CommonTestUtil::createMinshuntsu(TileType::SOUZU_2, 1)},
      MachiType::KANCHAN,
      AgariFormat::REGULAR_AGARI,
      result));
  EXPECT_TRUE(checkIrregularAgariFormatIsContained(result));
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

  // cout << HandParserResultUtil::getDebugString(result) << endl;

  EXPECT_EQ(2, result.parsed_hand_size());
  EXPECT_TRUE(check(
      {CommonTestUtil::createAntoitsu(TileType::PINZU_1),
       CommonTestUtil::createAnkoutsu(TileType::SANGEN_HAKU),
       CommonTestUtil::createAnkoutsu(TileType::SANGEN_HATSU),
       CommonTestUtil::createAnkoutsu(TileType::SANGEN_CHUN),
       CommonTestUtil::createMinkoutsu(TileType::SOUZU_1, true)},
      MachiType::SHABO,
      AgariFormat::REGULAR_AGARI,
      result));
  EXPECT_TRUE(checkIrregularAgariFormatIsContained(result));
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

  // cout << HandParserResultUtil::getDebugString(result) << endl;

  EXPECT_EQ(2, result.parsed_hand_size());
  EXPECT_TRUE(check(
      {CommonTestUtil::createAnkoutsu(TileType::PINZU_1),
       CommonTestUtil::createAnkoutsu(TileType::SANGEN_HAKU),
       CommonTestUtil::createAnkoutsu(TileType::SANGEN_HATSU),
       CommonTestUtil::createAnkoutsu(TileType::SANGEN_CHUN),
       CommonTestUtil::createMintoitsu(TileType::SOUZU_1, true)},
      MachiType::TANKI,
      AgariFormat::REGULAR_AGARI,
      result));
  EXPECT_TRUE(checkIrregularAgariFormatIsContained(result));
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

  // cout << HandParserResultUtil::getDebugString(result) << endl;

  EXPECT_EQ(2, result.parsed_hand_size());
  EXPECT_TRUE(check(
      {CommonTestUtil::createAntoitsu(TileType::PINZU_1),
       CommonTestUtil::createAntoitsu(TileType::SANGEN_HAKU),
       CommonTestUtil::createAntoitsu(TileType::SANGEN_HATSU),
       CommonTestUtil::createAntoitsu(TileType::SANGEN_CHUN),
       CommonTestUtil::createAntoitsu(TileType::WIND_NAN),
       CommonTestUtil::createAntoitsu(TileType::WIND_PE),
       CommonTestUtil::createMintoitsu(TileType::SOUZU_1, true)},
      MachiType::TANKI,
      AgariFormat::CHITOITSU_AGARI,
      result));
  EXPECT_TRUE(checkIrregularAgariFormatIsContained(result));
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

  // cout << HandParserResultUtil::getDebugString(result) << endl;

  EXPECT_EQ(1, result.parsed_hand_size());
  EXPECT_TRUE(checkIrregularAgariFormatIsContained(result));
}
