#ifndef YAKUAPPLIER_H_
#define YAKUAPPLIER_H_

#include "src-gen/cpp/mahjong-common.pb.h"
#include "src-gen/cpp/mahjong-rule.pb.h"
#include "src-gen/cpp/mahjong-scorecalculator.pb.h"

namespace ydec {
namespace msc{

class YakuApplier {
 public:
  // Take rule's ownership
  YakuApplier(mahjong::Rule* rule);
  virtual ~YakuApplier();

  void apply(const ParsedHand& parsed_hand, YakuApplierResult* result) const;

 private:
  mahjong::Rule* rule_;
};

class YakuConditionValidator {
 public:
  YakuConditionValidator(const mahjong::YakuCondition& condition,
                         const ParsedHand& parsed_hand);
  bool validate();

 private:
  const mahjong::YakuCondition& condition_;
  const ParsedHand& parsed_hand_;

  bool validateAllowedTileCondition(
      const ::google::protobuf::RepeatedPtrField<mahjong::TileCondition >& condition,
      const mahjong::TileType& tile);
};

} /* namespace msc */
} /* namespace ydec */

#endif /* YAKUAPPLIER_H_ */
