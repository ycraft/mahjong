#include <iostream>
#include <algorithm>

#include "src-gen/cpp/mahjong-scorecalculator.pb.h"

using namespace std;
using namespace mahjong;

Field* createField() {
  Field* field = new Field();
  field->add_doras(WANZU_1);
  field->set_honba(0);
  field->set_wind(WIND_TON);
  return field;
}

Hand* createHand() {
  Hand* hand = new Hand();
  hand->add_closed_tiles(WANZU_1);
  hand->add_closed_tiles(WANZU_1);
  hand->add_closed_tiles(WANZU_1);
  hand->add_closed_tiles(WANZU_2);
  hand->add_closed_tiles(WANZU_2);
  hand->add_closed_tiles(WANZU_2);
  hand->add_closed_tiles(WANZU_3);
  hand->add_closed_tiles(WANZU_3);
  hand->add_closed_tiles(WANZU_3);
  hand->add_closed_tiles(WANZU_4);
  hand->add_closed_tiles(WANZU_4);
  hand->add_closed_tiles(WANZU_4);
  hand->add_closed_tiles(WANZU_5);

  hand->set_agari_tile(WANZU_5);

  hand->set_is_reached(true);
  hand->set_is_tsumo(true);

  return hand;
}

Hand* createHand2() {
  Hand* hand = new Hand();
  hand->add_closed_tiles(WIND_TON);
  hand->add_closed_tiles(WIND_TON);
  hand->add_closed_tiles(WIND_TON);
  hand->add_closed_tiles(WIND_NAN);
  hand->add_closed_tiles(WIND_NAN);
  hand->add_closed_tiles(WIND_NAN);
  hand->add_closed_tiles(WIND_SHA);
  hand->add_closed_tiles(WIND_SHA);
  hand->add_closed_tiles(WIND_SHA);
  hand->add_closed_tiles(WIND_PE);
  hand->add_closed_tiles(WIND_PE);
  hand->add_closed_tiles(WIND_PE);
  hand->add_closed_tiles(SANGEN_HAKU);

  hand->set_agari_tile(SANGEN_HAKU);

  hand->set_is_reached(true);
  hand->set_is_tsumo(true);

  return hand;
}

Player* createPlayer() {
  Player* player = new Player();
  player->set_allocated_my_hand(createHand());
  player->set_my_wind(WIND_TON);
  return player;
}

bool isSequentialTileType(TileType tile) {
  return (tile & MASK_TILE_SEQUENTIAL) == SEQUENTIAL_TILE;
}

class ScoreCalculator {
 public:
  ScoreCalculator(const Field& field, const Player& player)
      : _field(field),
        _player(player),
        _hand(player.my_hand()),
        _num_free_tiles(_hand.closed_tiles_size() + 1) {
    _free_tiles = new TileType[_num_free_tiles];
    for (int i = 0; i < _hand.closed_tiles_size(); ++i) {
      _free_tiles[i] = _hand.closed_tiles(i);
    }
    _free_tiles[_num_free_tiles - 1] = _hand.agari_tile();

    sort(_free_tiles, _free_tiles + _num_free_tiles);

    _free_tile_group_ids = new unsigned int[_num_free_tiles];
    memset(_free_tile_group_ids, 0,
           _num_free_tiles * sizeof(_free_tile_group_ids[0]));
  }
  ;

  ~ScoreCalculator() {
    delete[] _free_tiles;
    delete[] _free_tile_group_ids;
  }

 private:
  const Field& _field;
  const Player& _player;
  const Hand& _hand;

  const unsigned int _num_free_tiles;
  unsigned int* _free_tile_group_ids;
  TileType* _free_tiles;

  void printDebugString() {
    cout << "group: ";
    for (int i = 0; i < _num_free_tiles; ++i) {
      cout << _free_tile_group_ids[i];
    }
    cout << endl;
  }

  void startDfs() {
    dfs(0, 1, false);
  }

  void dfs(int i, int id, bool has_jantou) {
//    cout << i << ", " << id << ", " << has_jantou << ", ";
//    printDebugString();

    if (i == _num_free_tiles && has_jantou) {
      // calc score here.
      cout << "OK -> ";
      printDebugString();
      return;
    }

    if (_free_tile_group_ids[i]) {
      dfs(i + 1, id, has_jantou);
      return;
    }

    _free_tile_group_ids[i] = id;

    // anko or jantou
    for (int j = i + 1, counter = 0; j < _num_free_tiles && _free_tiles[i] == _free_tiles[j]; ++j) {
      if (_free_tile_group_ids[j] == 0) {
        ++counter;
        _free_tile_group_ids[j] = id;

        if (!has_jantou && counter == 1) {
          dfs(j + 1, id + 1, true);
        } else if (counter == 2) {
          dfs(j + 1, id + 1, has_jantou);
          break;
        }
      }
    }

    for (int j = i + 1; j < _num_free_tiles && _free_tiles[i] == _free_tiles[j]; ++j) {
      if (_free_tile_group_ids[j] == id) {
        _free_tile_group_ids[j] = 0;
      }
    }

    // shuntsu
    if (isSequentialTileType(_free_tiles[i])) {
      TileType prev = _free_tiles[i];
      for (int j = i + 1, counter = 0; j < _num_free_tiles && _free_tiles[j] - prev <= 1; ++j) {
        if (_free_tile_group_ids[j] == 0 && _free_tiles[j] - prev == 1) {
          ++counter;
          prev = _free_tiles[j];
          _free_tile_group_ids[j] = id;

          if (counter == 2) {
            dfs(i + 1, id + 1, has_jantou);
            break;
          }
        }
      }
    }

    for (int j = i; j < _num_free_tiles; ++j) {
      if (_free_tile_group_ids[j] == id) {
        _free_tile_group_ids[j] = 0;
      }
    }
  }

 public:
  ScoreCalculationResponse* calculate() {
    for (int i = 0; i < _num_free_tiles; ++i) {
      cout << _free_tiles[i] << ",";
    }
    cout << endl;

    startDfs();
    ScoreCalculationResponse* response = new ScoreCalculationResponse();
    response->set_fan(2);
    response->set_fu(40);
    return response;
  }
};

int main() {
  cout << "start" << endl;
  Field* field = createField();
  Player* player = createPlayer();
  ScoreCalculator scoreCalculator(*field, *player);
  ScoreCalculationResponse* response = scoreCalculator.calculate();
  cout << response->DebugString() << endl;
  cout << "end" << endl;
  return 0;
}
