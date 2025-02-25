/* Copyright (c) 2021 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License.
 */

#ifndef _UTIL_EXPRESSION_UTILS_H_
#define _UTIL_EXPRESSION_UTILS_H_

#include "common/base/Status.h"
#include "common/expression/AggregateExpression.h"
#include "common/expression/BinaryExpression.h"
#include "common/expression/Expression.h"
#include "common/expression/FunctionCallExpression.h"
#include "common/expression/LabelExpression.h"
#include "common/expression/PropertyExpression.h"
#include "common/expression/TypeCastingExpression.h"
#include "common/expression/UnaryExpression.h"
#include "graph/context/ast/CypherAstContext.h"
#include "graph/visitor/EvaluableExprVisitor.h"
#include "graph/visitor/FindVisitor.h"
#include "graph/visitor/RewriteVisitor.h"

DECLARE_int32(max_expression_depth);

namespace nebula {
class ObjectPool;
namespace graph {
class ExpressionUtils {
 public:
  explicit ExpressionUtils(...) = delete;

  static inline bool isKindOf(const Expression* expr,
                              const std::unordered_set<Expression::Kind>& expected) {
    return expected.find(expr->kind()) != expected.end();
  }

  static bool isPropertyExpr(const Expression* expr);

  static const Expression* findAny(const Expression* self,
                                   const std::unordered_set<Expression::Kind>& expected);

  static bool hasAny(const Expression* expr, const std::unordered_set<Expression::Kind>& expected) {
    return findAny(expr, expected) != nullptr;
  }

  static std::vector<const Expression*> collectAll(
      const Expression* self, const std::unordered_set<Expression::Kind>& expected);

  static bool checkVarExprIfExist(const Expression* expr, const QueryContext* qctx);

  static std::vector<const Expression*> findAllStorage(const Expression* expr);

  static std::vector<const Expression*> findAllInputVariableProp(const Expression* expr);

  // **Expression type check**
  static bool isConstExpr(const Expression* expr);

  static bool isEvaluableExpr(const Expression* expr, const QueryContext* qctx = nullptr);

  static Expression* rewriteAttr2LabelTagProp(
      const Expression* expr, const std::unordered_map<std::string, AliasType>& aliasTypeMap);

  static Expression* rewriteLabelAttr2TagProp(const Expression* expr);

  static Expression* rewriteLabelAttr2EdgeProp(const Expression* expr);

  static Expression* rewriteAgg2VarProp(const Expression* expr);

  static Expression* rewriteInnerVar(const Expression* expr, std::string newVar);

  static Expression* rewriteParameter(const Expression* expr, QueryContext* qctx);

  // Rewrite relational expression, gather evaluable expressions to one side
  static Expression* rewriteRelExpr(const Expression* expr);
  static Expression* rewriteRelExprHelper(const Expression* expr, Expression*& relRightOperandExpr);

  // Rewrite IN expression into OR expression or relEQ expression
  static Expression* rewriteInExpr(const Expression* expr);

  // Rewrite Logical AND expr to Logical OR expr using distributive law
  // Examples:
  // A and (B or C)  => (A and B) or (A and C)
  // (A or B) and (C or D)  =>  (A and C) or (A and D) or (B and C) or (B or D)
  static Expression* rewriteLogicalAndToLogicalOr(const Expression* expr);

  // Return the operands of container expressions
  // For list and set, return the operands
  // For map, return the keys
  static std::vector<Expression*> getContainerExprOperands(const Expression* expr);

  // Clone and fold constant expression
  static StatusOr<Expression*> foldConstantExpr(const Expression* expr);

  // Clone and reduce unaryNot expression
  static Expression* reduceUnaryNotExpr(const Expression* expr);

  // Transform filter using multiple expression rewrite strategies
  // 1. rewrite relational expressions containing arithmetic operands so that
  //    all constants are on the right side of relExpr.
  // 2. fold constant
  // 3. reduce unary expression e.g. !(A and B) => !A or !B
  static StatusOr<Expression*> filterTransform(const Expression* expr);

  // Negate the given logical expr: (A && B) -> (!A || !B)
  static LogicalExpression* reverseLogicalExpr(LogicalExpression* expr);

  // Negate the given relational expr: (A > B) -> (A <= B)
  static RelationalExpression* reverseRelExpr(RelationalExpression* expr);

  // Return the negation of the given relational kind
  static Expression::Kind getNegatedRelExprKind(const Expression::Kind kind);

  // Return the negation of the given logical kind
  static Expression::Kind getNegatedLogicalExprKind(const Expression::Kind kind);

  // Return the negation of the given arithmetic kind: plus -> minus
  static Expression::Kind getNegatedArithmeticType(const Expression::Kind kind);

  static void pullAnds(Expression* expr);

  static void pullAndsImpl(LogicalExpression* expr, std::vector<Expression*>& operands);

  static void pullOrs(Expression* expr);
  static void pullOrsImpl(LogicalExpression* expr, std::vector<Expression*>& operands);

  static Expression* pushOrs(ObjectPool* pool, const std::vector<Expression*>& rels);

  static Expression* pushAnds(ObjectPool* pool, const std::vector<Expression*>& rels);

  static Expression* pushImpl(ObjectPool* pool,
                              Expression::Kind kind,
                              const std::vector<Expression*>& rels);

  static Expression* flattenInnerLogicalAndExpr(const Expression* expr);

  static Expression* flattenInnerLogicalOrExpr(const Expression* expr);

  static Expression* flattenInnerLogicalExpr(const Expression* expr);

  static void splitFilter(const Expression* expr,
                          std::function<bool(const Expression*)> picker,
                          Expression** filterPicked,
                          Expression** filterUnpicked);

  static Expression* expandExpr(ObjectPool* pool, const Expression* expr);

  static Expression* expandImplAnd(ObjectPool* pool, const Expression* expr);

  static std::vector<Expression*> expandImplOr(const Expression* expr);

  static Status checkAggExpr(const AggregateExpression* aggExpr);

  static bool findInnerRandFunction(const Expression* expr);

  // loop condition
  // ++loopSteps <= steps
  static Expression* stepCondition(ObjectPool* pool, const std::string& loopStep, uint32_t steps);

  // size(var) == 0
  static Expression* zeroCondition(ObjectPool* pool, const std::string& var);

  // size(var) != 0
  static Expression* neZeroCondition(ObjectPool* pool, const std::string& var);

  // var == value
  static Expression* equalCondition(ObjectPool* pool, const std::string& var, const Value& value);

  // TODO(jie) Move it to a better place
  static bool isGeoIndexAcceleratedPredicate(const Expression* expr);

  static bool checkExprDepth(const Expression* expr);
};

}  // namespace graph
}  // namespace nebula

#endif  // _UTIL_EXPRESSION_UTILS_H_
