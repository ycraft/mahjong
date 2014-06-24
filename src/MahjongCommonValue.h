/*
 * MahjongCommonValue.h
 *
 *  Created on: 2014/06/24
 *      Author: yuuki
 */

#ifndef MAHJONGCOMMONVALUE_H_
#define MAHJONGCOMMONVALUE_H_

namespace ydec {
namespace msc {

class MahjongCommonValue {
 private:
  MahjongCommonValue() {};
  virtual ~MahjongCommonValue() = 0;

 public:
  static const int KAZOE_YAKUMAN_FAN = 13;
};

} /* namespace msc */
} /* namespace ydec */

#endif /* MAHJONGCOMMONVALUE_H_ */
