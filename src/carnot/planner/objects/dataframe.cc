/*
 * Copyright 2018- The Pixie Authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "src/carnot/planner/objects/dataframe.h"
#include "src/carnot/planner/ir/ast_utils.h"
#include "src/carnot/planner/objects/collection_object.h"
#include "src/carnot/planner/objects/expr_object.h"
#include "src/carnot/planner/objects/funcobject.h"
#include "src/carnot/planner/objects/metadata_object.h"
#include "src/carnot/planner/objects/none_object.h"
#include "src/carnot/planner/objects/pixie_module.h"

namespace px {
namespace carnot {
namespace planner {
namespace compiler {

StatusOr<std::shared_ptr<Dataframe>> Dataframe::Create(OperatorIR* op, ASTVisitor* visitor) {
  std::shared_ptr<Dataframe> df(new Dataframe(op, op->graph(), visitor));
  PL_RETURN_IF_ERROR(df->Init());
  return df;
}

StatusOr<std::shared_ptr<Dataframe>> Dataframe::Create(IR* graph, ASTVisitor* visitor) {
  std::shared_ptr<Dataframe> df(new Dataframe(nullptr, graph, visitor));
  PL_RETURN_IF_ERROR(df->Init());
  return df;
}

// Parses elements of type TIRNodeType, either a single one or a collection, as a vector of
// TIRNodeType. Used for dataframe methods that take either a list or single item, like drop.
// drop('foo') and drop(['foo', 'bar']) for example.
template <typename TIRNodeType>
StatusOr<std::vector<TIRNodeType*>> ParseAsListOf(QLObjectPtr obj, std::string_view arg_name) {
  std::vector<TIRNodeType*> result;
  bool with_index = CollectionObject::IsCollection(obj);
  for (const auto& [idx, item] : Enumerate(ObjectAsCollection(obj))) {
    std::string name = std::string(arg_name);
    if (with_index) {
      name = absl::Substitute("$0 (index $1)", arg_name, idx);
    }
    PL_ASSIGN_OR_RETURN(auto casted, GetArgAs<TIRNodeType>(item, name));
    result.push_back(casted);
  }
  return result;
}

StatusOr<std::vector<std::string>> ParseAsListOfStrings(QLObjectPtr obj,
                                                        std::string_view arg_name) {
  PL_ASSIGN_OR_RETURN(auto string_irs, ParseAsListOf<StringIR>(obj, arg_name));
  std::vector<std::string> strs;
  strs.reserve(string_irs.size());
  for (StringIR* str : string_irs) {
    strs.push_back(str->str());
  }
  return strs;
}

Status Dataframe::Init() {
  PL_ASSIGN_OR_RETURN(
      std::shared_ptr<FuncObject> constructor_fn,
      FuncObject::Create(name(), {"table", "select", "start_time", "end_time"},
                         {{"select", "[]"},
                          {"start_time", "0"},
                          {"end_time", absl::Substitute("$0.$1()", PixieModule::kPixieModuleObjName,
                                                        PixieModule::kNowOpID)}},
                         /* has_variable_len_args */ false,
                         /* has_variable_len_kwargs */ false,
                         std::bind(&DataFrameHandler::Eval, graph(), std::placeholders::_1,
                                   std::placeholders::_2, std::placeholders::_3),
                         ast_visitor()));
  AddCallMethod(constructor_fn);
  PL_RETURN_IF_ERROR(constructor_fn->SetDocString(kDataFrameConstuctorDocString));
  // If the op is null, then don't init the other funcs.
  if (op() == nullptr) {
    return Status::OK();
  }

  /**
   * # Equivalent to the python method method syntax:
   * def merge(self, right, how, left_on, right_on, suffixes=['_x', '_y']):
   *     ...
   */
  PL_ASSIGN_OR_RETURN(
      std::shared_ptr<FuncObject> mergefn,
      FuncObject::Create(kMergeOpID, {"right", "how", "left_on", "right_on", "suffixes"},
                         {{"suffixes", "['_x', '_y']"}},
                         /* has_variable_len_args */ false,
                         /* has_variable_len_kwargs */ false,
                         std::bind(&JoinHandler::Eval, graph(), op(), std::placeholders::_1,
                                   std::placeholders::_2, std::placeholders::_3),
                         ast_visitor()));

  PL_RETURN_IF_ERROR(mergefn->SetDocString(kMergeOpDocstring));
  AddMethod(kMergeOpID, mergefn);

  /**
   * # Equivalent to the python method method syntax:
   * def agg(self, **kwargs):
   *     ...
   */
  PL_ASSIGN_OR_RETURN(
      std::shared_ptr<FuncObject> aggfn,
      FuncObject::Create(kBlockingAggOpID, {}, {},
                         /* has_variable_len_args */ false,
                         /* has_variable_len_kwargs */ true,
                         std::bind(&AggHandler::Eval, graph(), op(), std::placeholders::_1,
                                   std::placeholders::_2, std::placeholders::_3),
                         ast_visitor()));
  PL_RETURN_IF_ERROR(aggfn->SetDocString(kBlockingAggOpDocstring));
  AddMethod(kBlockingAggOpID, aggfn);

  /**
   * # Equivalent to the python method method syntax:
   * def agg(self, **kwargs):
   *     ...
   */
  // TODO(philkuz) temporary measure, need to fix the subscript assignment.
  PL_ASSIGN_OR_RETURN(
      std::shared_ptr<FuncObject> mapfn,
      FuncObject::Create(kMapOpID, {"column_name", "expr"}, {},
                         /* has_variable_len_args */ false,
                         /* has_variable_len_kwargs */ false,
                         std::bind(&MapAssignHandler::Eval, graph(), op(), std::placeholders::_1,
                                   std::placeholders::_2, std::placeholders::_3),
                         ast_visitor()));
  PL_RETURN_IF_ERROR(mapfn->SetDocString(kMapOpDocstring));
  AddMethod(kMapOpID, mapfn);
  /**
   * # Equivalent to the python method method syntax:
   * def drop(self, fn):
   *     ...
   */
  PL_ASSIGN_OR_RETURN(
      std::shared_ptr<FuncObject> dropfn,
      FuncObject::Create(kDropOpID, {"columns"}, {}, /* has_variable_len_args */ false,
                         /* has_variable_len_kwargs */ false,
                         std::bind(&DropHandler::Eval, graph(), op(), std::placeholders::_1,
                                   std::placeholders::_2, std::placeholders::_3),
                         ast_visitor()));
  PL_RETURN_IF_ERROR(dropfn->SetDocString(kDropOpDocstring));
  AddMethod(kDropOpID, dropfn);

  /**
   * # Equivalent to the python method method syntax:
   * def head(self, n=5, _pem_only=False):
   *     ...
   */
  PL_ASSIGN_OR_RETURN(
      std::shared_ptr<FuncObject> limitfn,
      FuncObject::Create(kLimitOpID, {"n", "_pem_only"}, {{"n", "5"}, {"_pem_only", "0"}},
                         /* has_variable_len_args */ false,
                         /* has_variable_len_kwargs */ false,
                         std::bind(&LimitHandler::Eval, graph(), op(), std::placeholders::_1,
                                   std::placeholders::_2, std::placeholders::_3),
                         ast_visitor()));
  PL_RETURN_IF_ERROR(limitfn->SetDocString(kLimitOpDocstring));
  AddMethod(kLimitOpID, limitfn);

  /**
   *
   * # Equivalent to the python method method syntax:
   * def __getitem__(self, key):
   *     ...
   *
   * # It's important to note that this is added as a subscript method instead.
   */
  std::shared_ptr<FuncObject> subscript_fn(
      new FuncObject(kSubscriptMethodName, {"key"}, {},
                     /* has_variable_len_args */ false,
                     /* has_variable_len_kwargs */ false,
                     std::bind(&SubscriptHandler::Eval, graph(), op(), std::placeholders::_1,
                               std::placeholders::_2, std::placeholders::_3),
                     ast_visitor()));
  // TODO(philkuz) figure out how to combine these.
  PL_RETURN_IF_ERROR(subscript_fn->SetDocString(kKeepOpDocstring));
  AddSubscriptMethod(subscript_fn);
  // TODO(philkuz) temporary measure to make sure this is recorded.
  std::shared_ptr<FuncObject> filter_fn(
      new FuncObject(kFilterOpID, {"key"}, {},
                     /* has_variable_len_args */ false,
                     /* has_variable_len_kwargs */ false,
                     std::bind(&SubscriptHandler::Eval, graph(), op(), std::placeholders::_1,
                               std::placeholders::_2, std::placeholders::_3),
                     ast_visitor()));
  PL_RETURN_IF_ERROR(filter_fn->SetDocString(kFilterOpDocstring));
  AddMethod(kFilterOpID, filter_fn);

  std::shared_ptr<FuncObject> group_by_fn(
      new FuncObject(kGroupByOpID, {"by"}, {},
                     /* has_variable_len_args */ false,
                     /* has_variable_len_kwargs */ false,
                     std::bind(&GroupByHandler::Eval, graph(), op(), std::placeholders::_1,
                               std::placeholders::_2, std::placeholders::_3),
                     ast_visitor()));
  PL_RETURN_IF_ERROR(group_by_fn->SetDocString(kGroupByOpDocstring));
  AddMethod(kGroupByOpID, group_by_fn);

  /**
   * # Equivalent to the python method method syntax:
   * def append(self, fn):
   *     ...
   */
  PL_ASSIGN_OR_RETURN(
      std::shared_ptr<FuncObject> union_fn,
      FuncObject::Create(kUnionOpID, {"objs"}, {}, /* has_variable_len_args */ false,
                         /* has_variable_len_kwargs */ false,
                         std::bind(&UnionHandler::Eval, graph(), op(), std::placeholders::_1,
                                   std::placeholders::_2, std::placeholders::_3),
                         ast_visitor()));
  PL_RETURN_IF_ERROR(union_fn->SetDocString(kUnionOpDocstring));
  AddMethod(kUnionOpID, union_fn);

  /**
   * # Equivalent to the python method syntax:
   * def rolling(self, window, on="time_"):
   *     ...
   */
  PL_ASSIGN_OR_RETURN(
      std::shared_ptr<FuncObject> rolling_fn,
      FuncObject::Create(kRollingOpID, {"window", "on"}, {{"on", "'time_'"}},
                         /* has_variable_len_args */ false,
                         /* has_variable_len_kwargs */ false,
                         std::bind(&RollingHandler::Eval, graph(), op(), std::placeholders::_1,
                                   std::placeholders::_2, std::placeholders::_3),
                         ast_visitor()));
  PL_RETURN_IF_ERROR(rolling_fn->SetDocString(kRollingOpDocstring));
  AddMethod(kRollingOpID, rolling_fn);

  /**
   * # Equivalent to the python method syntax:
   * def stream(self):
   *     ...
   */
  PL_ASSIGN_OR_RETURN(
      std::shared_ptr<FuncObject> stream_fn,
      FuncObject::Create(kStreamOpId, {}, {},
                         /* has_variable_len_args */ false,
                         /* has_variable_len_kwargs */ false,
                         std::bind(&StreamHandler::Eval, graph(), op(), std::placeholders::_1,
                                   std::placeholders::_2, std::placeholders::_3),
                         ast_visitor()));
  PL_RETURN_IF_ERROR(stream_fn->SetDocString(kStreamOpDocstring));
  AddMethod(kStreamOpId, stream_fn);

  PL_ASSIGN_OR_RETURN(auto md, MetadataObject::Create(op(), ast_visitor()));
  return AssignAttribute(kMetadataAttrName, md);
}

StatusOr<QLObjectPtr> Dataframe::GetAttributeImpl(const pypa::AstPtr& ast,
                                                  std::string_view name) const {
  // If this gets to this point, should fail here.
  DCHECK(HasNonMethodAttribute(name));

  if (QLObject::HasNonMethodAttribute(name)) {
    return QLObject::GetAttributeImpl(ast, name);
  }
  // We evaluate schemas in the analyzer, so at this point assume 'name' is a valid column.
  PL_ASSIGN_OR_RETURN(ColumnIR * column,
                      graph()->CreateNode<ColumnIR>(ast, std::string(name), /* parent_op_idx */ 0));
  return ExprObject::Create(column, ast_visitor());
}

StatusOr<std::shared_ptr<Dataframe>> Dataframe::FromColumnAssignment(const pypa::AstPtr& expr_node,
                                                                     ColumnIR* column,
                                                                     ExpressionIR* expr) {
  auto col_name = column->col_name();
  ColExpressionVector map_exprs{{col_name, expr}};
  PL_ASSIGN_OR_RETURN(MapIR * ir_node, graph_->CreateNode<MapIR>(expr_node, op(), map_exprs,
                                                                 /*keep_input_cols*/ true));
  return Dataframe::Create(ir_node, ast_visitor());
}

StatusOr<QLObjectPtr> JoinHandler::Eval(IR* graph, OperatorIR* op, const pypa::AstPtr& ast,
                                        const ParsedArgs& args, ASTVisitor* visitor) {
  // GetArg returns non-nullptr or errors out in Debug mode. No need
  // to check again.
  PL_ASSIGN_OR_RETURN(OperatorIR * right, GetArgAs<OperatorIR>(ast, args, "right"));
  PL_ASSIGN_OR_RETURN(StringIR * how, GetArgAs<StringIR>(ast, args, "how"));
  QLObjectPtr suffixes_node = args.GetArg("suffixes");
  std::string how_type = how->str();

  PL_ASSIGN_OR_RETURN(std::vector<ColumnIR*> left_on_cols,
                      ProcessCols(graph, ast, args.GetArg("left_on"), "left_on", 0));
  PL_ASSIGN_OR_RETURN(std::vector<ColumnIR*> right_on_cols,
                      ProcessCols(graph, ast, args.GetArg("right_on"), "right_on", 1));

  // TODO(philkuz) consider using a struct instead of a vector because it's a fixed size.
  if (!CollectionObject::IsCollection(suffixes_node)) {
    return suffixes_node->CreateError(
        "'suffixes' must be a list with 2 strings for the left and right suffixes. Received $0",
        suffixes_node->name());
  }

  PL_ASSIGN_OR_RETURN(std::vector<std::string> suffix_strs,
                      ParseAsListOfStrings(suffixes_node, "suffixes"));
  if (suffix_strs.size() != 2) {
    return suffixes_node->CreateError("'suffixes' must be a list with 2 elements. Received $0",
                                      suffix_strs.size());
  }

  PL_ASSIGN_OR_RETURN(JoinIR * join_op,
                      graph->CreateNode<JoinIR>(ast, std::vector<OperatorIR*>{op, right}, how_type,
                                                left_on_cols, right_on_cols, suffix_strs));
  return Dataframe::Create(join_op, visitor);
}

StatusOr<std::vector<ColumnIR*>> JoinHandler::ProcessCols(IR* graph, const pypa::AstPtr& ast,
                                                          QLObjectPtr obj, std::string arg_name,
                                                          int64_t parent_index) {
  DCHECK(obj != nullptr);
  PL_ASSIGN_OR_RETURN(std::vector<std::string> column_names, ParseAsListOfStrings(obj, arg_name));
  std::vector<ColumnIR*> columns;
  columns.reserve(column_names.size());

  for (const auto& col_name : column_names) {
    PL_ASSIGN_OR_RETURN(ColumnIR * col, graph->CreateNode<ColumnIR>(ast, col_name, parent_index));
    columns.push_back(col);
  }
  return columns;
}

StatusOr<QLObjectPtr> AggHandler::Eval(IR* graph, OperatorIR* op, const pypa::AstPtr& ast,
                                       const ParsedArgs& args, ASTVisitor* visitor) {
  // converts the mapping of args.kwargs into ColExpressionvector
  ColExpressionVector aggregate_expressions;
  for (const auto& [name, expr_obj] : args.kwargs()) {
    if (expr_obj->type() != QLObjectType::kTuple) {
      return expr_obj->CreateError("Expected tuple for value at kwarg $0 but received $1", name,
                                   expr_obj->name());
    }
    PL_ASSIGN_OR_RETURN(
        FuncIR * parsed_expr,
        ParseNameTuple(graph, ast, std::static_pointer_cast<TupleObject>(expr_obj)));
    aggregate_expressions.push_back({name, parsed_expr});
  }

  PL_ASSIGN_OR_RETURN(
      BlockingAggIR * agg_op,
      graph->CreateNode<BlockingAggIR>(ast, op, std::vector<ColumnIR*>{}, aggregate_expressions));
  return Dataframe::Create(agg_op, visitor);
}

StatusOr<FuncIR*> AggHandler::ParseNameTuple(IR* ir, const pypa::AstPtr& ast,
                                             std::shared_ptr<TupleObject> tuple) {
  DCHECK_GE(tuple->items().size(), 2UL);
  auto num_args = tuple->items().size() - 1;
  std::vector<StringIR*> arg_names;
  for (auto i = 0UL; i < num_args; i++) {
    auto name_or_s =
        GetArgAs<StringIR>(tuple->items()[i], absl::Substitute("$0-th tuple argument", i + 1));
    if (!name_or_s.ok()) {
      return tuple->items()[i]->CreateError(
          "All elements of the agg tuple must be column names, except the last which should be a "
          "function");
    }
    arg_names.push_back(name_or_s.ConsumeValueOrDie());
  }

  auto func = tuple->items()[num_args];
  if (func->type() != QLObjectType::kFunction) {
    return func->CreateError("Expected second tuple argument to be type Func, received $0",
                             func->name());
  }
  PL_ASSIGN_OR_RETURN(auto called, std::static_pointer_cast<FuncObject>(func)->Call({}, ast));

  PL_ASSIGN_OR_RETURN(FuncIR * func_ir, GetArgAs<FuncIR>(called, "last tuple argument"));

  // The function should be specified as a single function by itself.
  // This could change in the future.
  if (func_ir->args().size() != 0) {
    return func_ir->CreateIRNodeError("Unexpected aggregate function");
  }

  // parent_op_idx is 0 because we only have one parent for an aggregate.
  for (auto name : arg_names) {
    PL_ASSIGN_OR_RETURN(ColumnIR * argcol, ir->CreateNode<ColumnIR>(name->ast(), name->str(),
                                                                    /* parent_op_idx */ 0));
    PL_RETURN_IF_ERROR(func_ir->AddArg(argcol));
  }
  return func_ir;
}

StatusOr<QLObjectPtr> DropHandler::Eval(IR* graph, OperatorIR* op, const pypa::AstPtr& ast,
                                        const ParsedArgs& args, ASTVisitor* visitor) {
  QLObjectPtr columns_arg = args.GetArg("columns");
  PL_ASSIGN_OR_RETURN(std::vector<std::string> columns,
                      ParseAsListOfStrings(args.GetArg("columns"), "columns"));
  PL_ASSIGN_OR_RETURN(DropIR * drop_op, graph->CreateNode<DropIR>(ast, op, columns));
  return Dataframe::Create(drop_op, visitor);
}

StatusOr<QLObjectPtr> LimitHandler::Eval(IR* graph, OperatorIR* op, const pypa::AstPtr& ast,
                                         const ParsedArgs& args, ASTVisitor* visitor) {
  // TODO(philkuz) (PL-1161) Add support for compile time evaluation of Limit argument.
  PL_ASSIGN_OR_RETURN(IntIR * rows_node, GetArgAs<IntIR>(ast, args, "n"));
  PL_ASSIGN_OR_RETURN(IntIR * pem_only, GetArgAs<IntIR>(ast, args, "_pem_only"));
  int64_t limit_value = rows_node->val();
  bool pem_only_val = pem_only->val() > 0;

  PL_ASSIGN_OR_RETURN(LimitIR * limit_op,
                      graph->CreateNode<LimitIR>(ast, op, limit_value, pem_only_val));
  return Dataframe::Create(limit_op, visitor);
}

StatusOr<QLObjectPtr> SubscriptHandler::Eval(IR* graph, OperatorIR* op, const pypa::AstPtr& ast,
                                             const ParsedArgs& args, ASTVisitor* visitor) {
  QLObjectPtr key = args.GetArg("key");
  if (key->HasNode() && Match(key->node(), String())) {
    return EvalColumn(graph, op, ast, static_cast<StringIR*>(key->node()), visitor);
  }
  if (CollectionObject::IsCollection(key)) {
    PL_ASSIGN_OR_RETURN(std::vector<StringIR*> keep_cols, ParseAsListOf<StringIR>(key, "key"));
    return EvalKeep(graph, op, ast, keep_cols, visitor);
  }
  if (key->HasNode() && key->node()->IsExpression()) {
    return EvalFilter(graph, op, ast, static_cast<ExpressionIR*>(key->node()), visitor);
  }
  return key->CreateError(
      "subscript argument must have a list of strings or expression. '$0' not allowed",
      key->name());
}

StatusOr<QLObjectPtr> SubscriptHandler::EvalFilter(IR* graph, OperatorIR* op,
                                                   const pypa::AstPtr& ast, ExpressionIR* expr,
                                                   ASTVisitor* visitor) {
  PL_ASSIGN_OR_RETURN(FilterIR * filter_op, graph->CreateNode<FilterIR>(ast, op, expr));
  return Dataframe::Create(filter_op, visitor);
}

StatusOr<QLObjectPtr> SubscriptHandler::EvalColumn(IR* graph, OperatorIR*, const pypa::AstPtr&,
                                                   StringIR* expr, ASTVisitor* visitor) {
  PL_ASSIGN_OR_RETURN(ColumnIR * column, graph->CreateNode<ColumnIR>(expr->ast(), expr->str(),
                                                                     /* parent_op_idx */ 0));
  return ExprObject::Create(column, visitor);
}

StatusOr<QLObjectPtr> SubscriptHandler::EvalKeep(IR* graph, OperatorIR* op, const pypa::AstPtr& ast,
                                                 std::vector<StringIR*> keep_cols,
                                                 ASTVisitor* visitor) {
  absl::flat_hash_set<std::string> keep_cols_seen;
  ColExpressionVector keep_exprs;
  for (const auto& keep_col : keep_cols) {
    auto col_name = keep_col->str();
    if (keep_cols_seen.contains(col_name)) {
      return keep_col->CreateIRNodeError(
          "cannot specify the same column name more than once when filtering by cols. '$0' "
          "specified more than once",
          col_name);
    }
    keep_cols_seen.insert(col_name);

    // parent_op_idx is 0 because we only have one parent for a map.
    PL_ASSIGN_OR_RETURN(ColumnIR * column,
                        graph->CreateNode<ColumnIR>(ast, col_name, /* parent_op_idx */ 0));
    keep_exprs.emplace_back(col_name, column);
  }

  PL_ASSIGN_OR_RETURN(MapIR * map_op, graph->CreateNode<MapIR>(ast, op, keep_exprs,
                                                               /* keep_input_columns */ false));
  return Dataframe::Create(map_op, visitor);
}

StatusOr<QLObjectPtr> GroupByHandler::Eval(IR* graph, OperatorIR* op, const pypa::AstPtr& ast,
                                           const ParsedArgs& args, ASTVisitor* visitor) {
  PL_ASSIGN_OR_RETURN(std::vector<std::string> group_names,
                      ParseAsListOfStrings(args.GetArg("by"), "by"));
  std::vector<ColumnIR*> groups;
  groups.reserve(group_names.size());
  for (const auto& group : group_names) {
    PL_ASSIGN_OR_RETURN(ColumnIR * col,
                        graph->CreateNode<ColumnIR>(ast, group, /* parent_idx */ 0));
    groups.push_back(col);
  }

  PL_ASSIGN_OR_RETURN(GroupByIR * group_by_op, graph->CreateNode<GroupByIR>(ast, op, groups));
  return Dataframe::Create(group_by_op, visitor);
}

StatusOr<QLObjectPtr> UnionHandler::Eval(IR* graph, OperatorIR* op, const pypa::AstPtr& ast,
                                         const ParsedArgs& args, ASTVisitor* visitor) {
  std::vector<OperatorIR*> parents{op};
  PL_ASSIGN_OR_RETURN(std::vector<OperatorIR*> list_of_ops,
                      ParseAsListOf<OperatorIR>(args.GetArg("objs"), "objs"));
  parents.insert(parents.end(), list_of_ops.begin(), list_of_ops.end());
  PL_ASSIGN_OR_RETURN(UnionIR * union_op, graph->CreateNode<UnionIR>(ast, parents));
  return Dataframe::Create(union_op, visitor);
}

StatusOr<QLObjectPtr> RollingHandler::Eval(IR* graph, OperatorIR* op, const pypa::AstPtr& ast,
                                           const ParsedArgs& args, ASTVisitor* visitor) {
  PL_ASSIGN_OR_RETURN(StringIR * window_col_name, GetArgAs<StringIR>(ast, args, "on"));
  PL_ASSIGN_OR_RETURN(ExpressionIR * window_size, GetArgAs<ExpressionIR>(ast, args, "window"));

  if (window_col_name->str() != "time_") {
    return window_col_name->CreateIRNodeError(
        "Windowing is only supported on time_ at the moment, not $0", window_col_name->str());
  }

  PL_ASSIGN_OR_RETURN(ColumnIR * window_col,
                      graph->CreateNode<ColumnIR>(ast, window_col_name->str(), /* parent_idx */ 0));

  PL_ASSIGN_OR_RETURN(RollingIR * rolling_op,
                      graph->CreateNode<RollingIR>(ast, op, window_col, window_size));
  return Dataframe::Create(rolling_op, visitor);
}

StatusOr<QLObjectPtr> StreamHandler::Eval(IR* graph, OperatorIR* op, const pypa::AstPtr& ast,
                                          const ParsedArgs&, ASTVisitor* visitor) {
  PL_ASSIGN_OR_RETURN(StreamIR * stream_op, graph->CreateNode<StreamIR>(ast, op));
  return Dataframe::Create(stream_op, visitor);
}

StatusOr<QLObjectPtr> DataFrameHandler::Eval(IR* graph, const pypa::AstPtr& ast,
                                             const ParsedArgs& args, ASTVisitor* visitor) {
  PL_ASSIGN_OR_RETURN(StringIR * table, GetArgAs<StringIR>(ast, args, "table"));
  PL_ASSIGN_OR_RETURN(std::vector<std::string> columns,
                      ParseAsListOfStrings(args.GetArg("select"), "select"));
  PL_ASSIGN_OR_RETURN(ExpressionIR * start_time, GetArgAs<ExpressionIR>(ast, args, "start_time"));
  PL_ASSIGN_OR_RETURN(ExpressionIR * end_time, GetArgAs<ExpressionIR>(ast, args, "end_time"));

  std::string table_name = table->str();
  PL_ASSIGN_OR_RETURN(MemorySourceIR * mem_source_op,
                      graph->CreateNode<MemorySourceIR>(ast, table_name, columns));
  // If both start_time and end_time are default arguments, then we don't substitute them.
  if (!(args.default_subbed_args().contains("start_time") &&
        args.default_subbed_args().contains("end_time"))) {
    PL_RETURN_IF_ERROR(mem_source_op->SetTimeExpressions(start_time, end_time));
  }
  return Dataframe::Create(mem_source_op, visitor);
}

}  // namespace compiler
}  // namespace planner
}  // namespace carnot
}  // namespace px
