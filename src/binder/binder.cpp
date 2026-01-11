#include "dreamdb/binder/binder.h"

#include <stdexcept>
#include <unordered_set>

#include "dreamdb/binder/bound/bound_insert_statement.h"
#include "dreamdb/binder/bound/bound_delete_statement.h"
#include "dreamdb/binder/bound/bound_update_statement.h"
#include "dreamdb/binder/bound/bound_select_statement.h"
#include "dreamdb/parser/ast/ast_literal_expression_node.h"
#include "dreamdb/parser/ast/ast_function_call_expression_node.h"
#include "dreamdb/parser/ast/ast_vector_expression_node.h"
#include "dreamdb/parser/ast/ast_column_reference_expression_node.h"
#include "dreamdb/parser/ast/ast_select_statement_node.h"
#include "dreamdb/parser/ast/ast_binary_expression_node.h"
#include "dreamdb/parser/ast/ast_unary_expression_node.h"
#include "dreamdb/parser/ast/ast_in_expression_node.h"
#include "dreamdb/parser/ast/ast_between_expression_node.h"
#include "dreamdb/parser/ast/ast_like_expression_node.h"
#include "dreamdb/expression/constant_expression.h"
#include "dreamdb/expression/function_expression.h"
#include "dreamdb/expression/column_reference_expression.h"
#include "dreamdb/expression/binary_expression.h"
#include "dreamdb/expression/unary_expression.h"
#include "dreamdb/expression/in_expression.h"
#include "dreamdb/expression/between_expression.h"
#include "dreamdb/expression/like_expression.h"
#include "dreamdb/catalog/catalog_collection_entry.h"
#include "dreamdb/common/null.h"

namespace dreamdb
{

// 辅助函数：将 AST 二元操作符类型转换为 Bound 二元操作符类型
static BinaryOperatorType convert_binary_operator(AstBinaryOperatorType ast_op)
{
    switch (ast_op)
    {
        case AstBinaryOperatorType::AST_BINARY_OPERATOR_PLUS:
            return BinaryOperatorType::EXPRESSION_PLUS;
        case AstBinaryOperatorType::AST_BINARY_OPERATOR_MINUS:
            return BinaryOperatorType::EXPRESSION_MINUS;
        case AstBinaryOperatorType::AST_BINARY_OPERATOR_MULTIPLY:
            return BinaryOperatorType::EXPRESSION_MULTIPLY;
        case AstBinaryOperatorType::AST_BINARY_OPERATOR_DIVIDE:
            return BinaryOperatorType::EXPRESSION_DIVIDE;
        case AstBinaryOperatorType::AST_BINARY_OPERATOR_MODULO:
            return BinaryOperatorType::EXPRESSION_MODULO;
        case AstBinaryOperatorType::AST_BINARY_OPERATOR_EQUAL:
            return BinaryOperatorType::EXPRESSION_EQUAL;
        case AstBinaryOperatorType::AST_BINARY_OPERATOR_NOT_EQUAL:
            return BinaryOperatorType::EXPRESSION_NOT_EQUAL;
        case AstBinaryOperatorType::AST_BINARY_OPERATOR_GREATER_THAN:
            return BinaryOperatorType::EXPRESSION_GREATER_THAN;
        case AstBinaryOperatorType::AST_BINARY_OPERATOR_GREATER_EQUAL:
            return BinaryOperatorType::EXPRESSION_GREATER_EQUAL;
        case AstBinaryOperatorType::AST_BINARY_OPERATOR_LESS_THAN:
            return BinaryOperatorType::EXPRESSION_LESS_THAN;
        case AstBinaryOperatorType::AST_BINARY_OPERATOR_LESS_EQUAL:
            return BinaryOperatorType::EXPRESSION_LESS_EQUAL;
        case AstBinaryOperatorType::AST_BINARY_OPERATOR_AND:
            return BinaryOperatorType::EXPRESSION_AND;
        case AstBinaryOperatorType::AST_BINARY_OPERATOR_OR:
            return BinaryOperatorType::EXPRESSION_OR;
        case AstBinaryOperatorType::AST_BINARY_OPERATOR_UNKNOWN:
        default:
            throw std::runtime_error("Unknown binary operator type");
    }
}

// 辅助函数：将 AST 一元操作符类型转换为 Bound 一元操作符类型
static UnaryOperatorType convert_unary_operator(AstUnaryOperatorType ast_op)
{
    switch (ast_op)
    {
        case AstUnaryOperatorType::AST_UNARY_OPERATOR_NOT:
            return UnaryOperatorType::EXPRESSION_NOT;
        case AstUnaryOperatorType::AST_UNARY_OPERATOR_MINUS:
            return UnaryOperatorType::EXPRESSION_MINUS;
        case AstUnaryOperatorType::AST_UNARY_OPERATOR_PLUS:
            return UnaryOperatorType::EXPRESSION_PLUS;
        case AstUnaryOperatorType::AST_UNARY_OPERATOR_UNKNOWN:
        default:
            throw std::runtime_error("Unknown unary operator type");
    }
}

Binder::Binder(std::unique_ptr<Catalog> catalog, const std::string & current_database)
    : catalog_(std::move(catalog))
    , current_database_(current_database)
{
}

std::unique_ptr<BoundStatement> Binder::bind(const AstStatementNode & statement)
{
    switch (statement.get_statement_type())
    {
        case AstStatementNodeType::AST_STATEMENT_SELECT:
            return bind_select_statement(static_cast<const AstSelectStatementNode &>(statement));
        case AstStatementNodeType::AST_STATEMENT_INSERT:
            return bind_insert_statement(static_cast<const AstInsertStatementNode &>(statement));
        case AstStatementNodeType::AST_STATEMENT_DELETE:
            return bind_delete_statement(static_cast<const AstDeleteStatementNode &>(statement));
        case AstStatementNodeType::AST_STATEMENT_UPDATE:
            return bind_update_statement(static_cast<const AstUpdateStatementNode &>(statement));
        default:
            throw std::runtime_error("Unsupported statement type");
    }
}

std::unique_ptr<BoundStatement> Binder::bind_select_statement(const AstSelectStatementNode & select_statement)
{
    // 创建绑定后的查询语句
    auto bound_select_statement = std::make_unique<BoundSelectStatement>();

    if (!select_statement.has_collection_name()) {
        throw std::runtime_error("Collection name is required for select statement");
    }

    const std::string & collection_name = select_statement.get_collection_name();

    // 检查集合合法性
    const auto * collection_entry = catalog_->get_collection_entry(current_database_, collection_name);
    if (!collection_entry) {
        throw std::runtime_error("Collection not found: " + collection_name);
    }

    // 设置集合 ID
    bound_select_statement->collection_id = collection_entry->collection_id_;

    // 绑定 SELECT 列表
    if (!select_statement.has_select_items()) {
        throw std::runtime_error("SELECT list is required for select statement");
    }

    const auto & select_items = select_statement.get_select_items();
    for (const auto & select_item : select_items) {
        if (select_item.get_select_item_type() == SelectItemType::SELECT_ITEM_STAR) {
            // SELECT *：展开所有列
            for (std::size_t i = 0; i < collection_entry->column_entries_.size(); ++i) {
                const auto * column_entry = collection_entry->get_column_entry(i);
                if (column_entry) {
                    BoundSelectItem bound_item;
                    bound_item.expr = std::make_unique<ColumnReferenceExpression>(column_entry->column_index());
                    bound_item.alias = column_entry->name();  // 使用列名作为别名
                    bound_select_statement->select_items.push_back(std::move(bound_item));
                }
            }
        } else {
            // SELECT 表达式项
            if (!select_item.has_select_item_expression()) {
                throw std::runtime_error("Expression is required for SELECT item");
            }

            BoundSelectItem bound_item;
            bound_item.expr = bind_expression(select_item.get_select_item_expression(), collection_entry);
            
            // 设置别名
            if (select_item.has_select_item_alias()) {
                bound_item.alias = select_item.get_select_item_alias();
            }

            bound_select_statement->select_items.push_back(std::move(bound_item));
        }
    }

    // 绑定 WHERE 子句（如果存在）
    if (select_statement.has_where_clause()) {
        bound_select_statement->where = bind_expression(select_statement.get_where_clause(), collection_entry);
    }

    // 绑定 GROUP BY 子句（如果存在）
    if (select_statement.has_group_by_clauses()) {
        const auto & group_by_clauses = select_statement.get_group_by_clauses();
        bound_select_statement->group_by.reserve(group_by_clauses.size());
        for (const auto & group_by_expr : group_by_clauses) {
            bound_select_statement->group_by.push_back(bind_expression(*group_by_expr, collection_entry));
        }
    }

    // 绑定 HAVING 子句（如果存在）
    if (select_statement.has_having_clause()) {
        bound_select_statement->having = bind_expression(select_statement.get_having_clause(), collection_entry);
    }

    // 绑定 ORDER BY 项（如果存在）
    if (select_statement.has_order_by_items()) {
        const auto & order_by_items = select_statement.get_order_by_items();
        bound_select_statement->order_by.reserve(order_by_items.size());
        for (const auto & order_by_item : order_by_items) {
            if (!order_by_item.has_expression()) {
                throw std::runtime_error("Expression is required for ORDER BY item");
            }

            BoundOrderByItem bound_item;
            bound_item.expr = bind_expression(order_by_item.get_expression(), collection_entry);
            bound_item.order = order_by_item.get_order_type();
            bound_select_statement->order_by.push_back(std::move(bound_item));
        }
    }

    // 绑定 LIMIT 子句（如果存在）
    if (select_statement.has_limit()) {
        auto limit_expr = bind_expression(select_statement.get_limit());
        
        // LIMIT 必须是常量整数表达式
        if (limit_expr->get_type() != ExpressionType::EXPRESSION_CONSTANT) {
            throw std::runtime_error("LIMIT must be a constant expression");
        }

        const auto * constant_expr = static_cast<const ConstantExpression *>(limit_expr.get());
        
        // 提取整数值
        std::size_t limit_value = 0;
        if (constant_expr->is_type<std::int64_t>()) {
            limit_value = static_cast<std::size_t>(constant_expr->get_field_value_as<std::int64_t>());
        } else if (constant_expr->is_type<std::int32_t>()) {
            limit_value = static_cast<std::size_t>(constant_expr->get_field_value_as<std::int32_t>());
        } else if (constant_expr->is_type<std::int16_t>()) {
            limit_value = static_cast<std::size_t>(constant_expr->get_field_value_as<std::int16_t>());
        } else if (constant_expr->is_type<std::int8_t>()) {
            limit_value = static_cast<std::size_t>(constant_expr->get_field_value_as<std::int8_t>());
        } else {
            throw std::runtime_error("LIMIT must be an integer constant");
        }

        bound_select_statement->limit = limit_value;
    }

    // 绑定 OFFSET 子句（如果存在）
    if (select_statement.has_offset()) {
        auto offset_expr = bind_expression(select_statement.get_offset());
        
        // OFFSET 必须是常量整数表达式
        if (offset_expr->get_type() != ExpressionType::EXPRESSION_CONSTANT) {
            throw std::runtime_error("OFFSET must be a constant expression");
        }

        const auto * constant_expr = static_cast<const ConstantExpression *>(offset_expr.get());
        
        // 提取整数值
        std::size_t offset_value = 0;
        if (constant_expr->is_type<std::int64_t>()) {
            offset_value = static_cast<std::size_t>(constant_expr->get_field_value_as<std::int64_t>());
        } else if (constant_expr->is_type<std::int32_t>()) {
            offset_value = static_cast<std::size_t>(constant_expr->get_field_value_as<std::int32_t>());
        } else if (constant_expr->is_type<std::int16_t>()) {
            offset_value = static_cast<std::size_t>(constant_expr->get_field_value_as<std::int16_t>());
        } else if (constant_expr->is_type<std::int8_t>()) {
            offset_value = static_cast<std::size_t>(constant_expr->get_field_value_as<std::int8_t>());
        } else {
            throw std::runtime_error("OFFSET must be an integer constant");
        }

        bound_select_statement->offset = offset_value;
    }

    return bound_select_statement;
}

std::unique_ptr<Expression> Binder::bind_expression(const AstExpressionNode & expression)
{
    switch (expression.get_expression_type())
    {
        case AstExpressionNodeType::AST_EXPRESSION_LITERAL:
        {
            const auto & literal_expr = static_cast<const AstLiteralExpressionNode &>(expression);
            FieldValue field_value;

            if (literal_expr.is_integer()) {
                field_value = static_cast<std::int64_t>(literal_expr.get_integer());
            } else if (literal_expr.is_float()) {
                field_value = literal_expr.get_float();
            } else if (literal_expr.is_string()) {
                field_value = literal_expr.get_string();
            } else if (literal_expr.is_boolean()) {
                field_value = literal_expr.get_boolean();
            } else if (literal_expr.is_null()) {
                field_value = Null();
            } else {
                throw std::runtime_error("Unsupported literal type");
            }

            return std::make_unique<ConstantExpression>(field_value);
        }
        case AstExpressionNodeType::AST_EXPRESSION_FUNCTION_CALL:
        {
            const auto & func_expr = static_cast<const AstFunctionCallExpressionNode &>(expression);
            
            if (!func_expr.has_function_name()) {
                throw std::runtime_error("Function name is required for function call expression");
            }

            const std::string & function_name = func_expr.get_function_name();
            std::vector<std::unique_ptr<Expression>> arguments;

            // 绑定所有参数
            if (func_expr.has_arguments()) {
                const auto & ast_arguments = func_expr.get_arguments();
                arguments.reserve(ast_arguments.size());
                for (const auto & ast_arg : ast_arguments) {
                    arguments.push_back(bind_expression(*ast_arg));
                }
            }

            return std::make_unique<FunctionExpression>(function_name, std::move(arguments));
        }
        case AstExpressionNodeType::AST_EXPRESSION_VECTOR:
        {
            const auto & vector_expr = static_cast<const AstVectorExpressionNode &>(expression);
            
            if (!vector_expr.has_elements()) {
                // 空向量
                return std::make_unique<ConstantExpression>(FieldValue(std::vector<float>()));
            }

            // 尝试将所有元素绑定为常量表达式
            // 如果所有元素都是字面量，则创建一个向量常量表达式
            std::vector<float> vector_value;
            vector_value.reserve(vector_expr.get_size());

            const auto & elements = vector_expr.get_elements();
            for (const auto & element : elements) {
                // 只支持字面量元素，其他类型需要在后续阶段处理
                if (element->get_expression_type() != AstExpressionNodeType::AST_EXPRESSION_LITERAL) {
                    throw std::runtime_error("Vector expression with non-literal elements is not supported");
                }

                const auto & literal_expr = static_cast<const AstLiteralExpressionNode &>(*element);
                if (literal_expr.is_float()) {
                    vector_value.push_back(static_cast<float>(literal_expr.get_float()));
                } else if (literal_expr.is_integer()) {
                    vector_value.push_back(static_cast<float>(literal_expr.get_integer()));
                } else {
                    throw std::runtime_error("Vector elements must be numeric literals");
                }
            }

            return std::make_unique<ConstantExpression>(FieldValue(std::move(vector_value)));
        }
        default:
            throw std::runtime_error("Unsupported expression type in bind_expression: " + 
                                   std::to_string(static_cast<int>(expression.get_expression_type())));
    }
}

std::unique_ptr<BoundStatement> Binder::bind_insert_statement(const AstInsertStatementNode & insert_statement)
{
    // 创建绑定后的插入语句
    auto bound_insert_statement = std::make_unique<BoundInsertStatement>();

    if (!insert_statement.has_collection_name()) {
        throw std::runtime_error("Collection name is required for insert statement");
    }

    const std::string & collection_name = insert_statement.get_collection_name();

    // 检查集合合法性
    const auto * collection_entry = catalog_->get_collection_entry(current_database_, collection_name);
    if (!collection_entry) {
        throw std::runtime_error("Collection not found: " + collection_name);
    }

    // 设置集合 ID
    bound_insert_statement->collection_id = collection_entry->collection_id_;

    if (!insert_statement.has_values()) {
        throw std::runtime_error("Values are required for insert statement");
    }

    const std::vector<std::unique_ptr<AstExpressionNode>> & values = insert_statement.get_values();

    if (!insert_statement.has_column_names()) {
        // 没有指定列名，则必须按照表结构顺序插入
        // 检查值的数量是否与表的列数一致
        if (values.size() != collection_entry->column_entries_.size()) {
            throw std::runtime_error("Number of values does not match number of columns");
        }

        // 按照列的顺序绑定值
        for (std::size_t i = 0; i < values.size(); ++i) {
            BoundInsertItem item;
            item.column_index = i;
            item.value = bind_expression(*values[i]);
            bound_insert_statement->insert_items.push_back(std::move(item));
        }
    } else {
        // 指定了列名，则必须按照指定的列名顺序插入
        const std::vector<std::string> & column_names = insert_statement.get_column_names();

        // 检查列名和值的维度是否一致
        if (column_names.size() != values.size()) {
            throw std::runtime_error("Column names and values must have the same dimension");
        }

        // 检查是否有重复的列名
        std::unordered_set<std::string> seen_columns;
        for (const auto & column_name : column_names) {
            if (seen_columns.find(column_name) != seen_columns.end()) {
                throw std::runtime_error("Duplicate column name in INSERT statement: " + column_name);
            }
            seen_columns.insert(column_name);
        }

        // 验证列名是否存在，并获取列索引
        for (std::size_t i = 0; i < column_names.size(); ++i) {
            const std::string & column_name = column_names[i];
            const auto * column_entry = collection_entry->get_column_entry(column_name);
            if (!column_entry) {
                throw std::runtime_error("Column not found: " + column_name);
            }

            BoundInsertItem item;
            item.column_index = column_entry->column_index();
            item.value = bind_expression(*values[i]);
            bound_insert_statement->insert_items.push_back(std::move(item));
        }
    }

    return bound_insert_statement;
}

std::unique_ptr<Expression> Binder::bind_expression(const AstExpressionNode & expression, const CatalogCollectionEntry * collection_entry)
{
    switch (expression.get_expression_type())
    {
        case AstExpressionNodeType::AST_EXPRESSION_COLUMN_REFERENCE:
        {
            const auto & column_expr = static_cast<const AstColumnReferenceExpressionNode &>(expression);
            
            if (!column_expr.has_column_name()) {
                throw std::runtime_error("Column name is required for column reference expression");
            }

            if (!collection_entry) {
                throw std::runtime_error("Collection entry is required for column reference binding");
            }

            const std::string & column_name = column_expr.get_column_name();

            // 如果指定了数据库名，验证是否匹配当前数据库
            if (column_expr.has_database_name()) {
                const std::string & database_name = column_expr.get_database_name();
                if (database_name != current_database_) {
                    throw std::runtime_error("Database name mismatch in column reference: " + database_name);
                }
            }

            // 如果指定了集合名，验证是否匹配当前集合
            if (column_expr.has_collection_name()) {
                const std::string & ref_collection_name = column_expr.get_collection_name();
                if (ref_collection_name != collection_entry->collection_name_) {
                    throw std::runtime_error("Collection name mismatch in column reference: " + ref_collection_name);
                }
            }

            // 查找列条目
            const auto * column_entry = collection_entry->get_column_entry(column_name);
            if (!column_entry) {
                throw std::runtime_error("Column not found: " + column_name);
            }

            return std::make_unique<ColumnReferenceExpression>(column_entry->column_index());
        }
        case AstExpressionNodeType::AST_EXPRESSION_BINARY:
        {
            const auto & binary_expr = static_cast<const AstBinaryExpressionNode &>(expression);
            
            if (!binary_expr.has_operator_type()) {
                throw std::runtime_error("Operator type is required for binary expression");
            }
            if (!binary_expr.has_left()) {
                throw std::runtime_error("Left operand is required for binary expression");
            }
            if (!binary_expr.has_right()) {
                throw std::runtime_error("Right operand is required for binary expression");
            }

            BinaryOperatorType operator_type = convert_binary_operator(binary_expr.get_operator_type());
            auto left = bind_expression(binary_expr.get_left(), collection_entry);
            auto right = bind_expression(binary_expr.get_right(), collection_entry);

            return std::make_unique<BinaryExpression>(operator_type, std::move(left), std::move(right));
        }
        case AstExpressionNodeType::AST_EXPRESSION_UNARY:
        {
            const auto & unary_expr = static_cast<const AstUnaryExpressionNode &>(expression);
            
            if (!unary_expr.has_operator_type()) {
                throw std::runtime_error("Operator type is required for unary expression");
            }
            if (!unary_expr.has_operand()) {
                throw std::runtime_error("Operand is required for unary expression");
            }

            UnaryOperatorType operator_type = convert_unary_operator(unary_expr.get_operator_type());
            auto operand = bind_expression(unary_expr.get_operand(), collection_entry);

            return std::make_unique<UnaryExpression>(operator_type, std::move(operand));
        }
        case AstExpressionNodeType::AST_EXPRESSION_IN:
        {
            const auto & in_expr = static_cast<const AstInExpressionNode &>(expression);
            
            if (!in_expr.has_left()) {
                throw std::runtime_error("Left expression is required for IN expression");
            }
            if (!in_expr.has_values()) {
                throw std::runtime_error("Value list is required for IN expression");
            }

            auto value = bind_expression(in_expr.get_left(), collection_entry);
            std::vector<std::unique_ptr<Expression>> list;
            
            const auto & ast_values = in_expr.get_values();
            list.reserve(ast_values.size());
            for (const auto & ast_value : ast_values) {
                list.push_back(bind_expression(*ast_value, collection_entry));
            }

            return std::make_unique<InExpression>(std::move(value), std::move(list), in_expr.is_not());
        }
        case AstExpressionNodeType::AST_EXPRESSION_BETWEEN:
        {
            const auto & between_expr = static_cast<const AstBetweenExpressionNode &>(expression);
            
            if (!between_expr.has_left()) {
                throw std::runtime_error("Left expression is required for BETWEEN expression");
            }
            if (!between_expr.has_start()) {
                throw std::runtime_error("Start expression is required for BETWEEN expression");
            }
            if (!between_expr.has_end()) {
                throw std::runtime_error("End expression is required for BETWEEN expression");
            }

            auto value = bind_expression(between_expr.get_left(), collection_entry);
            auto min = bind_expression(between_expr.get_start(), collection_entry);
            auto max = bind_expression(between_expr.get_end(), collection_entry);

            return std::make_unique<BetweenExpression>(std::move(value), std::move(min), std::move(max), between_expr.is_not());
        }
        case AstExpressionNodeType::AST_EXPRESSION_LIKE:
        {
            const auto & like_expr = static_cast<const AstLikeExpressionNode &>(expression);
            
            if (!like_expr.has_left()) {
                throw std::runtime_error("Left expression is required for LIKE expression");
            }
            if (!like_expr.has_pattern()) {
                throw std::runtime_error("Pattern expression is required for LIKE expression");
            }

            auto value = bind_expression(like_expr.get_left(), collection_entry);
            auto pattern = bind_expression(like_expr.get_pattern(), collection_entry);

            return std::make_unique<LikeExpression>(std::move(value), std::move(pattern), like_expr.is_not());
        }
        case AstExpressionNodeType::AST_EXPRESSION_LITERAL:
        case AstExpressionNodeType::AST_EXPRESSION_VECTOR:
            // 这些表达式类型不需要集合上下文，使用无上下文的绑定方法
            return bind_expression(expression);
        case AstExpressionNodeType::AST_EXPRESSION_FUNCTION_CALL:
        {
            // 函数调用需要集合上下文，因为参数可能包含列引用
            const auto & func_expr = static_cast<const AstFunctionCallExpressionNode &>(expression);
            
            if (!func_expr.has_function_name()) {
                throw std::runtime_error("Function name is required for function call expression");
            }

            const std::string & function_name = func_expr.get_function_name();
            std::vector<std::unique_ptr<Expression>> arguments;

            // 使用集合上下文绑定所有参数
            if (func_expr.has_arguments()) {
                const auto & ast_arguments = func_expr.get_arguments();
                arguments.reserve(ast_arguments.size());
                for (const auto & ast_arg : ast_arguments) {
                    arguments.push_back(bind_expression(*ast_arg, collection_entry));
                }
            }

            return std::make_unique<FunctionExpression>(function_name, std::move(arguments));
        }
        default:
            throw std::runtime_error("Unsupported expression type in bind_expression with collection context: " + 
                                   std::to_string(static_cast<int>(expression.get_expression_type())));
    }
}

std::unique_ptr<BoundStatement> Binder::bind_delete_statement(const AstDeleteStatementNode & delete_statement)
{
    // 创建绑定后的删除语句
    auto bound_delete_statement = std::make_unique<BoundDeleteStatement>();

    if (!delete_statement.has_collection_name()) {
        throw std::runtime_error("Collection name is required for delete statement");
    }

    const std::string & collection_name = delete_statement.get_collection_name();

    // 检查集合合法性
    const auto * collection_entry = catalog_->get_collection_entry(current_database_, collection_name);
    if (!collection_entry) {
        throw std::runtime_error("Collection not found: " + collection_name);
    }

    // 设置集合 ID
    bound_delete_statement->collection_id = collection_entry->collection_id_;

    // 绑定 WHERE 子句（如果存在）
    if (delete_statement.has_where_clause()) {
        bound_delete_statement->where = bind_expression(delete_statement.get_where_clause(), collection_entry);
    }

    return bound_delete_statement;
}

std::unique_ptr<BoundStatement> Binder::bind_update_statement(const AstUpdateStatementNode & update_statement)
{
    // 创建绑定后的更新语句
    auto bound_update_statement = std::make_unique<BoundUpdateStatement>();

    if (!update_statement.has_collection_name()) {
        throw std::runtime_error("Collection name is required for update statement");
    }

    const std::string & collection_name = update_statement.get_collection_name();

    // 检查集合合法性
    const auto * collection_entry = catalog_->get_collection_entry(current_database_, collection_name);
    if (!collection_entry) {
        throw std::runtime_error("Collection not found: " + collection_name);
    }

    // 设置集合 ID
    bound_update_statement->collection_id = collection_entry->collection_id_;

    // 绑定 SET 子句
    if (!update_statement.has_assignments()) {
        throw std::runtime_error("Assignments are required for update statement");
    }

    const auto & assignments = update_statement.get_assignments();
    if (assignments.empty()) {
        throw std::runtime_error("At least one assignment is required for update statement");
    }

    // 检查是否有重复的列名
    std::unordered_set<std::string> seen_columns;
    for (const auto & assignment : assignments) {
        if (!assignment.has_column_name()) {
            throw std::runtime_error("Column name is required in UPDATE assignment");
        }

        const std::string & column_name = assignment.get_column_name();
        if (seen_columns.find(column_name) != seen_columns.end()) {
            throw std::runtime_error("Duplicate column name in UPDATE statement: " + column_name);
        }
        seen_columns.insert(column_name);
    }

    // 绑定每个赋值项
    for (const auto & assignment : assignments) {
        const std::string & column_name = assignment.get_column_name();

        // 验证列名是否存在
        const auto * column_entry = collection_entry->get_column_entry(column_name);
        if (!column_entry) {
            throw std::runtime_error("Column not found: " + column_name);
        }

        BoundUpdateItem item;
        // 创建列引用表达式
        item.column_reference = std::make_unique<ColumnReferenceExpression>(column_entry->column_index());
        // 绑定值表达式（使用集合上下文，因为值表达式可能包含列引用）
        item.value = bind_expression(assignment.get_value(), collection_entry);
        bound_update_statement->update_items.push_back(std::move(item));
    }

    // 绑定 WHERE 子句（如果存在）
    if (update_statement.has_where_clause()) {
        bound_update_statement->where = bind_expression(update_statement.get_where_clause(), collection_entry);
    }

    return bound_update_statement;
}

} // namespace dreamdb
