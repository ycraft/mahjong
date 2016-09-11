#ifndef YDEC_MAHJONG_MAHJONG_COMMON_VALUE_H_
#define YDEC_MAHJONG_MAHJONG_COMMON_VALUE_H_

namespace ydec {
namespace mahjong {

class MahjongCommonValue {
 private:
  MahjongCommonValue() {};
  virtual ~MahjongCommonValue() = 0;

 public:
  static const int KAZOE_YAKUMAN_FAN;
};

}  // namespace mahjong
}  // namespace ydec

#endif  // YDEC_MAHJONG_MAHJONG_COMMON_VALUE_H_
