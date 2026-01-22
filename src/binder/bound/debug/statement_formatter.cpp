#include "dreamdb/binder/bound/debug/statement_formatter.h"

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
#include "dreamdb/binder/bound/expression/binary.h"
#include "dreamdb/binder/bound/expression/unary.h"
#include "dreamdb/common/type.h"

namespace dreamdb::binder::bound
{

namespace
{

/**
 * @brief 格式化表达式（辅助函数）
 */
std::string format_expression(const BoundExpression & expression, BoundExpressionFormatter & formatter)
{
    return formatter.format(expression);
}

/**
 * @brief 格式化二元运算符类型
 * @note 此函数为表达式格式化器准备，当前未使用
 */
[[maybe_unused]] std::string format_binary_operator(BoundBinaryOperatorType op_type)
{
    switch (op_type) {
        case BoundBinaryOperatorType::Plus:
            return "+";
        case BoundBinaryOperatorType::Minus:
            return "-";
        case BoundBinaryOperatorType::Multiply:
            return "*";
        case BoundBinaryOperatorType::Divide:
            return "/";
        case BoundBinaryOperatorType::Modulo:
            return "%";
        case BoundBinaryOperatorType::Equal:
            return "=";
        case BoundBinaryOperatorType::NotEqual:
            return "!=";
        case BoundBinaryOperatorType::LessThan:
            return "<";
        case BoundBinaryOperatorType::GreaterThan:
            return ">";
        case BoundBinaryOperatorType::LessEqual:
            return "<=";
        case BoundBinaryOperatorType::GreaterEqual:
            return ">=";
        case BoundBinaryOperatorType::And:
            return "AND";
        case BoundBinaryOperatorType::Or:
            return "OR";
        default:
            return "?";
    }
}

/**
 * @brief 格式化一元运算符类型
 * @note 此函数为表达式格式化器准备，当前未使用
 */
[[maybe_unused]] std::string format_unary_operator(BoundUnaryOperatorType op_type)
{
    switch (op_type) {
        case BoundUnaryOperatorType::Not:
            return "NOT";
        case BoundUnaryOperatorType::Minus:
            return "-";
        case BoundUnaryOperatorType::Plus:
            return "+";
        default:
            return "?";
    }
}

/**
 * @brief 格式化排序方向
 */
std::string format_direction(dreamdb::Direction direction)
{
    switch (direction) {
        case dreamdb::Direction::ASC:
            return "ASC";
        case dreamdb::Direction::DESC:
            return "DESC";
        default:
            return "?";
    }
}

/**
 * @brief 格式化索引类型
 */
std::string format_index_type(dreamdb::IndexType index_type)
{
    switch (index_type) {
        case dreamdb::IndexType::BTREE:
            return "BTREE";
        case dreamdb::IndexType::HASH:
            return "HASH";
        default:
            return "?";
    }
}

/**
 * @brief 格式化向量索引类型
 */
std::string format_vindex_type(dreamdb::VIndexType vindex_type)
{
    switch (vindex_type) {
        case dreamdb::VIndexType::FLAT:
            return "FLAT";
        case dreamdb::VIndexType::IVF_FLAT:
            return "IVF_FLAT";
        case dreamdb::VIndexType::HNSW:
            return "HNSW";
        default:
            return "?";
    }
}

} // anonymous namespace

BoundStatementFormatter::BoundStatementFormatter() noexcept
    : expression_formatter_()
    , oss_()
{
}

std::string BoundStatementFormatter::format(const BoundStatement & statement)
{
    oss_.str("");
    oss_.clear();
    statement.accept(*this);
    return oss_.str();
}

void BoundStatementFormatter::visit(const BoundAlterStatement & alter_statement)
{
    oss_ << "ALTER collection_id:" << alter_statement.collection_id() << " ";

    const auto & operation = alter_statement.alter_operation();
    std::visit([this](const auto & op) {
        using T = std::decay_t<decltype(op)>;

        if constexpr (std::is_same_v<T, BoundAlterAddColumn>) {
            oss_ << "ADD COLUMN " << op.column_definition.get_name();
        } else if constexpr (std::is_same_v<T, BoundAlterDropColumn>) {
            oss_ << "DROP COLUMN column_id:" << op.column_id;
        } else if constexpr (std::is_same_v<T, BoundAlterModifyColumn>) {
            oss_ << "MODIFY COLUMN column_id:" << op.column_id 
                 << " " << op.new_definition.get_name();
        } else if constexpr (std::is_same_v<T, BoundAlterRenameColumn>) {
            oss_ << "RENAME COLUMN column_id:" << op.column_id 
                 << " TO " << op.new_name;
        }
    }, operation);
}

void BoundStatementFormatter::visit(const BoundCreateStatement & create_statement)
{
    oss_ << "CREATE ";

    const auto & operation = create_statement.create_operation();
    std::visit([this, &create_statement](const auto & op) {
        using T = std::decay_t<decltype(op)>;
        
        if constexpr (std::is_same_v<T, BoundCreateDatabase>) {
            oss_ << "DATABASE " << op.database_name;
            if (create_statement.if_not_exists()) {
                oss_ << " IF NOT EXISTS";
            }
        } else if constexpr (std::is_same_v<T, BoundCreateCollection>) {
            oss_ << "COLLECTION ";
            if (create_statement.if_not_exists()) {
                oss_ << "IF NOT EXISTS ";
            }
            oss_ << op.collection_name << " (";
            for (std::size_t i = 0; i < op.column_definitions.size(); ++i) {
                if (i > 0) {
                    oss_ << ", ";
                }
                const auto & field = op.column_definitions[i];
                oss_ << field.get_name() << " " << static_cast<int>(field.get_type());
            }
            oss_ << ")";
        } else if constexpr (std::is_same_v<T, BoundCreateIndex>) {
            oss_ << "INDEX " << op.index_name 
                 << " ON collection_id:" << op.collection_id
                 << " (";
            for (std::size_t i = 0; i < op.column_ids.size(); ++i) {
                if (i > 0) {
                    oss_ << ", ";
                }
                oss_ << "column_id:" << op.column_ids[i];
            }
            oss_ << ") TYPE " << format_index_type(op.index_type);
            if (create_statement.if_not_exists()) {
                oss_ << " IF NOT EXISTS";
            }
        } else if constexpr (std::is_same_v<T, BoundCreateVIndex>) {
            oss_ << "VINDEX " << op.vindex_name
                 << " ON collection_id:" << op.collection_id
                 << " column_id:" << op.column_id
                 << " TYPE " << format_vindex_type(op.vindex_type);
            if (!op.with_options.empty()) {
                oss_ << " WITH (";
                for (std::size_t i = 0; i < op.with_options.size(); ++i) {
                    if (i > 0) {
                        oss_ << ", ";
                    }
                    oss_ << op.with_options[i].first << "=" << op.with_options[i].second;
                }
                oss_ << ")";
            }
            if (create_statement.if_not_exists()) {
                oss_ << " IF NOT EXISTS";
            }
        }
    }, operation);
}

void BoundStatementFormatter::visit(const BoundDeleteStatement & delete_statement)
{
    oss_ << "DELETE FROM collection_id:" << delete_statement.collection_id();

    if (delete_statement.has_where()) {
        oss_ << " WHERE " << format_expression(delete_statement.where_ref(), expression_formatter_);
    }
}

void BoundStatementFormatter::visit(const BoundDescribeStatement & describe_statement)
{
    oss_ << "DESCRIBE collection_id:" << describe_statement.collection_id();
}

void BoundStatementFormatter::visit(const BoundDropStatement & drop_statement)
{
    const auto & operation = drop_statement.operation();
    std::visit([this](const auto & op) {
        using T = std::decay_t<decltype(op)>;
        
        if constexpr (std::is_same_v<T, BoundDropDatabase>) {
            oss_ << "DROP DATABASE ";
            if (op.database_id.has_value()) {
                oss_ << "database_id:" << op.database_id.value();
            } else {
                oss_ << "database_id:NULL";
            }
        } else if constexpr (std::is_same_v<T, BoundDropCollection>) {
            oss_ << "DROP COLLECTION ";
            if (op.collection_id.has_value()) {
                oss_ << "collection_id:" << op.collection_id.value();
            } else {
                oss_ << "collection_id:NULL";
            }
        } else if constexpr (std::is_same_v<T, BoundDropIndex>) {
            oss_ << "DROP INDEX ";
            if (op.index_id.has_value()) {
                oss_ << "index_id:" << op.index_id.value();
            } else {
                oss_ << "index_id:NULL";
            }
        } else if constexpr (std::is_same_v<T, BoundDropVIndex>) {
            oss_ << "DROP VINDEX ";
            if (op.vindex_id.has_value()) {
                oss_ << "vindex_id:" << op.vindex_id.value();
            } else {
                oss_ << "vindex_id:NULL";
            }
        }
    }, operation);
}

void BoundStatementFormatter::visit(const BoundInsertStatement & insert_statement)
{
    oss_ << "INSERT INTO collection_id:" << insert_statement.collection_id() << " (";

    for (std::size_t i = 0; i < insert_statement.insert_item_count(); ++i) {
        if (i > 0) {
            oss_ << ", ";
        }
        const auto & item = insert_statement.insert_item_at(i);
        oss_ << format_expression(*item.column_reference, expression_formatter_);
    }

    oss_ << ") VALUES (";

    for (std::size_t i = 0; i < insert_statement.insert_item_count(); ++i) {
        if (i > 0) {
            oss_ << ", ";
        }
        const auto & item = insert_statement.insert_item_at(i);
        oss_ << format_expression(*item.value, expression_formatter_);
    }

    oss_ << ")";
}

void BoundStatementFormatter::visit(const BoundSelectStatement & select_statement)
{
    oss_ << "SELECT ";

    // 格式化 SELECT 项
    for (std::size_t i = 0; i < select_statement.select_item_count(); ++i) {
        if (i > 0) {
            oss_ << ", ";
        }
        const auto & item = select_statement.select_item_at(i);
        oss_ << format_expression(*item.expr, expression_formatter_);
        if (!item.alias.empty()) {
            oss_ << " AS " << item.alias;
        }
    }

    oss_ << " FROM collection_id:" << select_statement.collection_id();

    // 格式化 WHERE 子句
    if (select_statement.has_where()) {
        oss_ << " WHERE " << format_expression(select_statement.where_ref(), expression_formatter_);
    }

    // 格式化 GROUP BY 子句
    if (select_statement.group_by_count() > 0) {
        oss_ << " GROUP BY ";
        for (std::size_t i = 0; i < select_statement.group_by_count(); ++i) {
            if (i > 0) {
                oss_ << ", ";
            }
            oss_ << format_expression(*select_statement.group_by_at(i), expression_formatter_);
        }
    }

    // 格式化 HAVING 子句
    if (select_statement.has_having()) {
        oss_ << " HAVING " << format_expression(select_statement.having_ref(), expression_formatter_);
    }

    // 格式化 ORDER BY 子句
    if (select_statement.order_by_count() > 0) {
        oss_ << " ORDER BY ";
        for (std::size_t i = 0; i < select_statement.order_by_count(); ++i) {
            if (i > 0) {
                oss_ << ", ";
            }
            const auto & item = select_statement.order_by_at(i);
            oss_ << format_expression(*item.expr, expression_formatter_) 
                 << " " << format_direction(item.order);
        }
    }

    // 格式化 LIMIT 子句
    if (select_statement.has_limit()) {
        oss_ << " LIMIT " << select_statement.limit();
    }

    // 格式化 OFFSET 子句
    if (select_statement.has_offset()) {
        oss_ << " OFFSET " << select_statement.offset();
    }
}

void BoundStatementFormatter::visit(const BoundShowStatement & show_statement)
{
    const auto & operation = show_statement.operation();
    std::visit([this](const auto & op) {
        using T = std::decay_t<decltype(op)>;

        if constexpr (std::is_same_v<T, BoundShowDatabases>) {
            oss_ << "SHOW DATABASES";
        } else if constexpr (std::is_same_v<T, BoundShowCollections>) {
            oss_ << "SHOW COLLECTIONS";
            if (op.database_id.has_value()) {
                oss_ << " FROM database_id:" << op.database_id.value();
            }
        } else if constexpr (std::is_same_v<T, BoundShowIndexes>) {
            oss_ << "SHOW INDEXES FROM collection_id:" << op.collection_id;
        } else if constexpr (std::is_same_v<T, BoundShowVIndexes>) {
            oss_ << "SHOW VINDEXES FROM collection_id:" << op.collection_id;
        }
    }, operation);
}

void BoundStatementFormatter::visit(const BoundUpdateStatement & update_statement)
{
    oss_ << "UPDATE collection_id:" << update_statement.collection_id() << " SET ";

    // 格式化更新项
    for (std::size_t i = 0; i < update_statement.update_item_count(); ++i) {
        if (i > 0) {
            oss_ << ", ";
        }
        const auto & item = update_statement.update_item_at(i);
        oss_ << format_expression(*item.column_reference, expression_formatter_)
             << " = " << format_expression(*item.value, expression_formatter_);
    }

    // 格式化 WHERE 子句
    if (update_statement.has_where()) {
        oss_ << " WHERE " << format_expression(update_statement.where_ref(), expression_formatter_);
    }
}

void BoundStatementFormatter::visit(const BoundUseStatement & use_statement)
{
    oss_ << "USE database_id:" << use_statement.database_id();
}

} // namespace dreamdb::binder::bound
