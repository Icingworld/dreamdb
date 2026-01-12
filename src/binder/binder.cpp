#include "dreamdb/binder/binder.h"

#include <stdexcept>
#include <unordered_set>

#include "dreamdb/binder/bound/bound_insert_statement.h"
#include "dreamdb/binder/bound/bound_delete_statement.h"
#include "dreamdb/binder/bound/bound_update_statement.h"
#include "dreamdb/binder/bound/bound_select_statement.h"
#include "dreamdb/binder/bound/bound_use_statement.h"
#include "dreamdb/binder/bound/bound_show_statement.h"
#include "dreamdb/binder/bound/bound_describe_statement.h"
#include "dreamdb/binder/bound/bound_drop_statement.h"
#include "dreamdb/binder/bound/bound_create_statement.h"
#include "dreamdb/binder/bound/bound_alter_statement.h"
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
#include "dreamdb/parser/ast/ast_use_statement_node.h"
#include "dreamdb/parser/ast/ast_show_statement_node.h"
#include "dreamdb/parser/ast/ast_describe_statement_node.h"
#include "dreamdb/parser/ast/ast_drop_statement_node.h"
#include "dreamdb/parser/ast/ast_create_statement_node.h"
#include "dreamdb/parser/ast/ast_alter_statement_node.h"
#include "dreamdb/expression/constant_expression.h"
#include "dreamdb/expression/function_expression.h"
#include "dreamdb/expression/column_reference_expression.h"
#include "dreamdb/expression/binary_expression.h"
#include "dreamdb/expression/unary_expression.h"
#include "dreamdb/expression/in_expression.h"
#include "dreamdb/expression/between_expression.h"
#include "dreamdb/expression/like_expression.h"
#include "dreamdb/catalog/catalog_collection_entry.h"
#include "dreamdb/catalog/catalog_database_entry.h"
#include "dreamdb/common/null.h"
#include "dreamdb/schema/field.h"

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
        case AstStatementNodeType::AST_STATEMENT_USE:
            return bind_use_statement(static_cast<const AstUseStatementNode &>(statement));
        case AstStatementNodeType::AST_STATEMENT_SHOW:
            return bind_show_statement(static_cast<const AstShowStatementNode &>(statement));
        case AstStatementNodeType::AST_STATEMENT_DESCRIBE:
            return bind_describe_statement(static_cast<const AstDescribeStatementNode &>(statement));
        case AstStatementNodeType::AST_STATEMENT_DROP:
            return bind_drop_statement(static_cast<const AstDropStatementNode &>(statement));
        case AstStatementNodeType::AST_STATEMENT_CREATE:
            return bind_create_statement(static_cast<const AstCreateStatementNode &>(statement));
        case AstStatementNodeType::AST_STATEMENT_ALTER:
            return bind_alter_statement(static_cast<const AstAlterStatementNode &>(statement));
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

std::unique_ptr<BoundStatement> Binder::bind_use_statement(const AstUseStatementNode & use_statement)
{
    // 创建绑定后的 USE 语句
    auto bound_use_statement = std::make_unique<BoundUseStatement>();

    if (!use_statement.has_database_name()) {
        throw std::runtime_error("Database name is required for USE statement");
    }

    const std::string & database_name = use_statement.get_database_name();

    // 检查数据库是否存在并获取数据库条目
    const auto * database_entry = catalog_->get_database_entry(database_name);
    if (!database_entry) {
        throw std::runtime_error("Database not found: " + database_name);
    }

    // 设置数据库 ID
    bound_use_statement->database_id = database_entry->database_id_;

    return bound_use_statement;
}

std::unique_ptr<BoundStatement> Binder::bind_show_statement(const AstShowStatementNode & show_statement)
{
    // 创建绑定后的 SHOW 语句
    auto bound_show_statement = std::make_unique<BoundShowStatement>();

    if (!show_statement.has_show_operation()) {
        throw std::runtime_error("Show operation is required for SHOW statement");
    }

    if (show_statement.has_show_databases()) {
        // SHOW DATABASES - 不需要任何参数
        bound_show_statement->show_operation = BoundShowDatabases{};
    }
    else if (show_statement.has_show_collections()) {
        // SHOW COLLECTIONS
        const auto & show_collections = show_statement.get_show_collections();
        BoundShowCollections bound_show_collections;

        if (show_collections.has_database_name()) {
            const std::string & database_name = show_collections.get_database_name();
            const auto * database_entry = catalog_->get_database_entry(database_name);
            if (!database_entry) {
                throw std::runtime_error("Database not found: " + database_name);
            }
            bound_show_collections.database_id = database_entry->database_id_;
        }

        bound_show_statement->show_operation = bound_show_collections;
    }
    else if (show_statement.has_show_indexes()) {
        // SHOW INDEXES
        const auto & show_indexes = show_statement.get_show_indexes();
        BoundShowIndexes bound_show_indexes;

        std::string database_name = current_database_;
        if (show_indexes.has_database_name()) {
            database_name = show_indexes.get_database_name();
        }

        const std::string & collection_name = show_indexes.get_collection_name();
        const auto * collection_entry = catalog_->get_collection_entry(database_name, collection_name);
        if (!collection_entry) {
            throw std::runtime_error("Collection not found: " + collection_name);
        }

        bound_show_indexes.collection_id = collection_entry->collection_id_;

        if (show_indexes.has_database_name()) {
            const auto * database_entry = catalog_->get_database_entry(database_name);
            if (!database_entry) {
                throw std::runtime_error("Database not found: " + database_name);
            }
            bound_show_indexes.database_id = database_entry->database_id_;
        }

        bound_show_statement->show_operation = bound_show_indexes;
    }
    else if (show_statement.has_show_vindexes()) {
        // SHOW VINDEXES
        const auto & show_vindexes = show_statement.get_show_vindexes();
        BoundShowVIndexes bound_show_vindexes;

        std::string database_name = current_database_;
        if (show_vindexes.has_database_name()) {
            database_name = show_vindexes.get_database_name();
        }

        const std::string & collection_name = show_vindexes.get_collection_name();
        const auto * collection_entry = catalog_->get_collection_entry(database_name, collection_name);
        if (!collection_entry) {
            throw std::runtime_error("Collection not found: " + collection_name);
        }

        bound_show_vindexes.collection_id = collection_entry->collection_id_;

        if (show_vindexes.has_database_name()) {
            const auto * database_entry = catalog_->get_database_entry(database_name);
            if (!database_entry) {
                throw std::runtime_error("Database not found: " + database_name);
            }
            bound_show_vindexes.database_id = database_entry->database_id_;
        }

        bound_show_statement->show_operation = bound_show_vindexes;
    }
    else {
        throw std::runtime_error("Unknown show operation type");
    }

    return bound_show_statement;
}

std::unique_ptr<BoundStatement> Binder::bind_describe_statement(const AstDescribeStatementNode & describe_statement)
{
    // 创建绑定后的 DESCRIBE 语句
    auto bound_describe_statement = std::make_unique<BoundDescribeStatement>();

    if (!describe_statement.has_collection_name()) {
        throw std::runtime_error("Collection name is required for DESCRIBE statement");
    }

    const std::string & collection_name = describe_statement.get_collection_name();

    // 检查集合合法性
    const auto * collection_entry = catalog_->get_collection_entry(current_database_, collection_name);
    if (!collection_entry) {
        throw std::runtime_error("Collection not found: " + collection_name);
    }

    // 设置集合 ID
    bound_describe_statement->collection_id = collection_entry->collection_id_;

    return bound_describe_statement;
}

std::unique_ptr<BoundStatement> Binder::bind_drop_statement(const AstDropStatementNode & drop_statement)
{
    // 创建绑定后的 DROP 语句
    auto bound_drop_statement = std::make_unique<BoundDropStatement>();

    // 设置 if_exists 标志
    bound_drop_statement->if_exists = drop_statement.get_if_exists();

    if (!drop_statement.has_drop_operation()) {
        throw std::runtime_error("Drop operation is required for DROP statement");
    }

    if (drop_statement.has_drop_database()) {
        // DROP DATABASE
        const auto & drop_database = drop_statement.get_drop_database();
        BoundDropDatabase bound_drop_database;

        const std::string & database_name = drop_database.get_database_name();
        const auto * database_entry = catalog_->get_database_entry(database_name);
        if (!database_entry) {
            if (!bound_drop_statement->if_exists) {
                throw std::runtime_error("Database not found: " + database_name);
            }
            // 如果 if_exists 为 true，即使数据库不存在也不抛出异常，但需要设置一个无效的 ID
            bound_drop_database.database_id = 0;  // 无效 ID
        } else {
            bound_drop_database.database_id = database_entry->database_id_;
        }

        bound_drop_statement->drop_operation = bound_drop_database;
    }
    else if (drop_statement.has_drop_collection()) {
        // DROP COLLECTION
        const auto & drop_collection = drop_statement.get_drop_collection();
        BoundDropCollection bound_drop_collection;

        const std::string & collection_name = drop_collection.get_collection_name();
        const auto * collection_entry = catalog_->get_collection_entry(current_database_, collection_name);
        if (!collection_entry) {
            if (!bound_drop_statement->if_exists) {
                throw std::runtime_error("Collection not found: " + collection_name);
            }
            bound_drop_collection.collection_id = 0;  // 无效 ID
        } else {
            bound_drop_collection.collection_id = collection_entry->collection_id_;
        }

        bound_drop_statement->drop_operation = bound_drop_collection;
    }
    else if (drop_statement.has_drop_index()) {
        // DROP INDEX
        const auto & drop_index = drop_statement.get_drop_index();
        BoundDropIndex bound_drop_index;

        const std::string & collection_name = drop_index.get_collection_name();
        const auto * collection_entry = catalog_->get_collection_entry(current_database_, collection_name);
        if (!collection_entry) {
            throw std::runtime_error("Collection not found: " + collection_name);
        }

        bound_drop_index.collection_id = collection_entry->collection_id_;
        bound_drop_index.index_name = drop_index.get_index_name();

        // 验证索引是否存在（如果 if_exists 为 false）
        if (!bound_drop_statement->if_exists) {
            const auto * index_entry = collection_entry->get_index_entry(bound_drop_index.index_name);
            if (!index_entry) {
                throw std::runtime_error("Index not found: " + bound_drop_index.index_name);
            }
        }

        bound_drop_statement->drop_operation = bound_drop_index;
    }
    else if (drop_statement.has_drop_vindex()) {
        // DROP VINDEX
        const auto & drop_vindex = drop_statement.get_drop_vindex();
        BoundDropVIndex bound_drop_vindex;

        const std::string & collection_name = drop_vindex.get_collection_name();
        const auto * collection_entry = catalog_->get_collection_entry(current_database_, collection_name);
        if (!collection_entry) {
            throw std::runtime_error("Collection not found: " + collection_name);
        }

        bound_drop_vindex.collection_id = collection_entry->collection_id_;
        bound_drop_vindex.vindex_name = drop_vindex.get_vindex_name();

        // 验证向量索引是否存在（如果 if_exists 为 false）
        if (!bound_drop_statement->if_exists) {
            const auto * vindex_entry = collection_entry->get_vindex_entry(bound_drop_vindex.vindex_name);
            if (!vindex_entry) {
                throw std::runtime_error("VIndex not found: " + bound_drop_vindex.vindex_name);
            }
        }

        bound_drop_statement->drop_operation = bound_drop_vindex;
    }
    else {
        throw std::runtime_error("Unknown drop operation type");
    }

    return bound_drop_statement;
}

std::unique_ptr<BoundStatement> Binder::bind_create_statement(const AstCreateStatementNode & create_statement)
{
    auto bound_create_statement = std::make_unique<BoundCreateStatement>();
    bound_create_statement->if_not_exists = create_statement.get_if_not_exists();

    if (!create_statement.has_create_operation()) {
        throw std::runtime_error("Create operation is required for CREATE statement");
    }

    if (create_statement.has_create_database()) {
        // CREATE DATABASE - 只需要名称，创建时还没有 ID
        const auto & create_database = create_statement.get_create_database();
        BoundCreateDatabase bound_create_database;
        bound_create_database.database_name = create_database.get_database_name();
        bound_create_statement->create_operation = bound_create_database;
    }
    else if (create_statement.has_create_collection()) {
        // CREATE COLLECTION - 需要将列定义从 AstColumnDefinition 转换为 Field
        // 注意：由于列定义的转换涉及复杂的类型解析和默认值绑定，这里暂时只存储集合名称
        // 列定义将在执行阶段处理，因为需要更多上下文信息
        const auto & create_collection = create_statement.get_create_collection();
        BoundCreateCollection bound_create_collection;
        bound_create_collection.collection_name = create_collection.get_collection_name();
        // column_definitions 将在执行阶段从 AST 转换
        bound_create_collection.column_definitions.clear();
        bound_create_statement->create_operation = bound_create_collection;
    }
    else if (create_statement.has_create_index()) {
        // CREATE INDEX - 需要验证集合存在，将列名转换为列 ID，将类型字符串转换为枚举
        const auto & create_index = create_statement.get_create_index();
        BoundCreateIndex bound_create_index;

        const std::string & collection_name = create_index.get_collection_name();
        const auto * collection_entry = catalog_->get_collection_entry(current_database_, collection_name);
        if (!collection_entry) {
            throw std::runtime_error("Collection not found: " + collection_name);
        }

        bound_create_index.collection_id = collection_entry->collection_id_;
        bound_create_index.index_name = create_index.get_index_name();

        // 将列名转换为列 ID
        const auto & column_names = create_index.get_column_names();
        bound_create_index.column_ids.clear();
        bound_create_index.column_ids.reserve(column_names.size());
        for (const auto & column_name : column_names) {
            const auto * column_entry = collection_entry->get_column_entry(column_name);
            if (!column_entry) {
                throw std::runtime_error("Column not found: " + column_name + " in collection " + collection_name);
            }
            bound_create_index.column_ids.push_back(column_entry->column_index());
        }

        // 将索引类型字符串转换为枚举
        const std::string & index_type_str = create_index.get_index_type();
        if (index_type_str == "BTREE" || index_type_str == "btree") {
            bound_create_index.index_type = IndexType::BTREE;
        }
        else if (index_type_str == "HASH" || index_type_str == "hash") {
            bound_create_index.index_type = IndexType::HASH;
        }
        else {
            throw std::runtime_error("Unknown index type: " + index_type_str);
        }

        bound_create_statement->create_operation = bound_create_index;
    }
    else if (create_statement.has_create_vindex()) {
        // CREATE VINDEX - 需要验证集合存在，将列名转换为列 ID，将类型字符串转换为枚举
        const auto & create_vindex = create_statement.get_create_vindex();
        BoundCreateVIndex bound_create_vindex;

        const std::string & collection_name = create_vindex.get_collection_name();
        const auto * collection_entry = catalog_->get_collection_entry(current_database_, collection_name);
        if (!collection_entry) {
            throw std::runtime_error("Collection not found: " + collection_name);
        }

        bound_create_vindex.collection_id = collection_entry->collection_id_;
        bound_create_vindex.vindex_name = create_vindex.get_vindex_name();

        // 将列名转换为列 ID
        const std::string & column_name = create_vindex.get_column_name();
        const auto * column_entry = collection_entry->get_column_entry(column_name);
        if (!column_entry) {
            throw std::runtime_error("Column not found: " + column_name + " in collection " + collection_name);
        }
        bound_create_vindex.column_id = column_entry->column_index();

        // 将向量索引类型字符串转换为枚举
        const std::string & vindex_type_str = create_vindex.get_vindex_type();
        if (vindex_type_str == "FLAT" || vindex_type_str == "flat") {
            bound_create_vindex.vindex_type = VIndexType::FLAT;
        }
        else if (vindex_type_str == "IVF_FLAT" || vindex_type_str == "ivf_flat") {
            bound_create_vindex.vindex_type = VIndexType::IVF_FLAT;
        }
        else if (vindex_type_str == "HNSW" || vindex_type_str == "hnsw") {
            bound_create_vindex.vindex_type = VIndexType::HNSW;
        }
        else {
            throw std::runtime_error("Unknown vindex type: " + vindex_type_str);
        }

        // 处理 WITH 子句：将表达式转换为字符串键值对（仅支持字面量值）
        const auto & with_clauses = create_vindex.get_with_clauses();
        bound_create_vindex.with_clauses.clear();
        bound_create_vindex.with_clauses.reserve(with_clauses.size());
        for (const auto & clause : with_clauses) {
            std::string key = clause.get_key();
            std::string value_str;
            if (clause.has_value()) {
                const auto & value_expr = clause.get_value();
                if (value_expr->get_expression_type() == AstExpressionNodeType::AST_EXPRESSION_LITERAL) {
                    const auto & literal_expr = static_cast<const AstLiteralExpressionNode &>(*value_expr);
                    if (literal_expr.is_string()) {
                        value_str = literal_expr.get_string();
                    } else if (literal_expr.is_integer()) {
                        value_str = std::to_string(literal_expr.get_integer());
                    } else if (literal_expr.is_float()) {
                        value_str = std::to_string(literal_expr.get_float());
                    } else if (literal_expr.is_boolean()) {
                        value_str = literal_expr.get_boolean() ? "true" : "false";
                    } else {
                        throw std::runtime_error("Unsupported literal type in WITH clause");
                    }
                } else {
                    // 非字面量表达式需要在执行时处理，这里暂时跳过或抛出错误
                    throw std::runtime_error("Only literal values are supported in WITH clause during binding");
                }
            }
            bound_create_vindex.with_clauses.emplace_back(key, value_str);
        }

        bound_create_statement->create_operation = bound_create_vindex;
    }
    else {
        throw std::runtime_error("Unknown create operation type");
    }

    return bound_create_statement;
}

std::unique_ptr<BoundStatement> Binder::bind_alter_statement(const AstAlterStatementNode & alter_statement)
{
    auto bound_alter_statement = std::make_unique<BoundAlterStatement>();

    if (!alter_statement.has_collection_name()) {
        throw std::runtime_error("Collection name is required for ALTER statement");
    }

    const std::string & collection_name = alter_statement.get_collection_name();
    const auto * collection_entry = catalog_->get_collection_entry(current_database_, collection_name);
    if (!collection_entry) {
        throw std::runtime_error("Collection not found: " + collection_name);
    }

    bound_alter_statement->collection_id = collection_entry->collection_id_;

    if (!alter_statement.has_alter_operation()) {
        throw std::runtime_error("Alter operation is required for ALTER statement");
    }

    if (alter_statement.has_add_column()) {
        // ALTER ADD COLUMN
        // 列定义将在执行阶段从 AST 转换
        // 这里暂时创建一个空的 Field，实际转换在执行阶段进行
        BoundAlterAddColumn bound_add_column{
            Field::create_integer_field("", true, false)
        };
        bound_alter_statement->alter_operation = bound_add_column;
    }
    else if (alter_statement.has_drop_column()) {
        // ALTER DROP COLUMN
        const auto & drop_column = alter_statement.get_drop_column();
        BoundAlterDropColumn bound_drop_column;

        const std::string & column_name = drop_column.get_column_name();
        const auto * column_entry = collection_entry->get_column_entry(column_name);
        if (!column_entry) {
            throw std::runtime_error("Column not found: " + column_name + " in collection " + collection_name);
        }

        bound_drop_column.column_id = column_entry->column_index();
        bound_alter_statement->alter_operation = bound_drop_column;
    }
    else if (alter_statement.has_modify_column()) {
        // ALTER MODIFY COLUMN
        const auto & modify_column = alter_statement.get_modify_column();

        const auto & new_definition = modify_column.get_new_definition();
        if (!new_definition.has_name()) {
            throw std::runtime_error("Column name is required for MODIFY COLUMN");
        }

        const std::string & column_name = new_definition.get_name();
        const auto * column_entry = collection_entry->get_column_entry(column_name);
        if (!column_entry) {
            throw std::runtime_error("Column not found: " + column_name + " in collection " + collection_name);
        }

        // 新的列定义将在执行阶段从 AST 转换
        BoundAlterModifyColumn bound_modify_column{
            column_entry->column_index(),
            Field::create_integer_field("", true, false)
        };
        bound_alter_statement->alter_operation = bound_modify_column;
    }
    else if (alter_statement.has_rename_column()) {
        // ALTER RENAME COLUMN
        const auto & rename_column = alter_statement.get_rename_column();
        BoundAlterRenameColumn bound_rename_column;

        const std::string & old_name = rename_column.get_old_name();
        const auto * column_entry = collection_entry->get_column_entry(old_name);
        if (!column_entry) {
            throw std::runtime_error("Column not found: " + old_name + " in collection " + collection_name);
        }

        bound_rename_column.column_id = column_entry->column_index();
        bound_rename_column.new_name = rename_column.get_new_name();
        bound_alter_statement->alter_operation = bound_rename_column;
    }
    else {
        throw std::runtime_error("Unknown alter operation type");
    }

    return bound_alter_statement;
}

} // namespace dreamdb
