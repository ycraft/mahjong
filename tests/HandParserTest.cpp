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
  hand.set_richi_type(RichiType::RICHI);
  hand.set_agari_type(AgariType::RON);

  HandParserResult result;
  _handParser.parse(hand, &result);

  // cout << HandParserResultUtil::getDebugString(result) << endl;

  EXPECT_EQ(4, result.parsed_hand_size());
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
  hand.set_richi_type(RichiType::RICHI);
  hand.set_agari_type(AgariType::TSUMO);

  HandParserResult result;
  _handParser.parse(hand, &result);

  // cout << HandParserResultUtil::getDebugString(result) << endl;

  EXPECT_EQ(6, result.parsed_hand_size());
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
  hand.set_richi_type(RichiType::RICHI);
  hand.set_agari_type(AgariType::TSUMO);

  HandParserResult result;
  _handParser.parse(hand, &result);

  // cout << HandParserResultUtil::getDebugString(result) << endl;

  EXPECT_EQ(2, result.parsed_hand_size());
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
  hand.set_richi_type(RichiType::RICHI);
  hand.set_agari_type(AgariType::TSUMO);

  HandParserResult result;
  _handParser.parse(hand, &result);

  // cout << HandParserResultUtil::getDebugString(result) << endl;

  EXPECT_EQ(1, result.parsed_hand_size());
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
  hand.set_richi_type(RichiType::RICHI);
  hand.set_agari_type(AgariType::TSUMO);

  HandParserResult result;
  _handParser.parse(hand, &result);

  // cout << HandParserResultUtil::getDebugString(result) << endl;

  EXPECT_EQ(1, result.parsed_hand_size());
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
  hand.set_richi_type(RichiType::RICHI);
  hand.set_agari_type(AgariType::TSUMO);

  HandParserResult result;
  _handParser.parse(hand, &result);

  // cout << HandParserResultUtil::getDebugString(result) << endl;

  EXPECT_EQ(1, result.parsed_hand_size());
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
  hand.set_richi_type(RichiType::RICHI);
  hand.set_agari_type(AgariType::TSUMO);

  HandParserResult result;
  _handParser.parse(hand, &result);

  // cout << HandParserResultUtil::getDebugString(result) << endl;

  EXPECT_EQ(1, result.parsed_hand_size());
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
  hand.set_richi_type(RichiType::RICHI);
  hand.set_agari_type(AgariType::TSUMO);

  HandParserResult result;
  _handParser.parse(hand, &result);

  // cout << HandParserResultUtil::getDebugString(result) << endl;

  EXPECT_EQ(3, result.parsed_hand_size());
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
  hand.set_richi_type(RichiType::RICHI);
  hand.set_agari_type(AgariType::TSUMO);

  HandParserResult result;
  _handParser.parse(hand, &result);

  // cout << HandParserResultUtil::getDebugString(result) << endl;

  EXPECT_EQ(5, result.parsed_hand_size());
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
  hand.set_richi_type(RichiType::RICHI);
  hand.set_agari_type(AgariType::TSUMO);

  HandParserResult result;
  _handParser.parse(hand, &result);

  // cout << HandParserResultUtil::getDebugString(result) << endl;

  EXPECT_EQ(2, result.parsed_hand_size());
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
  hand.set_richi_type(RichiType::RICHI);
  hand.set_agari_type(AgariType::TSUMO);

  HandParserResult result;
  _handParser.parse(hand, &result);

  // cout << HandParserResultUtil::getDebugString(result) << endl;

  EXPECT_EQ(1, result.parsed_hand_size());
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
  hand.set_richi_type(RichiType::RICHI);
  hand.set_agari_type(AgariType::TSUMO);

  HandParserResult result;
  _handParser.parse(hand, &result);

  // cout << HandParserResultUtil::getDebugString(result) << endl;

  EXPECT_EQ(2, result.parsed_hand_size());

  EXPECT_TRUE(result.parsed_hand(0).is_agarikei());
  EXPECT_EQ(MachiType::RYANMEN, result.parsed_hand(0).machi_type());

  EXPECT_FALSE(result.parsed_hand(1).is_agarikei());
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
  hand.set_richi_type(RichiType::RICHI);
  hand.set_agari_type(AgariType::TSUMO);

  HandParserResult result;
  _handParser.parse(hand, &result);

  // cout << HandParserResultUtil::getDebugString(result) << endl;

  EXPECT_EQ(2, result.parsed_hand_size());

  EXPECT_TRUE(result.parsed_hand(0).is_agarikei());
  EXPECT_EQ(MachiType::PENCHAN, result.parsed_hand(0).machi_type());

  EXPECT_FALSE(result.parsed_hand(1).is_agarikei());
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
  hand.set_richi_type(RichiType::RICHI);
  hand.set_agari_type(AgariType::TSUMO);

  HandParserResult result;
  _handParser.parse(hand, &result);

  // cout << HandParserResultUtil::getDebugString(result) << endl;

  EXPECT_EQ(2, result.parsed_hand_size());

  EXPECT_TRUE(result.parsed_hand(0).is_agarikei());
  EXPECT_EQ(MachiType::PENCHAN, result.parsed_hand(0).machi_type());

  EXPECT_FALSE(result.parsed_hand(1).is_agarikei());
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
  hand.set_richi_type(RichiType::RICHI);
  hand.set_agari_type(AgariType::TSUMO);

  HandParserResult result;
  _handParser.parse(hand, &result);

  // cout << HandParserResultUtil::getDebugString(result) << endl;

  EXPECT_EQ(2, result.parsed_hand_size());

  EXPECT_TRUE(result.parsed_hand(0).is_agarikei());
  EXPECT_EQ(MachiType::KANCHAN, result.parsed_hand(0).machi_type());

  EXPECT_FALSE(result.parsed_hand(1).is_agarikei());
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
  hand.set_richi_type(RichiType::RICHI);
  hand.set_agari_type(AgariType::TSUMO);

  HandParserResult result;
  _handParser.parse(hand, &result);

  // cout << HandParserResultUtil::getDebugString(result) << endl;

  EXPECT_EQ(2, result.parsed_hand_size());

  EXPECT_TRUE(result.parsed_hand(0).is_agarikei());
  EXPECT_EQ(MachiType::SHABO, result.parsed_hand(0).machi_type());

  EXPECT_FALSE(result.parsed_hand(1).is_agarikei());
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
  hand.set_richi_type(RichiType::RICHI);
  hand.set_agari_type(AgariType::TSUMO);

  HandParserResult result;
  _handParser.parse(hand, &result);

  // cout << HandParserResultUtil::getDebugString(result) << endl;

  EXPECT_EQ(2, result.parsed_hand_size());

  EXPECT_TRUE(result.parsed_hand(0).is_agarikei());
  EXPECT_EQ(MachiType::TANKI, result.parsed_hand(0).machi_type());

  EXPECT_FALSE(result.parsed_hand(1).is_agarikei());
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
  hand.set_richi_type(RichiType::RICHI);
  hand.set_agari_type(AgariType::TSUMO);

  HandParserResult result;
  _handParser.parse(hand, &result);

  // cout << HandParserResultUtil::getDebugString(result) << endl;

  EXPECT_EQ(2, result.parsed_hand_size());

  EXPECT_TRUE(result.parsed_hand(0).is_agarikei());
  EXPECT_EQ(MachiType::TANKI, result.parsed_hand(0).machi_type());

  EXPECT_FALSE(result.parsed_hand(1).is_agarikei());
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
  hand.set_richi_type(RichiType::RICHI);
  hand.set_agari_type(AgariType::TSUMO);

  HandParserResult result;
  _handParser.parse(hand, &result);

  // cout << HandParserResultUtil::getDebugString(result) << endl;

  EXPECT_EQ(1, result.parsed_hand_size());

  EXPECT_FALSE(result.parsed_hand(0).is_agarikei());
  EXPECT_EQ(MachiType::UNKNOWN_MACHI_TYPE, result.parsed_hand(0).machi_type());
}
