#include "dreamdb/parser/ast/debug/statement_formatter.h"

#include <sstream>

#include "dreamdb/parser/ast/debug/expression_formatter.h"
#include "dreamdb/parser/ast/statement/statement.h"
#include "dreamdb/parser/ast/statement/alter.h"
#include "dreamdb/parser/ast/statement/column_definition.h"
#include "dreamdb/parser/ast/statement/create.h"
#include "dreamdb/parser/ast/statement/delete.h"
#include "dreamdb/parser/ast/statement/describe.h"
#include "dreamdb/parser/ast/statement/drop.h"
#include "dreamdb/parser/ast/statement/insert.h"
#include "dreamdb/parser/ast/statement/select.h"
#include "dreamdb/parser/ast/statement/show.h"
#include "dreamdb/parser/ast/statement/update.h"
#include "dreamdb/parser/ast/statement/use.h"

namespace dreamdb::parser::ast
{

namespace // anonymous namespace
{

// SQL 风格字符串转义：单引号用两个单引号表示
std::string escape_sql_string(const std::string & input)
{
    std::string result;
    result.reserve(input.size());

    for (char c : input) {
        if (c == '\'') {
            result.push_back('\'');
            result.push_back('\'');
        } else {
            result.push_back(c);
        }
    }

    return result;
}

} // anonymous namespace

AstStatementFormatter::AstStatementFormatter() noexcept
    : expression_formatter_()
    , oss_()
{
}

std::string AstStatementFormatter::format(const AstStatement & statement)
{
    oss_.str("");
    oss_.clear();
    statement.accept(*this);
    return oss_.str();
}

void AstStatementFormatter::visit(const AstAlterStatement & alter_statement)
{
    oss_ << "ALTER COLLECTION " << alter_statement.collection_name() << " ";

    const AstAlterStatementOperation & operation = alter_statement.operation();
    std::visit([this](const auto & op) {
        using T = std::decay_t<decltype(op)>;

        if constexpr (std::is_same_v<T, AstAlterAddColumn>) {
            oss_ << "ADD COLUMN ";
            format_column_definition(op.column_definition);
        } else if constexpr (std::is_same_v<T, AstAlterDropColumn>) {
            oss_ << "DROP COLUMN " << op.column_name;
        } else if constexpr (std::is_same_v<T, AstAlterModifyColumn>) {
            oss_ << "MODIFY COLUMN ";
            format_column_definition(op.new_definition);
        } else if constexpr (std::is_same_v<T, AstAlterRenameColumn>) {
            oss_ << "RENAME COLUMN " << op.old_name << " TO " << op.new_name;
        }
    }, operation);
}

void AstStatementFormatter::visit(const AstCreateStatement & create_statement)
{
    oss_ << "CREATE ";

    const AstCreateStatementOperation & operation = create_statement.operation();
    bool if_not_exists = create_statement.if_not_exists();
    std::visit([if_not_exists, this](const auto & op) {
        using T = std::decay_t<decltype(op)>;

        if constexpr (std::is_same_v<T, AstCreateDatabase>) {
            if (if_not_exists) {
                oss_ << "DATABASE IF NOT EXISTS ";
            } else {
                oss_ << "DATABASE ";
            }
            oss_ << op.database_name;
        } else if constexpr (std::is_same_v<T, AstCreateCollection>) {
            if (if_not_exists) {
                oss_ << "COLLECTION IF NOT EXISTS ";
            } else {
                oss_ << "COLLECTION ";
            }
            oss_ << op.collection_name << " (";
            for (std::size_t i = 0; i < op.column_definitions.size(); ++i) {
                format_column_definition(op.column_definitions[i]);
                if (i < op.column_definitions.size() - 1) {
                    oss_ << ", ";
                }
            }
            oss_ << ")";
        } else if constexpr (std::is_same_v<T, AstCreateIndex>) {
            if (if_not_exists) {
                oss_ << "INDEX IF NOT EXISTS ";
            } else {
                oss_ << "INDEX ";
            }
            oss_ << op.index_name << " ON " << op.collection_name << " (";
            for (std::size_t i = 0; i < op.column_names.size(); ++i) {
                oss_ << op.column_names[i];
                if (i < op.column_names.size() - 1) {
                    oss_ << ", ";
                }
            }
            oss_ << ")";
            if (op.index_type.has_value()) {
                oss_ << " USING " << *op.index_type;
            }
        } else if constexpr (std::is_same_v<T, AstCreateVIndex>) {
            if (if_not_exists) {
                oss_ << "VINDEX IF NOT EXISTS ";
            } else {
                oss_ << "VINDEX ";
            }
            oss_ << op.vindex_name << " ON " << op.collection_name << "." << op.column_name;
            if (op.vindex_type.has_value()) {
                oss_ << " USING " << *op.vindex_type;
                // 格式化 WITH 子句
                if (!op.with_clauses.empty()) {
                    oss_ << " WITH (";
                    for (std::size_t i = 0; i < op.with_clauses.size(); ++i) {
                        oss_ << op.with_clauses[i].key << " = "
                            << expression_formatter_.format(*op.with_clauses[i].value);
                        if (i < op.with_clauses.size() - 1) {
                            oss_ << ", ";
                        }
                    }
                    oss_ << ")";
                }
            }
        }
    }, operation);
}

void AstStatementFormatter::visit(const AstDeleteStatement & delete_statement)
{
    oss_ << "DELETE FROM " << delete_statement.collection_name();

    // 格式化 WHERE 子句
    if (delete_statement.has_where()) {
        oss_ << " WHERE " << expression_formatter_.format(delete_statement.where_ref());
    }
}

void AstStatementFormatter::visit(const AstDescribeStatement & describe_statement)
{
    oss_ << "DESCRIBE " << describe_statement.collection_name();
}

void AstStatementFormatter::visit(const AstDropStatement & drop_statement)
{
    oss_ << "DROP ";

    const AstDropStatementOperation & operation = drop_statement.operation();
    bool if_exists = drop_statement.if_exists();
    std::visit([if_exists, this](const auto & op) {
        using T = std::decay_t<decltype(op)>;

        if constexpr (std::is_same_v<T, AstDropDatabase>) {
            oss_ << "DATABASE";
            if (if_exists) {
                oss_ << " IF EXISTS";
            }
            oss_ << " " << op.database_name;
        } else if constexpr (std::is_same_v<T, AstDropCollection>) {
            oss_ << "COLLECTION";
            if (if_exists) {
                oss_ << " IF EXISTS";
            }
            oss_ << " " << op.collection_name;
        } else if constexpr (std::is_same_v<T, AstDropIndex>) {
            oss_ << "INDEX";
            if (if_exists) {
                oss_ << " IF EXISTS";
            }
            oss_ << " " << op.index_name << " ON " << op.collection_name;
        } else if constexpr (std::is_same_v<T, AstDropVIndex>) {
            oss_ << "VINDEX";
            if (if_exists) {
                oss_ << " IF EXISTS";
            }
            oss_ << " " << op.vindex_name << " ON " << op.collection_name;
        }
    }, operation);
}

void AstStatementFormatter::visit(const AstInsertStatement & insert_statement)
{
    oss_ << "INSERT INTO " << insert_statement.collection_name();

    // 格式化列名列表
    if (insert_statement.has_column_names()) {
        oss_ << " (";
        for (std::size_t i = 0; i < insert_statement.column_name_count(); ++i) {
            oss_ << insert_statement.column_name_at(i);
            if (i < insert_statement.column_name_count() - 1) {
                oss_ << ", ";
            }
        }
        oss_ << ")";
    }

    // 格式化值列表
    oss_ << " VALUES (";
    for (std::size_t i = 0; i < insert_statement.value_count(); ++i) {
        oss_ << expression_formatter_.format(insert_statement.value_at(i));
        if (i < insert_statement.value_count() - 1) {
            oss_ << ", ";
        }
    }
    oss_ << ")";
}

void AstStatementFormatter::visit(const AstSelectStatement & select_statement)
{
    oss_ << "SELECT ";

    // 格式化 SELECT 投影列表
    for (std::size_t i = 0; i < select_statement.select_item_count(); ++i) {
        const AstSelectItem & select_item = select_statement.select_item_at(i);

        std::visit([this](const auto & item) {
            using T = std::decay_t<decltype(item)>;

            if constexpr (std::is_same_v<T, AstSelectStarItem>) {
                oss_ << "*";
            } else if constexpr (std::is_same_v<T, AstSelectExpressionItem>) {
                oss_ << expression_formatter_.format(*item.expression);
                if (item.alias.has_value()) {
                    oss_ << " AS " << *item.alias;
                }
            }
        }, select_item);

        if (i < select_statement.select_item_count() - 1) {
            oss_ << ", ";
        }
    }

    // 格式化 FROM 子句
    oss_ << " FROM " << select_statement.collection_name();

    // 格式化 WHERE 子句
    if (select_statement.has_where()) {
        oss_ << " WHERE " << expression_formatter_.format(select_statement.where_ref());
    }

    // 格式化 GROUP BY 子句
    if (select_statement.group_by_count() > 0) {
        oss_ << " GROUP BY ";
        for (std::size_t i = 0; i < select_statement.group_by_count(); ++i) {
            oss_ << expression_formatter_.format(select_statement.group_by_at(i));
            if (i < select_statement.group_by_count() - 1) {
                oss_ << ", ";
            }
        }
    }

    // 格式化 HAVING 子句
    if (select_statement.has_having()) {
        oss_ << " HAVING " << expression_formatter_.format(select_statement.having_ref());
    }

    // 格式化 ORDER BY 子句
    if (select_statement.order_by_item_count() > 0) {
        oss_ << " ORDER BY ";
        for (std::size_t i = 0; i < select_statement.order_by_item_count(); ++i) {
            const AstOrderByItem & order_by_item = select_statement.order_by_item_at(i);
            oss_ << expression_formatter_.format(*order_by_item.expression);
            if (order_by_item.direction == Direction::ASC) {
                oss_ << " ASC";
            } else if (order_by_item.direction == Direction::DESC) {
                oss_ << " DESC";
            }
            if (i < select_statement.order_by_item_count() - 1) {
                oss_ << ", ";
            }
        }
    }

    // 格式化 LIMIT, OFFSET 子句
    if (select_statement.limit().has_value()) {
        oss_ << " LIMIT " << *select_statement.limit();
    }
    if (select_statement.offset().has_value()) {
        oss_ << " OFFSET " << *select_statement.offset();
    }
}

void AstStatementFormatter::visit(const AstShowStatement & show_statement)
{
    const AstShowStatementOperation & operation = show_statement.operation();
    std::visit([this](const auto & op) {
        using T = std::decay_t<decltype(op)>;

        if constexpr (std::is_same_v<T, AstShowDatabases>) {
            oss_ << "SHOW DATABASES";
        } else if constexpr (std::is_same_v<T, AstShowCollections>) {
            oss_ << "SHOW COLLECTIONS";
            if (op.database_name.has_value()) {
                oss_ << " FROM " << op.database_name.value();
            }
        } else if constexpr (std::is_same_v<T, AstShowIndexes>) {
            oss_ << "SHOW INDEXES FROM " << op.collection_name;
            if (op.database_name.has_value()) {
                oss_ << " FROM " << op.database_name.value();
            }
        } else if constexpr (std::is_same_v<T, AstShowVIndexes>) {
            oss_ << "SHOW VINDEXES FROM " << op.collection_name;
            if (op.database_name.has_value()) {
                oss_ << " FROM " << op.database_name.value();
            }
        }
    }, operation);
}

void AstStatementFormatter::visit(const AstUpdateStatement & update_statement)
{
    oss_ << "UPDATE " << update_statement.collection_name() << " SET ";

    // 格式化 SET 子句
    for (std::size_t i = 0; i < update_statement.assignment_count(); ++i) {
        const AstUpdateAssignment & assignment = update_statement.assignment_at(i);
        oss_ << assignment.column_name << " = "
            << expression_formatter_.format(*assignment.value);
        if (i < update_statement.assignment_count() - 1) {
            oss_ << ", ";
        }
    }

    // 格式化 WHERE 子句
    if (update_statement.has_where()) {
        oss_ << " WHERE " << expression_formatter_.format(update_statement.where_ref());
    }
}

void AstStatementFormatter::visit(const AstUseStatement & use_statement)
{
    oss_ << "USE " << use_statement.database_name();
}

void AstStatementFormatter::format_column_definition(const AstColumnDefinition & column_def)
{
    oss_ << column_def.name() << " " << column_def.type_name();

    // 格式化类型参数
    if (column_def.argument_count() > 0) {
        oss_ << "(";
        for (std::size_t i = 0; i < column_def.argument_count(); ++i) {
            oss_ << expression_formatter_.format(column_def.argument_at(i));
            if (i < column_def.argument_count() - 1) {
                oss_ << ", ";
            }
        }
        oss_ << ")";
    }

    // 格式化修饰符
    for (std::size_t i = 0; i < column_def.modifier_count(); ++i) {
        AstColumnModifier modifier = column_def.modifier_at(i);
        switch (modifier) {
            case AstColumnModifier::NotNull:
                oss_ << " NOT NULL";
                break;
            case AstColumnModifier::Unique:
                oss_ << " UNIQUE";
                break;
            case AstColumnModifier::PrimaryKey:
                oss_ << " PRIMARY KEY";
                break;
            case AstColumnModifier::AutoIncrement:
                oss_ << " AUTO_INCREMENT";
                break;
            case AstColumnModifier::Default:
                if (column_def.has_default_value()) {
                    oss_ << " DEFAULT " << expression_formatter_.format(column_def.default_value_ref());
                }
                break;
        }
    }

    // 格式化注释
    if (column_def.has_comment()) {
        oss_ << " COMMENT '" << escape_sql_string(column_def.comment()) << "'";
    }
}

} // namespace dreamdb::parser::ast
