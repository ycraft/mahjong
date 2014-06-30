/*
 * ValidatorUtils.h
 *
 *  Created on: 2014/06/26
 *      Author: yuuki
 */
#ifndef VALIDATORUTILS_H_
#define VALIDATORUTILS_H_

#include <functional>
#include <vector>
#include <iostream>

#include "MahjongCommonUtils.h"

namespace ydec {
namespace msc {

class ValidatorUtils {
 private:
  ValidatorUtils();
  virtual ~ValidatorUtils() = 0;

 public:
  template<typename V_IN, typename V_OUT, typename C>
  static std::function<bool(const V_IN&, const V_IN&)> comp(const C& comparator) {
    return [&comparator](const V_IN& l, const V_IN& r) {
      return comparator(static_cast<V_OUT>(l), static_cast<V_OUT>(r));
    };
  }

  template<typename V_OUT, typename C>
  static std::function<bool(const int&, const int&)> enumComp(const C& comparator) {
    return comp<int, V_OUT, C>(comparator);
  }

  template<typename T, typename U, typename C>
  static bool validateRequiredItems(const T& required_items,
                                    const U& actual_items,
                                    const C& comparator) {
    if (required_items.size() == 0) {
      return true;
    }

    if (required_items.size() > actual_items.size()) {
      return false;
    }

    std::vector<bool> used(actual_items.size(), false);
    for (const typename T::value_type& required_item : required_items) {
      bool found = false;
      int i = 0;
      for (typename U::const_iterator iter = actual_items.begin();
          iter != actual_items.end();
          ++iter, ++i) {
        if (used[i]) {
          continue;
        }

        if (!comparator(required_item, *iter)) {
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
};

} /* namespace msc */
} /* namespace ydec */

#endif /* VALIDATORUTILS_H_ */
