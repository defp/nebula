/* Copyright (c) 2021 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License.
 */

#ifndef GRAPH_OPTIMIZER_RULE_PUSHLIMITDOWNINDEXSCANRULE_H
#define GRAPH_OPTIMIZER_RULE_PUSHLIMITDOWNINDEXSCANRULE_H

#include <initializer_list>

#include "graph/optimizer/OptRule.h"

namespace nebula {
namespace opt {

class PushLimitDownIndexScanRule final : public OptRule {
 public:
  const Pattern &pattern() const override;

  StatusOr<OptRule::TransformResult> transform(OptContext *ctx,
                                               const MatchedResult &matched) const override;

  std::string toString() const override;

 private:
  PushLimitDownIndexScanRule();

  static std::unique_ptr<OptRule> kInstance;

  static const std::initializer_list<graph::PlanNode::Kind> kIndexScanKinds;
};

}  // namespace opt
}  // namespace nebula
#endif
