#include "dreamdb/binder/binder.h"

#include <stdexcept>
#include <unordered_map>
#include <unordered_set>
#include <limits>

#include "dreamdb/catalog/catalog.h"
#include "dreamdb/parser/ast/statement/statement.h"
#include "dreamdb/parser/ast/statement/alter.h"
#include "dreamdb/parser/ast/statement/create.h"
#include "dreamdb/parser/ast/statement/delete.h"
#include "dreamdb/parser/ast/statement/describe.h"
#include "dreamdb/parser/ast/statement/drop.h"
#include "dreamdb/parser/ast/statement/insert.h"
#include "dreamdb/parser/ast/statement/select.h"
#include "dreamdb/parser/ast/statement/show.h"
#include "dreamdb/parser/ast/statement/update.h"
#include "dreamdb/parser/ast/statement/use.h"
#include "dreamdb/binder/bound/statement/statement.h"
#include "dreamdb/binder/bound/statement/alter.h"
#include "dreamdb/binder/bound/statement/create.h"
#include "dreamdb/binder/bound/statement/delete.h"
#include "dreamdb/binder/bound/statement/describe.h"
#include "dreamdb/binder/bound/statement/drop.h"
#include "dreamdb/binder/bound/statement/insert.h"
#include "dreamdb/binder/bound/statement/select.h"
#include "dreamdb/binder/bound/statement/show.h"
#include "dreamdb/binder/bound/statement/update.h"
#include "dreamdb/binder/bound/statement/use.h"
#include "dreamdb/binder/bound/expression/expression.h"
#include "dreamdb/binder/bound/expression/column_reference.h"
#include "dreamdb/binder/bound/expression/constant.h"
#include "dreamdb/binder/bound/expression/binary.h"
#include "dreamdb/binder/bound/expression/unary.h"
#include "dreamdb/binder/bound/expression/function_call.h"
#include "dreamdb/binder/bound/expression/in.h"
#include "dreamdb/binder/bound/expression/between.h"
#include "dreamdb/binder/bound/expression/like.h"
#include "dreamdb/parser/ast/statement/column_definition.h"
#include "dreamdb/parser/ast/expression/literal.h"
#include "dreamdb/parser/ast/expression/column_reference.h"
#include "dreamdb/parser/ast/expression/binary.h"
#include "dreamdb/parser/ast/expression/unary.h"
#include "dreamdb/parser/ast/expression/function_call.h"
#include "dreamdb/parser/ast/expression/in.h"
#include "dreamdb/parser/ast/expression/between.h"
#include "dreamdb/parser/ast/expression/like.h"
#include "dreamdb/parser/ast/expression/vector.h"
#include "dreamdb/schema/field.h"
#include "dreamdb/common/null.h"

namespace dreamdb::binder
{

namespace
{

/**
 * @brief 将类型名称字符串转换为 FieldType 枚举
 */
dreamdb::FieldType parse_field_type(const std::string & type_name)
{
    if (type_name == "TINYINT" || type_name == "tinyint") {
        return dreamdb::FieldType::TINYINT;
    } else if (type_name == "SMALLINT" || type_name == "smallint") {
        return dreamdb::FieldType::SMALLINT;
    } else if (type_name == "INTEGER" || type_name == "integer" || type_name == "INT" || type_name == "int") {
        return dreamdb::FieldType::INTEGER;
    } else if (type_name == "BIGINT" || type_name == "bigint") {
        return dreamdb::FieldType::BIGINT;
    } else if (type_name == "FLOAT" || type_name == "float") {
        return dreamdb::FieldType::FLOAT;
    } else if (type_name == "DOUBLE" || type_name == "double") {
        return dreamdb::FieldType::DOUBLE;
    } else if (type_name == "DECIMAL" || type_name == "decimal") {
        return dreamdb::FieldType::DECIMAL;
    } else if (type_name == "CHAR" || type_name == "char") {
        return dreamdb::FieldType::CHAR;
    } else if (type_name == "VARCHAR" || type_name == "varchar") {
        return dreamdb::FieldType::VARCHAR;
    } else if (type_name == "BOOLEAN" || type_name == "boolean" || type_name == "BOOL" || type_name == "bool") {
        return dreamdb::FieldType::BOOLEAN;
    } else if (type_name == "TIMESTAMP" || type_name == "timestamp") {
        return dreamdb::FieldType::TIMESTAMP;
    } else if (type_name == "ENUM" || type_name == "enum") {
        return dreamdb::FieldType::ENUM;
    } else if (type_name == "VECTOR" || type_name == "vector") {
        return dreamdb::FieldType::VECTOR;
    } else {
        throw std::runtime_error("Unknown field type: " + type_name);
    }
}

/**
 * @brief 从表达式中提取整数值
 * @param expression 表达式
 * @return 整数值，如果表达式不是整数字面量则抛出异常
 */
int extract_integer_from_expression(const dreamdb::parser::ast::AstExpression & expression)
{
    if (expression.expression_type() != dreamdb::parser::ast::AstExpressionType::Literal) {
        throw std::runtime_error("Expected integer literal expression for column type argument");
    }
    
    const auto & literal_expr = static_cast<const dreamdb::parser::ast::AstLiteralExpression &>(expression);
    const auto & literal_value = literal_expr.value();
    
    if (std::holds_alternative<std::int64_t>(literal_value)) {
        std::int64_t value = std::get<std::int64_t>(literal_value);
        if (value < std::numeric_limits<int>::min() || value > std::numeric_limits<int>::max()) {
            throw std::runtime_error("Integer value out of range for int type");
        }
        return static_cast<int>(value);
    } else {
        throw std::runtime_error("Expected integer literal, got other type");
    }
}

/**
 * @brief 从表达式中提取字符串值
 * @param expression 表达式
 * @return 字符串值，如果表达式不是字符串字面量则抛出异常
 */
std::string extract_string_from_expression(const dreamdb::parser::ast::AstExpression & expression)
{
    if (expression.expression_type() != dreamdb::parser::ast::AstExpressionType::Literal) {
        throw std::runtime_error("Expected string literal expression for column type argument");
    }

    const auto & literal_expr = static_cast<const dreamdb::parser::ast::AstLiteralExpression &>(expression);
    const auto & literal_value = literal_expr.value();

    if (std::holds_alternative<std::string>(literal_value)) {
        return std::get<std::string>(literal_value);
    } else {
        throw std::runtime_error("Expected string literal, got other type");
    }
}

/**
 * @brief 将类型名称字符串转换为 LogicalType
 */
dreamdb::common::LogicalType parse_logical_type(const std::string & type_name)
{
    if (type_name == "TINYINT" || type_name == "tinyint" ||
        type_name == "SMALLINT" || type_name == "smallint" ||
        type_name == "INTEGER" || type_name == "integer" || type_name == "INT" || type_name == "int" ||
        type_name == "BIGINT" || type_name == "bigint") {
        return dreamdb::common::LogicalType{dreamdb::common::LogicalTypeId::Integer};
    } else if (type_name == "FLOAT" || type_name == "float" ||
               type_name == "DOUBLE" || type_name == "double" ||
               type_name == "DECIMAL" || type_name == "decimal") {
        return dreamdb::common::LogicalType{dreamdb::common::LogicalTypeId::Float};
    } else if (type_name == "CHAR" || type_name == "char" ||
               type_name == "VARCHAR" || type_name == "varchar" ||
               type_name == "ENUM" || type_name == "enum") {
        return dreamdb::common::LogicalType{dreamdb::common::LogicalTypeId::String};
    } else if (type_name == "BOOLEAN" || type_name == "boolean" || type_name == "BOOL" || type_name == "bool") {
        return dreamdb::common::LogicalType{dreamdb::common::LogicalTypeId::Boolean};
    } else if (type_name == "TIMESTAMP" || type_name == "timestamp") {
        return dreamdb::common::LogicalType{dreamdb::common::LogicalTypeId::String};
    } else if (type_name == "VECTOR" || type_name == "vector") {
        return dreamdb::common::LogicalType{dreamdb::common::LogicalTypeId::Vector};
    } else {
        throw std::runtime_error("Unknown field type: " + type_name);
    }
}

/**
 * @brief 将 AstColumnModifier 转换为 BoundColumnModifier
 */
bound::BoundColumnModifier convert_column_modifier(dreamdb::parser::ast::AstColumnModifier modifier)
{
    switch (modifier) {
        case dreamdb::parser::ast::AstColumnModifier::NotNull:
            return bound::BoundColumnModifier::NotNull;
        case dreamdb::parser::ast::AstColumnModifier::Unique:
            return bound::BoundColumnModifier::Unique;
        case dreamdb::parser::ast::AstColumnModifier::PrimaryKey:
            return bound::BoundColumnModifier::PrimaryKey;
        case dreamdb::parser::ast::AstColumnModifier::AutoIncrement:
            return bound::BoundColumnModifier::AutoIncrement;
        case dreamdb::parser::ast::AstColumnModifier::Default:
            return bound::BoundColumnModifier::Default;
        default:
            throw std::runtime_error("Unknown column modifier");
    }
}

/**
 * @brief 将 AstColumnDefinition 转换为 BoundColumnDefinition
 * @param column_def AST 列定义
 * @param bind_expr_func 绑定表达式的函数对象
 */
template<typename BindExprFunc>
bound::BoundColumnDefinition convert_column_definition_to_bound(
    const dreamdb::parser::ast::AstColumnDefinition & column_def,
    BindExprFunc && bind_expr_func
)
{
    bound::BoundColumnDefinition bound_def;
    bound_def.name = column_def.name();
    bound_def.type = parse_logical_type(column_def.type_name());

    // 转换修饰符
    for (std::size_t i = 0; i < column_def.modifier_count(); ++i) {
        bound_def.modifiers.push_back(convert_column_modifier(column_def.modifier_at(i)));
    }

    // 绑定默认值表达式
    if (column_def.has_default_value()) {
        bound_def.default_value = bind_expr_func(*column_def.default_value());
    } else {
        bound_def.default_value = nullptr;
    }

    return bound_def;
}

/**
 * @brief 将字符串索引类型转换为 IndexType 枚举
 */
dreamdb::IndexType parse_index_type(const std::optional<std::string> & index_type_str)
{
    if (!index_type_str.has_value()) {
        return dreamdb::IndexType::BTREE;  // 默认类型
    }
    const std::string & type = index_type_str.value();
    if (type == "BTREE" || type == "btree" || type == "B-TREE" || type == "b-tree") {
        return dreamdb::IndexType::BTREE;
    } else if (type == "HASH" || type == "hash") {
        return dreamdb::IndexType::HASH;
    } else {
        throw std::runtime_error("Unknown index type: " + type);
    }
}

/**
 * @brief 将字符串向量索引类型转换为 VIndexType 枚举
 */
dreamdb::VIndexType parse_vindex_type(const std::optional<std::string> & vindex_type_str)
{
    if (!vindex_type_str.has_value()) {
        return dreamdb::VIndexType::FLAT;  // 默认类型
    }
    const std::string & type = vindex_type_str.value();
    if (type == "FLAT" || type == "flat") {
        return dreamdb::VIndexType::FLAT;
    } else if (type == "IVF_FLAT" || type == "ivf_flat" || type == "IVF-FLAT" || type == "ivf-flat") {
        return dreamdb::VIndexType::IVF_FLAT;
    } else if (type == "HNSW" || type == "hnsw") {
        return dreamdb::VIndexType::HNSW;
    } else {
        throw std::runtime_error("Unknown vector index type: " + type);
    }
}

/**
 * @brief 将字面量值转换为 FieldValue 和 LogicalType
 */
std::pair<dreamdb::FieldValue, dreamdb::common::LogicalType> convert_literal_value(
    const dreamdb::parser::ast::AstLiteralValue & literal_value
)
{
    return std::visit([](const auto & value) -> std::pair<dreamdb::FieldValue, dreamdb::common::LogicalType> {
        using T = std::decay_t<decltype(value)>;

        if constexpr (std::is_same_v<T, std::int64_t>) {
            // 整数：根据值的大小选择合适的类型
            // 这里统一使用 int64_t，实际可以根据值范围选择更小的类型
            return {
                static_cast<std::int64_t>(value),
                dreamdb::common::LogicalType{dreamdb::common::LogicalTypeId::Integer}
            };
        } else if constexpr (std::is_same_v<T, double>) {
            // 浮点数
            return {
                static_cast<double>(value),
                dreamdb::common::LogicalType{dreamdb::common::LogicalTypeId::Float}
            };
        } else if constexpr (std::is_same_v<T, std::string>) {
            // 字符串
            return {
                std::string(value),
                dreamdb::common::LogicalType{dreamdb::common::LogicalTypeId::String}
            };
        } else if constexpr (std::is_same_v<T, bool>) {
            // 布尔值
            return {
                static_cast<bool>(value),
                dreamdb::common::LogicalType{dreamdb::common::LogicalTypeId::Boolean}
            };
        } else if constexpr (std::is_same_v<T, dreamdb::Null>) {
            // 空值
            return {
                dreamdb::Null(),
                dreamdb::common::LogicalType{dreamdb::common::LogicalTypeId::Null}
            };
        } else {
            throw std::runtime_error("Unsupported literal value type");
        }
    }, literal_value);
}

/**
 * @brief 将 AST 二元运算符类型转换为 Bound 二元运算符类型
 */
bound::BoundBinaryOperatorType convert_binary_operator_type(
    dreamdb::parser::ast::AstBinaryOperatorType ast_type
)
{
    switch (ast_type) {
        case dreamdb::parser::ast::AstBinaryOperatorType::Plus:
            return bound::BoundBinaryOperatorType::Plus;
        case dreamdb::parser::ast::AstBinaryOperatorType::Minus:
            return bound::BoundBinaryOperatorType::Minus;
        case dreamdb::parser::ast::AstBinaryOperatorType::Multiply:
            return bound::BoundBinaryOperatorType::Multiply;
        case dreamdb::parser::ast::AstBinaryOperatorType::Divide:
            return bound::BoundBinaryOperatorType::Divide;
        case dreamdb::parser::ast::AstBinaryOperatorType::Modulo:
            return bound::BoundBinaryOperatorType::Modulo;
        case dreamdb::parser::ast::AstBinaryOperatorType::Equal:
            return bound::BoundBinaryOperatorType::Equal;
        case dreamdb::parser::ast::AstBinaryOperatorType::NotEqual:
            return bound::BoundBinaryOperatorType::NotEqual;
        case dreamdb::parser::ast::AstBinaryOperatorType::LessThan:
            return bound::BoundBinaryOperatorType::LessThan;
        case dreamdb::parser::ast::AstBinaryOperatorType::GreaterThan:
            return bound::BoundBinaryOperatorType::GreaterThan;
        case dreamdb::parser::ast::AstBinaryOperatorType::LessEqual:
            return bound::BoundBinaryOperatorType::LessEqual;
        case dreamdb::parser::ast::AstBinaryOperatorType::GreaterEqual:
            return bound::BoundBinaryOperatorType::GreaterEqual;
        case dreamdb::parser::ast::AstBinaryOperatorType::And:
            return bound::BoundBinaryOperatorType::And;
        case dreamdb::parser::ast::AstBinaryOperatorType::Or:
            return bound::BoundBinaryOperatorType::Or;
        default:
            throw std::runtime_error("Unknown binary operator type");
    }
}

/**
 * @brief 将 AST 一元运算符类型转换为 Bound 一元运算符类型
 */
bound::BoundUnaryOperatorType convert_unary_operator_type(
    dreamdb::parser::ast::AstUnaryOperatorType ast_type
)
{
    switch (ast_type) {
        case dreamdb::parser::ast::AstUnaryOperatorType::Not:
            return bound::BoundUnaryOperatorType::Not;
        case dreamdb::parser::ast::AstUnaryOperatorType::Minus:
            return bound::BoundUnaryOperatorType::Minus;
        case dreamdb::parser::ast::AstUnaryOperatorType::Plus:
            return bound::BoundUnaryOperatorType::Plus;
        default:
            throw std::runtime_error("Unknown unary operator type");
    }
}

/**
 * @brief 推断二元表达式的逻辑类型
 */
dreamdb::common::LogicalType infer_binary_expression_type(
    bound::BoundBinaryOperatorType operator_type,
    const bound::BoundExpression & left,
    const bound::BoundExpression & /* right */
)
{
    // 对于比较运算符和逻辑运算符，结果类型总是 Boolean
    if (operator_type >= bound::BoundBinaryOperatorType::Equal &&
        operator_type <= bound::BoundBinaryOperatorType::Or) {
        return dreamdb::common::LogicalType{dreamdb::common::LogicalTypeId::Boolean};
    }

    // 对于算术运算符，结果类型与操作数类型相同（简化处理，实际可能需要类型提升）
    // 这里使用左操作数的类型
    return left.logical_type();
}

} // anonymous namespace

Binder::Binder(const dreamdb::catalog::Catalog & catalog)
    : catalog_(catalog)
    , context_({std::nullopt, std::nullopt, std::nullopt})
{
}

std::unique_ptr<bound::BoundStatement> Binder::bind(const dreamdb::parser::ast::AstStatement & statement)
{
    switch (statement.statement_type()) {
        case dreamdb::parser::ast::AstStatementType::Alter:
            return bind_alter_statement(
                static_cast<const dreamdb::parser::ast::AstAlterStatement &>(statement)
            );
        case dreamdb::parser::ast::AstStatementType::Create:
            return bind_create_statement(
                static_cast<const dreamdb::parser::ast::AstCreateStatement &>(statement)
            );
        case dreamdb::parser::ast::AstStatementType::Delete:
            return bind_delete_statement(
                static_cast<const dreamdb::parser::ast::AstDeleteStatement &>(statement)
            );
        case dreamdb::parser::ast::AstStatementType::Describe:
            return bind_describe_statement(
                static_cast<const dreamdb::parser::ast::AstDescribeStatement &>(statement)
            );
        case dreamdb::parser::ast::AstStatementType::Drop:
            return bind_drop_statement(
                static_cast<const dreamdb::parser::ast::AstDropStatement &>(statement)
            );
        case dreamdb::parser::ast::AstStatementType::Insert:
            return bind_insert_statement(
                static_cast<const dreamdb::parser::ast::AstInsertStatement &>(statement)
            );
        case dreamdb::parser::ast::AstStatementType::Select:
            return bind_select_statement(
                static_cast<const dreamdb::parser::ast::AstSelectStatement &>(statement)
            );
        case dreamdb::parser::ast::AstStatementType::Show:
            return bind_show_statement(
                static_cast<const dreamdb::parser::ast::AstShowStatement &>(statement)
            );
        case dreamdb::parser::ast::AstStatementType::Update:
            return bind_update_statement(
                static_cast<const dreamdb::parser::ast::AstUpdateStatement &>(statement)
            );
        case dreamdb::parser::ast::AstStatementType::Use:
            return bind_use_statement(
                static_cast<const dreamdb::parser::ast::AstUseStatement &>(statement)
            );
        default:
            throw std::runtime_error("Unknown statement type");
    }
}

std::unique_ptr<bound::BoundStatement> Binder::bind_alter_statement(
    const dreamdb::parser::ast::AstAlterStatement & alter_statement
)
{
    // 获取集合名称
    const std::string & collection_name = alter_statement.collection_name();

    // 检查当前数据库是否存在
    if (!context_.current_database_id.has_value()) {
        throw std::runtime_error("Current database not set");
    }

    // 检查集合是否存在
    auto collection_id = catalog_.resolve_collection(
        context_.current_database_id.value(), collection_name
    );
    if (!collection_id.has_value()) {
        throw std::runtime_error("Collection " + collection_name + " not found");
    }

    // 设置当前集合上下文（用于列引用解析，例如在 ALTER MODIFY COLUMN 的默认值中）
    auto old_collection_id = context_.current_collection_id;
    context_.current_collection_id = collection_id.value();

    // 获取操作
    const auto & ast_operation = alter_statement.operation();

    auto result = std::visit([this, collection_id](const auto & op) -> std::unique_ptr<bound::BoundStatement> {
        using T = std::decay_t<decltype(op)>;

        if constexpr (std::is_same_v<T, dreamdb::parser::ast::AstAlterAddColumn>) {
            // ALTER ADD COLUMN
            auto bound_def = convert_column_definition_to_bound(
                op.column_definition,
                [this](const auto & expr) { return this->bind_expression(expr); }
            );
            bound::BoundAlterAddColumn bound_operation{std::move(bound_def)};

            // 创建 BoundAlterStatement 语句
            return std::make_unique<bound::BoundAlterStatement>(
                collection_id.value(), std::move(bound_operation)
            );
        } else if constexpr (std::is_same_v<T, dreamdb::parser::ast::AstAlterDropColumn>) {
            // ALTER DROP COLUMN
            bound::BoundAlterDropColumn bound_operation;

            // 解析列
            auto column_info = catalog_.resolve_column(collection_id.value(), op.column_name);
            if (!column_info.has_value()) {
                throw std::runtime_error("Column " + op.column_name + " not found");
            }
            bound_operation.column_id = column_info->id;

            // 创建 BoundAlterStatement 语句
            return std::make_unique<bound::BoundAlterStatement>(
                collection_id.value(), std::move(bound_operation)
            );
        } else if constexpr (std::is_same_v<T, dreamdb::parser::ast::AstAlterModifyColumn>) {
            // ALTER MODIFY COLUMN
            // 解析列（使用新定义的列名）
            auto column_info = catalog_.resolve_column(
                collection_id.value(), op.new_definition.name()
            );
            if (!column_info.has_value()) {
                throw std::runtime_error("Column " + op.new_definition.name() + " not found");
            }
            auto bound_def = convert_column_definition_to_bound(
                op.new_definition,
                [this](const auto & expr) { return this->bind_expression(expr); }
            );
            bound::BoundAlterModifyColumn bound_operation{column_info->id, std::move(bound_def)};

            // 创建 BoundAlterStatement 语句
            return std::make_unique<bound::BoundAlterStatement>(
                collection_id.value(), std::move(bound_operation)
            );
        } else if constexpr (std::is_same_v<T, dreamdb::parser::ast::AstAlterRenameColumn>) {
            // ALTER RENAME COLUMN
            bound::BoundAlterRenameColumn bound_operation;

            // 解析旧列名
            auto column_info = catalog_.resolve_column(collection_id.value(), op.old_name);
            if (!column_info.has_value()) {
                throw std::runtime_error("Column " + op.old_name + " not found");
            }
            bound_operation.column_id = column_info->id;
            bound_operation.new_name = op.new_name;

            // 创建 BoundAlterStatement 语句
            return std::make_unique<bound::BoundAlterStatement>(
                collection_id.value(), std::move(bound_operation)
            );
        }
    }, ast_operation);

    // 恢复上下文
    context_.current_collection_id = old_collection_id;

    return result;
}

std::unique_ptr<bound::BoundStatement> Binder::bind_create_statement(
    const dreamdb::parser::ast::AstCreateStatement & create_statement
)
{
    // 获取操作
    const auto & ast_operation = create_statement.operation();
    bool if_not_exists = create_statement.if_not_exists();

    return std::visit([this, if_not_exists](const auto & op) -> std::unique_ptr<bound::BoundStatement> {
        using T = std::decay_t<decltype(op)>;

        if constexpr (std::is_same_v<T, dreamdb::parser::ast::AstCreateDatabase>) {
            // CREATE DATABASE
            bound::BoundCreateDatabase bound_operation;
            bound_operation.database_name = op.database_name;

            // 创建 BoundCreateStatement 语句
            return std::make_unique<bound::BoundCreateStatement>(
                if_not_exists, std::move(bound_operation)
            );
        } else if constexpr (std::is_same_v<T, dreamdb::parser::ast::AstCreateCollection>) {
            // CREATE COLLECTION
            bound::BoundCreateCollection bound_operation;
            bound_operation.collection_name = op.collection_name;

            // 转换列定义
            for (const auto & column_def : op.column_definitions) {
                auto bound_def = convert_column_definition_to_bound(
                    column_def,
                    [this](const auto & expr) { return this->bind_expression(expr); }
                );
                bound_operation.column_definitions.push_back(std::move(bound_def));
            }

            // 创建 BoundCreateStatement 语句
            return std::make_unique<bound::BoundCreateStatement>(
                if_not_exists, std::move(bound_operation)
            );
        } else if constexpr (std::is_same_v<T, dreamdb::parser::ast::AstCreateIndex>) {
            // CREATE INDEX
            bound::BoundCreateIndex bound_operation;

            // 检查当前数据库是否存在
            if (!context_.current_database_id.has_value()) {
                throw std::runtime_error("Current database not set");
            }

            // 解析集合
            auto collection_id = catalog_.resolve_collection(
                context_.current_database_id.value(), op.collection_name
            );
            if (!collection_id.has_value()) {
                throw std::runtime_error("Collection " + op.collection_name + " not found");
            }
            bound_operation.collection_id = collection_id.value();

            // 解析列名列表
            for (const std::string & column_name : op.column_names) {
                auto column_info = catalog_.resolve_column(collection_id.value(), column_name);
                if (!column_info.has_value()) {
                    throw std::runtime_error("Column " + column_name + " not found");
                }
                bound_operation.column_ids.push_back(column_info->id);
            }

            // 解析索引类型
            bound_operation.index_type = parse_index_type(op.index_type);
            bound_operation.index_name = op.index_name;

            // 创建 BoundCreateStatement 语句
            return std::make_unique<bound::BoundCreateStatement>(
                if_not_exists, std::move(bound_operation)
            );
        } else if constexpr (std::is_same_v<T, dreamdb::parser::ast::AstCreateVIndex>) {
            // CREATE VINDEX
            bound::BoundCreateVIndex bound_operation;

            // 检查当前数据库是否存在
            if (!context_.current_database_id.has_value()) {
                throw std::runtime_error("Current database not set");
            }

            // 解析集合
            auto collection_id = catalog_.resolve_collection(
                context_.current_database_id.value(), op.collection_name
            );
            if (!collection_id.has_value()) {
                throw std::runtime_error("Collection " + op.collection_name + " not found");
            }
            bound_operation.collection_id = collection_id.value();

            // 解析列
            auto column_info = catalog_.resolve_column(collection_id.value(), op.column_name);
            if (!column_info.has_value()) {
                throw std::runtime_error("Column " + op.column_name + " not found");
            }
            bound_operation.column_id = column_info->id;

            // 解析向量索引类型
            bound_operation.vindex_type = parse_vindex_type(op.vindex_type);
            bound_operation.vindex_name = op.vindex_name;

            // 处理 WITH 子句选项
            // 注意：WITH 选项的值是表达式，需要在执行时计算
            // 当前设计：将表达式转换为字符串表示，由执行器处理
            // TODO: 考虑是否需要将表达式绑定后存储，或保持当前设计在执行时处理
            for (const auto & with_option : op.with_clauses) {
                // 绑定表达式以验证其有效性，但值需要在执行时计算
                auto bound_value = bind_expression(*with_option.value);
                if (!bound_value) {
                    throw std::runtime_error("Failed to bind WITH option value for key: " + with_option.key);
                }
                // 当前设计：存储键和空字符串，值在执行时从表达式计算
                // 如果需要，可以存储表达式的字符串表示或序列化形式
                bound_operation.with_options.push_back({with_option.key, ""});
            }

            // 创建 BoundCreateStatement 语句
            return std::make_unique<bound::BoundCreateStatement>(
                if_not_exists, std::move(bound_operation)
            );
        }
    }, ast_operation);
}

std::unique_ptr<bound::BoundStatement> Binder::bind_delete_statement(
    const dreamdb::parser::ast::AstDeleteStatement & delete_statement
)
{
    // 获取集合名称
    const std::string & collection_name = delete_statement.collection_name();

    // 检查当前数据库是否存在
    if (!context_.current_database_id.has_value()) {
        throw std::runtime_error("Current database not set");
    }

    // 检查集合是否存在
    auto collection_id = catalog_.resolve_collection(
        context_.current_database_id.value(), collection_name
    );
    if (!collection_id.has_value()) {
        throw std::runtime_error("Collection " + collection_name + " not found");
    }

    // 设置当前集合上下文（用于列引用解析）
    auto old_collection_id = context_.current_collection_id;
    context_.current_collection_id = collection_id.value();

    // 获取 WHERE 子句
    std::unique_ptr<bound::BoundExpression> bound_where = nullptr;
    if (!delete_statement.has_where()) {
        // 不包含 WHERE 子句，删除所有数据
        // 恢复上下文
        context_.current_collection_id = old_collection_id;
        return std::make_unique<bound::BoundDeleteStatement>(
            collection_id.value(), nullptr
        );
    }

    // 绑定 WHERE 子句
    bound_where = bind_expression(delete_statement.where_ref());

    // 恢复上下文
    context_.current_collection_id = old_collection_id;

    // 创建 BoundDeleteStatement 语句
    return std::make_unique<bound::BoundDeleteStatement>(collection_id.value(), std::move(bound_where));
}

std::unique_ptr<bound::BoundStatement> Binder::bind_describe_statement(
    const dreamdb::parser::ast::AstDescribeStatement & describe_statement
)
{
    // 获取集合名称
    const std::string & collection_name = describe_statement.collection_name();

    // 检查当前数据库是否存在
    // TODO: 删除上下文测试代码，当前数据库将在 CommandExecutor 中设置
    if (!context_.current_database_id.has_value()) {
        throw std::runtime_error("Current database not set");
    }

    // 检查集合是否存在
    auto collection_id = catalog_.resolve_collection(
        context_.current_database_id.value(), collection_name
    );
    if (!collection_id.has_value()) {
        throw std::runtime_error("Collection " + collection_name + " not found");
    }

    // 创建 BoundDescribe 语句
    return std::make_unique<bound::BoundDescribeStatement>(collection_id.value());
}

std::unique_ptr<bound::BoundStatement> Binder::bind_drop_statement(
    const dreamdb::parser::ast::AstDropStatement & drop_statement
)
{
    // 获取操作
    const auto & ast_operation = drop_statement.operation();
    return std::visit([this, &drop_statement](const auto & op) -> std::unique_ptr<bound::BoundStatement> {
        using T = std::decay_t<decltype(op)>;

        if constexpr (std::is_same_v<T, dreamdb::parser::ast::AstDropDatabase>) {
            // 获取数据库名称
            const std::string & database_name = op.database_name;
            // 获取是否有存在性检查
            bool if_exists = drop_statement.if_exists();

            // 创建 BoundDropDatabase 操作
            bound::BoundDropDatabase bound_operation;

            // 检查数据库是否存在
            auto database_id = catalog_.resolve_database(database_name);
            if (!database_id.has_value()) {
                if (if_exists) {
                    // 不存在但是有存在性检查
                    bound_operation.database_id = std::nullopt;
                } else {
                    // 不存在且没有存在性检查
                    throw std::runtime_error("Database " + database_name + " not found");
                }
            } else {
                // 存在
                bound_operation.database_id = database_id.value();
            }

            // 创建 BoundDropStatement 语句
            return std::make_unique<bound::BoundDropStatement>(std::move(bound_operation));
        } else if constexpr (std::is_same_v<T, dreamdb::parser::ast::AstDropCollection>) {
            // 获取集合名称
            const std::string & collection_name = op.collection_name;

            // 获取是否有存在性检查
            bool if_exists = drop_statement.if_exists();

            // 创建 BoundDropCollection 操作
            bound::BoundDropCollection bound_operation;

            // 检查当前数据库是否存在
            if (!context_.current_database_id.has_value()) {
                throw std::runtime_error("Current database not set");
            }

            // 检查集合是否存在
            auto collection_id = catalog_.resolve_collection(
                context_.current_database_id.value(), collection_name
            );
            if (!collection_id.has_value()) {
                if (if_exists) {
                    // 不存在但是有存在性检查
                    bound_operation.collection_id = std::nullopt;
                } else {
                    // 不存在且没有存在性检查
                    throw std::runtime_error("Collection " + collection_name + " not found");
                }
            } else {
                // 存在
                bound_operation.collection_id = collection_id.value();
            }

            // 创建 BoundDropStatement 语句
            return std::make_unique<bound::BoundDropStatement>(std::move(bound_operation));
        } else if constexpr (std::is_same_v<T, dreamdb::parser::ast::AstDropIndex>) {
            // 获取索引名称
            const std::string & index_name = op.index_name;
            // 获取集合名称
            const std::string & collection_name = op.collection_name;

            // 获取是否有存在性检查
            bool if_exists = drop_statement.if_exists();

            // 创建 BoundDropIndex 操作
            bound::BoundDropIndex bound_operation;

            // 检查当前数据库是否存在
            if (!context_.current_database_id.has_value()) {
                throw std::runtime_error("Current database not set");
            }

            // 解析集合
            auto collection_id = catalog_.resolve_collection(
                context_.current_database_id.value(), collection_name
            );
            if (!collection_id.has_value()) {
                if (if_exists) {
                    bound_operation.index_id = std::nullopt;
                    return std::make_unique<bound::BoundDropStatement>(std::move(bound_operation));
                }
                throw std::runtime_error("Collection " + collection_name + " not found");
            }

            // 解析索引
            auto index_id = catalog_.resolve_index(collection_id.value(), index_name);
            if (!index_id.has_value()) {
                if (if_exists) {
                    bound_operation.index_id = std::nullopt;
                } else {
                    throw std::runtime_error("Index " + index_name + " not found");
                }
            } else {
                bound_operation.index_id = index_id.value();
            }

            // 创建 BoundDropStatement 语句
            return std::make_unique<bound::BoundDropStatement>(std::move(bound_operation));
        } else if constexpr (std::is_same_v<T, dreamdb::parser::ast::AstDropVIndex>) {
            // 获取向量索引名称
            const std::string & vindex_name = op.vindex_name;
            // 获取集合名称
            const std::string & collection_name = op.collection_name;

            // 获取是否有存在性检查
            bool if_exists = drop_statement.if_exists();

            // 创建 BoundDropVIndex 操作
            bound::BoundDropVIndex bound_operation;

            // 检查当前数据库是否存在
            if (!context_.current_database_id.has_value()) {
                throw std::runtime_error("Current database not set");
            }

            // 解析集合
            auto collection_id = catalog_.resolve_collection(
                context_.current_database_id.value(), collection_name
            );
            if (!collection_id.has_value()) {
                if (if_exists) {
                    bound_operation.vindex_id = std::nullopt;
                    return std::make_unique<bound::BoundDropStatement>(std::move(bound_operation));
                }
                throw std::runtime_error("Collection " + collection_name + " not found");
            }

            // 解析向量索引
            auto vindex_id = catalog_.resolve_vindex(collection_id.value(), vindex_name);
            if (!vindex_id.has_value()) {
                if (if_exists) {
                    bound_operation.vindex_id = std::nullopt;
                } else {
                    throw std::runtime_error("VIndex " + vindex_name + " not found");
                }
            } else {
                bound_operation.vindex_id = vindex_id.value();
            }

            // 创建 BoundDropStatement 语句
            return std::make_unique<bound::BoundDropStatement>(std::move(bound_operation));
        }
    }, ast_operation);
}

std::unique_ptr<bound::BoundStatement> Binder::bind_insert_statement(
    const dreamdb::parser::ast::AstInsertStatement & insert_statement
)
{
    const std::string & collection_name = insert_statement.collection_name();

    if (!context_.current_database_id.has_value()) {
        throw std::runtime_error("Current database not set");
    }

    auto collection_id = catalog_.resolve_collection(
        context_.current_database_id.value(), collection_name
    );
    if (!collection_id.has_value()) {
        throw std::runtime_error("Collection " + collection_name + " not found");
    }

    // 设置当前集合上下文（用于列引用解析）
    auto old_collection_id = context_.current_collection_id;
    context_.current_collection_id = collection_id.value();

    auto column_info = catalog_.get_columns(collection_id.value());
    std::vector<bound::BoundInsertItem> insert_items;

    if (insert_statement.column_name_count() > 0) {
        // 指定了列名列表的情况
        // 验证列名：不能重复，必须在集合列中
        std::unordered_set<std::string> catalog_column_names;
        for (const auto & column : column_info) {
            catalog_column_names.insert(column.name);
        }

        std::unordered_set<std::string> insert_column_names;
        for (std::size_t i = 0; i < insert_statement.column_name_count(); ++i) {
            const std::string & column_name = insert_statement.column_name_at(i);
            if (insert_column_names.find(column_name) != insert_column_names.end()) {
                throw std::runtime_error("Column " + column_name + " is duplicated");
            }
            if (catalog_column_names.find(column_name) == catalog_column_names.end()) {
                throw std::runtime_error("Column " + column_name + " not found");
            }
            insert_column_names.insert(column_name);
        }

        // 创建列名到值索引的映射
        std::unordered_map<std::string, std::size_t> column_name_to_value_index;
        for (std::size_t i = 0; i < insert_statement.column_name_count(); ++i) {
            column_name_to_value_index[insert_statement.column_name_at(i)] = i;
        }

        // 按照表结构顺序生成插入项
        for (const auto & column : column_info) {
            auto it = column_name_to_value_index.find(column.name);
            if (it != column_name_to_value_index.end()) {
                auto column_reference = std::make_unique<bound::BoundColumnReferenceExpression>(
                    column.id,
                    column.logical_type
                );
                auto bound_value = bind_expression(insert_statement.value_at(it->second));

                bound::BoundInsertItem insert_item;
                insert_item.column_reference = std::move(column_reference);
                insert_item.value = std::move(bound_value);
                insert_items.push_back(std::move(insert_item));
            }
        }

        // 检查未提供的 NOT NULL 列
        for (const auto & column : column_info) {
            if (column_name_to_value_index.find(column.name) == column_name_to_value_index.end()) {
                if (!column.is_nullable) {
                    throw std::runtime_error(
                        "Column " + column.name + " is not nullable and must be provided a value"
                    );
                }
            }
        }
    } else {
        // 未指定列名列表，按照表结构顺序插入
        std::size_t value_count = insert_statement.value_count();
        std::size_t total_column_count = column_info.size();
        std::size_t non_nullable_column_count = 0;
        for (const auto & column : column_info) {
            if (!column.is_nullable) {
                ++non_nullable_column_count;
            }
        }

        // 验证值列表数量
        if (value_count < non_nullable_column_count) {
            throw std::runtime_error(
                "Value count (" + std::to_string(value_count) +
                ") is less than required non-nullable column count (" +
                std::to_string(non_nullable_column_count) + ")"
            );
        }
        if (value_count > total_column_count) {
            throw std::runtime_error(
                "Value count (" + std::to_string(value_count) +
                ") is greater than total column count (" +
                std::to_string(total_column_count) + ")"
            );
        }
        if (value_count != non_nullable_column_count && value_count != total_column_count) {
            throw std::runtime_error(
                "Value count (" + std::to_string(value_count) +
                ") must equal either non-nullable column count (" +
                std::to_string(non_nullable_column_count) + 
                ") or total column count (" + std::to_string(total_column_count) + ")"
            );
        }

        // 按照表结构顺序生成插入项
        std::size_t value_index = 0;
        for (const auto & column : column_info) {
            // 如果值数量等于非空列数量，跳过可空列
            if (value_count == non_nullable_column_count && column.is_nullable) {
                continue;
            }

            // 如果值列表已用完，检查剩余列是否允许为空
            if (value_index >= value_count) {
                if (!column.is_nullable) {
                    throw std::runtime_error(
                        "Column " + column.name + " is not nullable and must be provided a value"
                    );
                }
                continue;
            }

            auto column_reference = std::make_unique<bound::BoundColumnReferenceExpression>(
                column.id,
                column.logical_type
            );
            auto bound_value = bind_expression(insert_statement.value_at(value_index));

            bound::BoundInsertItem insert_item;
            insert_item.column_reference = std::move(column_reference);
            insert_item.value = std::move(bound_value);
            insert_items.push_back(std::move(insert_item));

            ++value_index;
        }

        if (value_index < value_count) {
            throw std::runtime_error(
                "Value count (" + std::to_string(value_count) + 
                ") exceeds the number of columns processed (" + 
                std::to_string(value_index) + ")"
            );
        }
    }

    // 恢复上下文
    context_.current_collection_id = old_collection_id;

    return std::make_unique<bound::BoundInsertStatement>(
        collection_id.value(), std::move(insert_items)
    );
}

std::unique_ptr<bound::BoundStatement> Binder::bind_select_statement(
    const dreamdb::parser::ast::AstSelectStatement & select_statement
)
{
    // 获取集合名称
    const std::string & collection_name = select_statement.collection_name();

    // 检查当前数据库是否存在
    if (!context_.current_database_id.has_value()) {
        throw std::runtime_error("Current database not set");
    }

    // 检查集合是否存在
    auto collection_id = catalog_.resolve_collection(
        context_.current_database_id.value(), collection_name
    );
    if (!collection_id.has_value()) {
        throw std::runtime_error("Collection " + collection_name + " not found");
    }

    // 设置当前集合上下文（用于列引用解析）
    auto old_collection_id = context_.current_collection_id;
    context_.current_collection_id = collection_id.value();

    // 获取集合的所有列信息（用于 SELECT *）
    auto column_info = catalog_.get_columns(collection_id.value());

    // 绑定 SELECT 项
    std::vector<bound::BoundSelectItem> bound_select_items;
    for (std::size_t i = 0; i < select_statement.select_item_count(); ++i) {
        const auto & select_item = select_statement.select_item_at(i);
        std::visit([&bound_select_items, &column_info, this](const auto & item) {
            using T = std::decay_t<decltype(item)>;

            if constexpr (std::is_same_v<T, dreamdb::parser::ast::AstSelectStarItem>) {
                // SELECT *：展开为所有列的列引用表达式
                for (const auto & column : column_info) {
                    auto column_reference = std::make_unique<bound::BoundColumnReferenceExpression>(
                        column.id,
                        column.logical_type
                    );
                    bound::BoundSelectItem bound_item;
                    bound_item.expr = std::move(column_reference);
                    bound_item.alias = "";  // SELECT * 没有别名
                    bound_select_items.push_back(std::move(bound_item));
                }
            } else if constexpr (std::is_same_v<T, dreamdb::parser::ast::AstSelectExpressionItem>) {
                // SELECT 表达式：绑定表达式
                auto bound_expr = bind_expression(*item.expression);
                if (!bound_expr) {
                    throw std::runtime_error("Failed to bind SELECT expression");
                }
                bound::BoundSelectItem bound_item;
                bound_item.expr = std::move(bound_expr);
                bound_item.alias = item.alias.value_or("");  // 使用别名或空字符串
                bound_select_items.push_back(std::move(bound_item));
            }
        }, select_item);
    }

    // 绑定 WHERE 子句
    std::unique_ptr<bound::BoundExpression> bound_where = nullptr;
    if (select_statement.has_where()) {
        bound_where = bind_expression(select_statement.where_ref());
        if (!bound_where) {
            throw std::runtime_error("Failed to bind WHERE clause");
        }
    }

    // 绑定 GROUP BY 子句
    std::vector<std::unique_ptr<bound::BoundExpression>> bound_group_by;
    for (std::size_t i = 0; i < select_statement.group_by_count(); ++i) {
        auto bound_expr = bind_expression(select_statement.group_by_at(i));
        if (!bound_expr) {
            throw std::runtime_error("Failed to bind GROUP BY expression");
        }
        bound_group_by.push_back(std::move(bound_expr));
    }

    // 绑定 HAVING 子句
    std::unique_ptr<bound::BoundExpression> bound_having = nullptr;
    if (select_statement.has_having()) {
        bound_having = bind_expression(select_statement.having_ref());
        if (!bound_having) {
            throw std::runtime_error("Failed to bind HAVING clause");
        }
    }

    // 绑定 ORDER BY 子句
    std::vector<bound::BoundOrderByItem> bound_order_by;
    for (std::size_t i = 0; i < select_statement.order_by_item_count(); ++i) {
        const auto & order_by_item = select_statement.order_by_item_at(i);
        auto bound_expr = bind_expression(*order_by_item.expression);
        if (!bound_expr) {
            throw std::runtime_error("Failed to bind ORDER BY expression");
        }
        bound::BoundOrderByItem bound_item;
        bound_item.expr = std::move(bound_expr);
        bound_item.order = order_by_item.direction;  // Direction 类型相同，直接使用
        bound_order_by.push_back(std::move(bound_item));
    }

    // 处理 LIMIT 和 OFFSET
    std::optional<std::size_t> bound_limit = std::nullopt;
    if (select_statement.limit().has_value()) {
        bound_limit = static_cast<std::size_t>(select_statement.limit().value());
    }

    std::optional<std::size_t> bound_offset = std::nullopt;
    if (select_statement.offset().has_value()) {
        bound_offset = static_cast<std::size_t>(select_statement.offset().value());
    }

    // 恢复上下文
    context_.current_collection_id = old_collection_id;

    // 创建 BoundSelectStatement 语句
    return std::make_unique<bound::BoundSelectStatement>(
        collection_id.value(),
        std::move(bound_select_items),
        std::move(bound_where),
        std::move(bound_group_by),
        std::move(bound_having),
        std::move(bound_order_by),
        bound_limit,
        bound_offset
    );
}

std::unique_ptr<bound::BoundStatement> Binder::bind_show_statement(
    const dreamdb::parser::ast::AstShowStatement & show_statement
)
{
    // 获取操作
    const auto & ast_operation = show_statement.operation();
    return std::visit([this](const auto & op) -> std::unique_ptr<bound::BoundStatement> {
        using T = std::decay_t<decltype(op)>;

        if constexpr (std::is_same_v<T, dreamdb::parser::ast::AstShowDatabases>) {
            // 创建 BoundShowDatabases 操作
            bound::BoundShowDatabases bound_operation;

            // 创建 BoundShowStatement 语句
            return std::make_unique<bound::BoundShowStatement>(std::move(bound_operation));
        }
        else if constexpr (std::is_same_v<T, dreamdb::parser::ast::AstShowCollections>) {
            // 创建 BoundShowCollections 操作
            bound::BoundShowCollections bound_operation;

            // 如果指定了数据库名称，解析数据库 ID
            if (op.database_name.has_value()) {
                auto database_id = catalog_.resolve_database(op.database_name.value());
                if (!database_id.has_value()) {
                    throw std::runtime_error("Database " + op.database_name.value() + " not found");
                }
                bound_operation.database_id = database_id.value();
            } else {
                // 如果没有指定数据库名称，设置为 nullopt，让 Executor 使用当前数据库
                bound_operation.database_id = std::nullopt;
            }

            // 创建 BoundShowStatement 语句
            return std::make_unique<bound::BoundShowStatement>(std::move(bound_operation));
        }
        else if constexpr (std::is_same_v<T, dreamdb::parser::ast::AstShowIndexes>) {
            // 创建 BoundShowIndexes 操作
            bound::BoundShowIndexes bound_operation;

            // 确定数据库 ID（用于解析集合名称）
            dreamdb::common::database_id_t database_id;
            if (op.database_name.has_value()) {
                // 如果指定了数据库名称，解析数据库 ID
                auto resolved_db_id = catalog_.resolve_database(op.database_name.value());
                if (!resolved_db_id.has_value()) {
                    throw std::runtime_error("Database " + op.database_name.value() + " not found");
                }
                database_id = resolved_db_id.value();
            } else {
                // 如果没有指定数据库名称，使用当前数据库
                if (!context_.current_database_id.has_value()) {
                    throw std::runtime_error("Current database not set");
                }
                database_id = context_.current_database_id.value();
            }

            // 解析集合 ID（全局唯一）
            auto collection_id = catalog_.resolve_collection(database_id, op.collection_name);
            if (!collection_id.has_value()) {
                throw std::runtime_error("Collection " + op.collection_name + " not found");
            }

            bound_operation.collection_id = collection_id.value();

            // 创建 BoundShowStatement 语句
            return std::make_unique<bound::BoundShowStatement>(std::move(bound_operation));
        }
        else if constexpr (std::is_same_v<T, dreamdb::parser::ast::AstShowVIndexes>) {
            // 创建 BoundShowVIndexes 操作
            bound::BoundShowVIndexes bound_operation;

            // 确定数据库 ID（用于解析集合名称）
            dreamdb::common::database_id_t database_id;
            if (op.database_name.has_value()) {
                // 如果指定了数据库名称，解析数据库 ID
                auto resolved_db_id = catalog_.resolve_database(op.database_name.value());
                if (!resolved_db_id.has_value()) {
                    throw std::runtime_error("Database " + op.database_name.value() + " not found");
                }
                database_id = resolved_db_id.value();
            } else {
                // 如果没有指定数据库名称，使用当前数据库
                if (!context_.current_database_id.has_value()) {
                    throw std::runtime_error("Current database not set");
                }
                database_id = context_.current_database_id.value();
            }

            // 解析集合 ID（全局唯一）
            auto collection_id = catalog_.resolve_collection(database_id, op.collection_name);
            if (!collection_id.has_value()) {
                throw std::runtime_error("Collection " + op.collection_name + " not found");
            }

            bound_operation.collection_id = collection_id.value();

            // 创建 BoundShowStatement 语句
            return std::make_unique<bound::BoundShowStatement>(std::move(bound_operation));
        }
    }, ast_operation);
}

std::unique_ptr<bound::BoundStatement> Binder::bind_update_statement(
    const dreamdb::parser::ast::AstUpdateStatement & update_statement
)
{
    // 获取集合名称
    const std::string & collection_name = update_statement.collection_name();

    // 检查当前数据库是否存在
    if (!context_.current_database_id.has_value()) {
        throw std::runtime_error("Current database not set");
    }

    // 检查集合是否存在
    auto collection_id = catalog_.resolve_collection(
        context_.current_database_id.value(), collection_name
    );
    if (!collection_id.has_value()) {
        throw std::runtime_error("Collection " + collection_name + " not found");
    }

    // 设置当前集合上下文（用于列引用解析）
    auto old_collection_id = context_.current_collection_id;
    context_.current_collection_id = collection_id.value();

    // 获取 UPDATE 子句
    std::unordered_set<dreamdb::common::column_id_t> update_column_ids;
    std::vector<bound::BoundUpdateItem> update_items;
    for (std::size_t i = 0; i < update_statement.assignment_count(); ++i) {
        const auto & assignment = update_statement.assignment_at(i);

        // 获取列名称
        const std::string & column_name = assignment.column_name;

        // 获取列 ID
        auto column_info = catalog_.resolve_column(
            collection_id.value(), column_name
        );
        if (!column_info.has_value()) {
            throw std::runtime_error("Column " + column_name + " not found");
        }
        auto column_id = column_info->id;

        // 判断列是否重复
        if (update_column_ids.find(column_id) != update_column_ids.end()) {
            throw std::runtime_error("Column " + column_name + " is duplicated");
        }

        // 绑定值表达式
        auto bound_value = bind_expression(*assignment.value);

        // 添加到列 ID 集合
        update_column_ids.insert(column_id);

        // 添加到更新项列表
        update_items.push_back(bound::BoundUpdateItem{
            std::make_unique<bound::BoundColumnReferenceExpression>(
                column_id, column_info->logical_type
            ),
            std::move(bound_value)
        });
    }

    // 获取 WHERE 子句
    std::unique_ptr<bound::BoundExpression> bound_where = nullptr;
    if (update_statement.has_where()) {
        // 绑定 WHERE 子句
        bound_where = bind_expression(update_statement.where_ref());
    }

    // 恢复上下文
    context_.current_collection_id = old_collection_id;

    // 创建 BoundUpdateStatement 语句
    return std::make_unique<bound::BoundUpdateStatement>(
        collection_id.value(), std::move(update_items), std::move(bound_where)
    );
}

std::unique_ptr<bound::BoundStatement> Binder::bind_use_statement(
    const dreamdb::parser::ast::AstUseStatement & use_statement
)
{
    // 获取数据库名称
    const std::string & database_name = use_statement.database_name();

    // 检查数据库是否存在
    auto database_id = catalog_.resolve_database(database_name);
    if (!database_id.has_value()) {
        throw std::runtime_error("Database " + database_name + " not found");
    }

    // 更新上下文
    // TODO: 删除上下文测试代码，更新上下文将在 CommandExecutor 中进行
    context_.current_database_name = database_name;
    context_.current_database_id = database_id.value();

    // 创建 BoundUse 语句
    return std::make_unique<bound::BoundUseStatement>(database_id.value());
}

std::unique_ptr<bound::BoundExpression> Binder::bind_expression(
    const dreamdb::parser::ast::AstExpression & expression
)
{
    switch (expression.expression_type()) {
        case dreamdb::parser::ast::AstExpressionType::Literal: {
            const auto & literal_expr = static_cast<const dreamdb::parser::ast::AstLiteralExpression &>(expression);
            auto [field_value, logical_type] = convert_literal_value(literal_expr.value());
            return std::make_unique<bound::BoundConstantExpression>(
                std::move(field_value), logical_type
            );
        }
        case dreamdb::parser::ast::AstExpressionType::ColumnReference: {
            const auto & column_expr = static_cast<const dreamdb::parser::ast::AstColumnReferenceExpression &>(expression);

            // 确定数据库和集合
            dreamdb::common::database_id_t database_id;
            if (column_expr.database_name().has_value()) {
                auto resolved_db_id = catalog_.resolve_database(column_expr.database_name().value());
                if (!resolved_db_id.has_value()) {
                    throw std::runtime_error("Database " + column_expr.database_name().value() + " not found");
                }
                database_id = resolved_db_id.value();
            } else {
                if (!context_.current_database_id.has_value()) {
                    throw std::runtime_error("Current database not set");
                }
                database_id = context_.current_database_id.value();
            }

            dreamdb::common::collection_id_t collection_id;
            if (column_expr.collection_name().has_value()) {
                auto resolved_collection_id = catalog_.resolve_collection(
                    database_id, column_expr.collection_name().value()
                );
                if (!resolved_collection_id.has_value()) {
                    throw std::runtime_error("Collection " + column_expr.collection_name().value() + " not found");
                }
                collection_id = resolved_collection_id.value();
            } else {
                // 如果没有指定集合名，从当前上下文中获取
                if (!context_.current_collection_id.has_value()) {
                    throw std::runtime_error(
                        "Collection name must be specified in column reference, or column reference must be used within a statement context (SELECT/UPDATE/DELETE/INSERT/ALTER)"
                    );
                }
                collection_id = context_.current_collection_id.value();
            }

            // 解析列
            auto column_info = catalog_.resolve_column(collection_id, column_expr.column_name());
            if (!column_info.has_value()) {
                throw std::runtime_error("Column " + column_expr.column_name() + " not found");
            }

            return std::make_unique<bound::BoundColumnReferenceExpression>(
                column_info->id, column_info->logical_type
            );
        }
        case dreamdb::parser::ast::AstExpressionType::Binary: {
            const auto & binary_expr = static_cast<const dreamdb::parser::ast::AstBinaryExpression &>(expression);

            // 递归绑定左右操作数
            auto bound_left = bind_expression(binary_expr.left());
            auto bound_right = bind_expression(binary_expr.right());

            if (!bound_left || !bound_right) {
                throw std::runtime_error("Failed to bind binary expression operands");
            }

            // 转换运算符类型
            auto bound_operator_type = convert_binary_operator_type(binary_expr.binary_type());
 
            // 推断结果类型
            auto logical_type = infer_binary_expression_type(
                bound_operator_type, *bound_left, *bound_right
            );

            return std::make_unique<bound::BoundBinaryExpression>(
                bound_operator_type,
                std::move(bound_left),
                std::move(bound_right),
                logical_type
            );
        }
        case dreamdb::parser::ast::AstExpressionType::Unary: {
            const auto & unary_expr = static_cast<const dreamdb::parser::ast::AstUnaryExpression &>(expression);

            // 递归绑定操作数
            auto bound_operand = bind_expression(unary_expr.operand());

            if (!bound_operand) {
                throw std::runtime_error("Failed to bind unary expression operand");
            }

            // 转换运算符类型
            auto bound_operator_type = convert_unary_operator_type(unary_expr.unary_type());

            // 对于逻辑非，结果类型是 Boolean；对于算术运算符，结果类型与操作数相同
            dreamdb::common::LogicalType logical_type;
            if (bound_operator_type == bound::BoundUnaryOperatorType::Not) {
                logical_type = dreamdb::common::LogicalType{dreamdb::common::LogicalTypeId::Boolean};
            } else {
                logical_type = bound_operand->logical_type();
            }

            return std::make_unique<bound::BoundUnaryExpression>(
                bound_operator_type,
                std::move(bound_operand),
                logical_type
            );
        }
        case dreamdb::parser::ast::AstExpressionType::FunctionCall: {
            const auto & func_expr = static_cast<const dreamdb::parser::ast::AstFunctionCallExpression &>(expression);

            // 绑定所有参数
            std::vector<std::unique_ptr<bound::BoundExpression>> bound_arguments;
            for (std::size_t i = 0; i < func_expr.argument_count(); ++i) {
                auto bound_arg = bind_expression(func_expr.argument_at(i));
                if (!bound_arg) {
                    throw std::runtime_error("Failed to bind function argument");
                }
                bound_arguments.push_back(std::move(bound_arg));
            }

            // 推断函数返回类型（简化处理，实际需要根据函数名和参数类型查找函数签名）
            // 这里暂时使用第一个参数的类型，或者 Boolean（对于聚合函数如 COUNT）
            dreamdb::common::LogicalType return_type;
            if (bound_arguments.empty()) {
                // 无参数函数（如 NOW()），暂时返回 String
                return_type = dreamdb::common::LogicalType{dreamdb::common::LogicalTypeId::String};
            } else {
                // 使用第一个参数的类型（简化处理）
                return_type = bound_arguments[0]->logical_type();
            }

            // 判断是否为聚合函数（简化处理，实际需要查找函数定义）
            bool is_aggregate = false;
            const std::string & func_name = func_expr.function_name();
            if (func_name == "COUNT" || func_name == "SUM" || func_name == "AVG" ||
                func_name == "MAX" || func_name == "MIN") {
                is_aggregate = true;
                // 聚合函数通常返回数值类型
                return_type = dreamdb::common::LogicalType{dreamdb::common::LogicalTypeId::Integer};
            }

            return std::make_unique<bound::BoundFunctionCallExpression>(
                func_name,
                std::move(bound_arguments),
                return_type,
                is_aggregate
            );
        }
        case dreamdb::parser::ast::AstExpressionType::In: {
            const auto & in_expr = static_cast<const dreamdb::parser::ast::AstInExpression &>(expression);

            // 绑定左侧表达式
            auto bound_left = bind_expression(in_expr.left());
            if (!bound_left) {
                throw std::runtime_error("Failed to bind IN expression left operand");
            }

            // 绑定所有值表达式
            std::vector<std::unique_ptr<bound::BoundExpression>> bound_values;
            for (std::size_t i = 0; i < in_expr.value_count(); ++i) {
                auto bound_value = bind_expression(in_expr.value_at(i));
                if (!bound_value) {
                    throw std::runtime_error("Failed to bind IN expression value");
                }
                bound_values.push_back(std::move(bound_value));
            }

            // IN 表达式的结果类型是 Boolean
            auto logical_type = dreamdb::common::LogicalType{dreamdb::common::LogicalTypeId::Boolean};

            return std::make_unique<bound::BoundInExpression>(
                std::move(bound_left),
                std::move(bound_values),
                logical_type,
                in_expr.is_not()
            );
        }
        case dreamdb::parser::ast::AstExpressionType::Between: {
            const auto & between_expr = static_cast<const dreamdb::parser::ast::AstBetweenExpression &>(expression);

            // 绑定所有子表达式
            auto bound_left = bind_expression(between_expr.left());
            auto bound_start = bind_expression(between_expr.start());
            auto bound_end = bind_expression(between_expr.end());

            if (!bound_left || !bound_start || !bound_end) {
                throw std::runtime_error("Failed to bind BETWEEN expression operands");
            }

            // BETWEEN 表达式的结果类型是 Boolean
            auto logical_type = dreamdb::common::LogicalType{dreamdb::common::LogicalTypeId::Boolean};

            return std::make_unique<bound::BoundBetweenExpression>(
                std::move(bound_left),
                std::move(bound_start),
                std::move(bound_end),
                logical_type,
                between_expr.is_not()
            );
        }
        case dreamdb::parser::ast::AstExpressionType::Like: {
            const auto & like_expr = static_cast<const dreamdb::parser::ast::AstLikeExpression &>(expression);

            // 绑定左右表达式
            auto bound_left = bind_expression(like_expr.left());
            auto bound_pattern = bind_expression(like_expr.pattern());

            if (!bound_left || !bound_pattern) {
                throw std::runtime_error("Failed to bind LIKE expression operands");
            }

            // LIKE 表达式的结果类型是 Boolean
            auto logical_type = dreamdb::common::LogicalType{dreamdb::common::LogicalTypeId::Boolean};

            return std::make_unique<bound::BoundLikeExpression>(
                std::move(bound_left),
                std::move(bound_pattern),
                logical_type,
                like_expr.is_not()
            );
        }
        case dreamdb::parser::ast::AstExpressionType::Vector: {
            // 向量表达式：需要绑定所有元素，然后创建一个向量常量
            // 这里简化处理，将向量表达式转换为常量表达式
            const auto & vector_expr = static_cast<const dreamdb::parser::ast::AstVectorExpression &>(expression);

            // 绑定所有元素并提取值
            std::vector<float> vector_values;
            for (std::size_t i = 0; i < vector_expr.element_count(); ++i) {
                auto bound_element = bind_expression(vector_expr.element_at(i));
                if (!bound_element) {
                    throw std::runtime_error("Failed to bind vector element");
                }

                // 检查元素是否为常量表达式
                if (bound_element->expression_type() != bound::BoundExpressionType::Constant) {
                    throw std::runtime_error("Vector elements must be constant expressions");
                }

                const auto & constant_expr = static_cast<const bound::BoundConstantExpression &>(*bound_element);
                
                // 提取浮点数值
                if (constant_expr.is_type<double>()) {
                    vector_values.push_back(static_cast<float>(constant_expr.value_as<double>()));
                } else if (constant_expr.is_type<std::int64_t>()) {
                    vector_values.push_back(static_cast<float>(constant_expr.value_as<std::int64_t>()));
                } else {
                    throw std::runtime_error("Vector elements must be numeric");
                }
            }

            // 创建向量常量
            dreamdb::FieldValue vector_field_value = std::move(vector_values);
            auto logical_type = dreamdb::common::LogicalType{dreamdb::common::LogicalTypeId::Vector};

            return std::make_unique<bound::BoundConstantExpression>(
                std::move(vector_field_value), logical_type
            );
        }
        default:
            throw std::runtime_error("Unsupported expression type");
    }
}

} // dreamdb::binder
