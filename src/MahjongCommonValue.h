#ifndef MAHJONGCOMMONVALUE_H_
#define MAHJONGCOMMONVALUE_H_

namespace ydec {
namespace mahjong {

class MahjongCommonValue {
 private:
  MahjongCommonValue() {};
  virtual ~MahjongCommonValue() = 0;

 public:
  static const int KAZOE_YAKUMAN_FAN = 13;
};

} /* namespace mahjong */
} /* namespace ydec */

#endif /* MAHJONGCOMMONVALUE_H_ */
