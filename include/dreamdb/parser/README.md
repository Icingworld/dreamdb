# DreamDB SQL 功能参考手册 v0.1.0

## 一、快速开始

### 1. 概述

本文档定义了`DreamDB`向量数据库支持的`SQL`语法规范。作为轻量级向量数据库，`DreamDB`仅支持部分标准`SQL`语句，并对标准`SQL`进行了改造。因此可能不能直接使用标准`SQL`语句来操作`DreamDB`。

整个手册涵盖了`CREATE`、`DROP`、`ALTER`、`INSERT`、`UPDATE`、`DELETE`、`SELECT`、`USE`、`SHOW`、`DESCRIBE`命令，覆盖了数据库管理、集合管理、标量索引管理、向量索引管理、数据修改、数据查询等各方面功能。

### 2. 数据类型

`DreamDB`支持的数据类型如下：

- 标量类型：

  |       类型        |            约束            |     说明     |
  | :---------------: | :------------------------: | :----------: |
  |      TINYINT      |             无             |   8 位整数   |
  |     SMALLINT      |             无             |  16 位整数   |
  |      INTEGER      |             无             |  32 位整数   |
  |      BIGINT       |             无             |  64 位整数   |
  |       FLOAT       |             无             | 单精度浮点数 |
  |      DOUBLE       |             无             | 双精度浮点数 |
  |   DECIMAL(p, s)   | p: 总有效位数，s: 小数位数 |   定点小数   |
  |      CHAR(n)      |     n: 最多存储字符数      |  定长字符串  |
  |    VARCHAR(n)     |     n: 最多存储字符数      |  变长字符串  |
  |      BOOLEAN      |             无             |    布尔值    |
  |     TIMESTAMP     |             无             |    时间戳    |
  | ENUM(v1, v2, ...) |    取值必须在枚举列表中    |   枚举类型   |

- 向量类型：

  |   类型    |    约束     |        说明        |
  | :-------: | :---------: | :----------------: |
  | VECTOR(n) | n: 向量维度 | 单精度浮点向量类型 |

## 二、数据库管理

### 1. USE DATABASE

**说明**

切换当前使用的数据库，后续操作默认在该数据库中进行。如果不进行该操作，则后续所有操作将会报错。

**语法**

```sql
USE <database_name>;
```

**参数说明**

- `database_name`：要切换的数据库名称

**可能的响应**

- 成功
```txt
Database changed
```

- 失败 - 数据库不存在
```txt
Error: Unknown database 'database_name'
```

### 2. CREATE DATABASE

**说明**

创建一个新的数据库

**语法**

```sql
CREATE DATABASE <database_name>;
```

**参数说明**

- `database_name`：要创建的数据库名称

**可能的响应**

- 成功
```txt
Database created
```

- 失败 - 数据库已存在
```txt
Error: Database 'database_name' already exists
```

### 3. DROP DATABASE

**说明**

删除一个数据库，该数据库下的所有集合将被删除

**语法**

```sql
DROP DATABASE <database_name>;
```

**参数说明**

- `database_name`：要删除的数据库名称

**可能的响应**

- 成功
```txt
Database dropped
```

- 失败 - 数据库不存在
```txt
Error: Unknown database 'database_name'
```

### 4. SHOW DATABASES

**说明**

列出当前所有数据库

**语法**

```sql
SHOW DATABASES;
```

**可能的响应**

- 成功
```txt
Database
Database1
Database2
```

- 成功 - 没有数据库
```txt
No databases found
```

## 三、集合管理

### 1. CREATE COLLECTION

**说明**

创建一个新的集合

**语法**

```sql
CREATE COLLECTION <collection_name> (
    <field1_name> <field1_type> [constraint1],
    [...]
);
```

**参数说明**

- `collection_name`：要创建的集合名称
- `field1_name`：字段名称
- `field1_type`：字段类型
- `constraint1`：字段约束，可选值为`UNIQUE`、`NOT NULL`、`PRIMARY KEY`等

**可能的响应**

- 成功
```txt
Collection created
```

- 失败 - 集合已存在
```txt
Error: Collection 'collection_name' already exists
```

### 2. DROP COLLECTION

**说明**

删除一个集合，该集合下的所有数据将被删除

**语法**

```sql
DROP COLLECTION <collection_name>;
```

**参数说明**

- `collection_name`：要删除的集合名称

**可能的响应**

- 成功
```txt
Collection dropped
```

- 失败 - 集合不存在
```txt
Error: Unknown collection 'collection_name'
```

### 3. SHOW COLLECTIONS

**说明**

列出当前所有集合

**语法**

```sql
SHOW COLLECTIONS;
```

**可能的响应**

- 成功
```txt
Collection
Collection1
Collection2
```

- 成功 - 没有集合
```txt
No collections found
```

### 4. DESCRIBE COLLECTION

**说明**

列出指定集合的详细信息，包括集合名称、主键字段、字段类型、向量维度等

**语法**

```sql
{DESCRIBE | DESC} COLLECTION <collection_name>;
```

**参数说明**

- `DESC`: `DESCRIBE`命令的简写，具有相同的功能
- `collection_name`：要列出的集合名称

**可能的响应**

- 成功
```txt
Field1: field1_type
Field2: field2_type
Field3: field3_type
...
```

- 失败 - 集合不存在
```txt
Error: Unknown collection 'collection_name'
```

### 5. ALTER COLLECTION

> 暂不支持修改集合


## 四、标量索引管理

### 1. CREATE INDEX

**说明**

创建一个新的标量索引，该索引用于加速查询，索引不允许建立在`vector`类型的字段上。

**语法**

```sql
CREATE INDEX <index_name>
ON <collection_name> (<field_name>)
[USING BTREE | HASH];
```

**参数说明**

- `index_name`：索引名称，必须唯一
- `collection_name`：集合名称
- `field_name`：要建立索引的标量字段名（不能是`vector`类型的字段）
- `USING BTREE | HASH`：可选，指定索引类型，默认为`BTREE`

**可能的响应**

- 成功
```txt
Index created
```

- 失败 - 索引已存在
```txt
Error: Index 'index_name' already exists
```

### 2. DROP INDEX

**说明**

删除一个标量索引

**语法**

```sql
DROP INDEX <index_name>;
```

**参数说明**

- `index_name`：要删除的索引名称

**可能的响应**

- 成功
```txt
Index dropped
```

- 失败 - 索引不存在
```txt
Error: Unknown index 'index_name'
```

### 3. SHOW INDEXES

**说明**

列出指定集合的所有索引

**语法**

```sql
SHOW INDEXES FROM <collection_name>;
```

**参数说明**

- `collection_name`：要列出的集合名称

**可能的响应**

- 成功
```txt
Index
Index1
Index2
```

- 成功 - 没有标量索引
```txt
No indexes found
```

## 五、向量索引管理

### 1. CREATE VINDEX

**说明**

创建一个新的向量索引，该索引用于加速查询，索引字段必须为`vector`类型的字段。

**语法**

```sql
CREATE VINDEX <vindex_name>
ON <collection_name> (<field_name>)
[USING FLAT | IVF_FLAT | HNSW]
[WITH (
    nlist = <nlist>,
    M = <M>,
    ef_construction = <ef_construction>,
    metric = <metric>
)];
```

**参数说明**

- `vindex_name`：向量索引名称，必须唯一
- `collection_name`：集合名称
- `field_name`：要建立索引的向量字段名（必须是`vector`类型的字段）
- `USING FLAT | IVF_FLAT | HNSW`：可选，指定索引类型，默认为`FLAT`
- `nlist`：可选，指定`IVF_FLAT`索引的`nlist`参数
- `M`：可选，指定`HNSW`索引的`M`参数
- `ef_construction`：可选，指定`HNSW`索引的`ef_construction`参数
- `metric`：可选，指定索引的距离度量方式，默认为`L2`

**可能的响应**

- 成功
```txt
Vindex created
```

- 失败 - 向量索引已存在
```txt
Error: Vindex 'vindex_name' already exists
```

### 2. DROP VINDEX

**说明**

删除一个向量索引

**语法**

```sql
DROP VINDEX <vindex_name>;
```

**参数说明**

- `vindex_name`：要删除的向量索引名称

**可能的响应**

- 成功
```txt
Vindex dropped
```

- 失败 - 向量索引不存在
```txt
Error: Unknown vindex 'vindex_name'
```

### 3. SHOW VINDEXES

**说明**

列出指定集合的所有向量索引

**语法**

```sql
SHOW VINDEXES FROM <collection_name>;
```

**参数说明**

- `collection_name`：要列出的集合名称

**可能的响应**

- 成功
```txt
Vindex
Vindex1
Vindex2
```

- 成功 - 没有向量索引
```txt
No vindexes found
```

## 六、数据修改

### 1. INSERT

**说明**

向集合中插入一条新记录。支持插入标量字段和向量字段。向量值以数组形式表示，如 `[1.0, 2.0, 3.0, ...]`。所有字段均由用户在创建集合时定义，插入时需要提供所有必需字段（如 `NOT NULL` 约束的字段）。

**语法**

```sql
INSERT INTO <collection_name> (<field1>, <field2>, ...)
VALUES (<value1>, <value2>, ...);
```

**参数说明**

- `collection_name`：要插入数据的集合名称
- `fieldN`：集合中定义的字段名，可以是标量字段或向量字段
- `valueN`：与字段类型匹配的具体取值
  - 标量字段：根据字段类型提供相应值（字符串、数字、布尔值等）
  - 向量字段：值为浮点数数组，格式为 `[v1, v2, v3, ...]`，例如 `[0.1, 0.2, 0.3]`
  - 向量值的维度必须与字段定义时的维度一致

**示例**

- 插入包含向量和标量字段的记录
```sql
INSERT INTO products (id, vector, name, price, category)
VALUES ('prod_001', [0.1, 0.2, 0.3, 0.4], 'Product A', 99.99, 'Electronics');
```

- 仅插入部分字段（其他字段使用默认值或允许为 NULL）
```sql
INSERT INTO products (id, name)
VALUES ('prod_002', 'Product B');
```

- 插入时省略字段名（按顺序提供所有字段值）
```sql
INSERT INTO products
VALUES ('prod_003', [0.9, 1.0, 1.1, 1.2], 'Product C', 199.99, 'Books');
```

**注意事项**

- 必须提供所有 `NOT NULL` 约束的字段值
- 向量值的维度必须与字段定义时的维度一致
- 向量值必须是浮点数数组，维度不匹配或类型错误将导致插入失败
- 如果字段有 `PRIMARY KEY` 约束，必须提供唯一值

**可能的响应**

- 成功
```txt
1 row inserted
```

- 失败 - 向量维度不匹配
```txt
Error: Vector dimension mismatch, expected 4 but got 3
```

- 失败 - 缺少必需字段
```txt
Error: Missing required field 'field_name'
```

- 失败 - 主键冲突
```txt
Error: Duplicate entry for primary key
```

- 失败 - 集合不存在
```txt
Error: Unknown collection 'collection_name'
```

### 2. UPDATE

**说明**

更新集合中已有记录的字段值。

**语法**

```sql
UPDATE <collection_name>
SET <field1> = <value1> [, <field2> = <value2> ...]
[WHERE <condition>]
[ORDER BY <field> [ASC | DESC]]
[LIMIT <limit>];
```

**参数说明**

- `collection_name`：要更新数据的集合名称
- `SET field = value`：要更新的字段及其新值，可以同时更新多个字段
  - 支持更新标量字段（如 `name`、`price`、`category` 等）
  - 支持更新向量字段，向量值格式为数组，例如 `[0.1, 0.2, 0.3]`
  - 支持更新主键字段（如果用户定义了主键）
- `WHERE condition`：可选，更新条件表达式；仅支持基于标量字段的条件
- `ORDER BY <field> [ASC | DESC]`：可选，指定更新时的记录排序（多用于配合 `LIMIT`）
- `LIMIT <limit>`：可选，限制本次最多更新的记录条数

**示例**

- 更新单个标量字段
```sql
UPDATE products
SET price = 99.9
WHERE id = 1;
```

- 同时更新多个标量字段
```sql
UPDATE products
SET status = 'active', stock = stock + 10
WHERE category = 'Book';
```

- 更新向量字段
```sql
UPDATE products
SET vector = [0.1, 0.2, 0.3, 0.4]
WHERE id = 1;
```

- 同时更新向量字段和标量字段
```sql
UPDATE products
SET vector = [0.5, 0.6, 0.7, 0.8], status = 'updated'
WHERE category = 'Electronics';
```

- 更新主键字段
```sql
UPDATE products
SET id = 'new_id_001'
WHERE id = 'old_id_001';
```

- 限制更新数量
```sql
UPDATE products
SET status = 'archived'
ORDER BY created_at ASC
LIMIT 100;
```

**注意事项**

- 更新向量字段时，向量维度必须与字段定义时的维度一致
- 更新主键字段时，新值必须唯一，否则会导致更新失败
- 向量值必须是浮点数数组，维度不匹配将导致更新失败

**可能的响应**

- 成功
```txt
Rows matched: 10  Rows updated: 10
```

- 没有匹配的数据
```txt
Rows matched: 0  Rows updated: 0
```

- 失败 - 主键冲突
```txt
Error: Duplicate entry for primary key
```

- 失败 - 向量维度不匹配
```txt
Error: Vector dimension mismatch, expected 4 but got 3
```

- 失败 - 集合不存在
```txt
Error: Unknown collection 'collection_name'
```

### 3. DELETE

**说明**

从集合中删除一条或多条记录。默认会删除所有满足条件的记录，可通过 `LIMIT` 控制本次删除的最大条数。

**语法**

```sql
DELETE FROM <collection_name>
[WHERE <condition>]
[ORDER BY <field> [ASC | DESC]]
[LIMIT <limit>];
```

**参数说明**

- `collection_name`：要删除数据的集合名称
- `WHERE condition`：可选，删除条件表达式；仅支持基于标量字段的条件
- `ORDER BY <field> [ASC | DESC]`：可选，指定删除时的记录排序（多用于配合 `LIMIT`）
- `LIMIT <limit>`：可选，限制本次最多删除的记录条数；未指定时，将删除所有满足条件的记录

**示例**

- 按主键删除
```sql
DELETE FROM products WHERE id = 1;
```

- 按条件批量删除
```sql
DELETE FROM products
WHERE status = 'archived';
```

- 按时间顺序删除部分旧数据
```sql
DELETE FROM products
WHERE status = 'inactive'
ORDER BY created_at ASC
LIMIT 100;
```

**可能的响应**

- 成功
```txt
Rows deleted: 5
```

- 成功 - 没有匹配的数据
```txt
Rows deleted: 0
```

- 失败 - 集合不存在
```txt
Error: Unknown collection 'collection_name'
```

## 七、数据查询

### 1. SELECT

**说明**

从集合中查询符合条件的数据记录。支持查询标量字段和向量字段。支持基本查询、条件过滤、排序、分组聚合等操作。

**语法**

```sql
SELECT <column_list> 
FROM <collection_name> 
[WHERE <condition>] 
[GROUP BY <column> [, <column> ...]]
[HAVING <condition>]
[ORDER BY <column> [ASC | DESC] [, <column> [ASC | DESC] ...]] 
[LIMIT <limit>];
```

**参数说明**

- `column_list`：要查询的字段列表，多个字段用逗号分隔
  - 支持查询标量字段（如 `id`、`name`、`price` 等）
  - 支持查询向量字段（如 `vector`）
  - 支持使用 `*` 查询所有字段
  - 支持使用字段别名（`AS alias`）
  - 支持聚合函数：
    - `COUNT(*)`：返回查询到的记录数
    - `COUNT(column)`：返回指定字段非空值的数量
    - `SUM(column)`：返回指定字段的总和（仅数值类型）
    - `AVG(column)`：返回指定字段的平均值（仅数值类型）
    - `MIN(column)`：返回指定字段的最小值
    - `MAX(column)`：返回指定字段的最大值
- `collection_name`：要查询的集合名称
- `WHERE condition`：可选，查询条件表达式。只有满足条件的记录才会被返回
  - 支持标量字段的条件判断（如 `id = 1`、`name = 'value'`、`price > 100`）
  - 支持比较运算符：`=`、`!=`、`<>`、`>`、`<`、`>=`、`<=`
  - 支持逻辑运算符：`AND`、`OR`、`NOT`
  - 支持 `IN`、`NOT IN`、`BETWEEN ... AND ...`、`LIKE` 等操作
  - 不支持向量字段的条件判断（向量相似度搜索需要通过 API 实现）
- `GROUP BY column [, column ...]`：可选，按指定字段分组。通常与聚合函数一起使用
  - 支持按单个字段分组（如 `GROUP BY category`）
  - 支持按多个字段分组（如 `GROUP BY category, status`）
  - 分组字段必须是标量字段，不能是向量字段
  - 使用 `GROUP BY` 时，`SELECT` 列表中的非聚合字段必须出现在 `GROUP BY` 子句中
- `HAVING condition`：可选，对分组后的结果进行过滤。与 `WHERE` 的区别是，`HAVING` 在分组后执行，可以包含聚合函数
  - 支持对聚合结果进行条件判断（如 `HAVING COUNT(*) > 10`、`HAVING AVG(price) > 1000`）
  - 支持标量字段的条件判断（如 `HAVING category = 'Electronics'`）
  - 不支持向量字段的条件判断
- `ORDER BY column [ASC | DESC] [, column [ASC | DESC] ...]`：可选，指定结果的排序顺序
  - `ASC`：升序（默认）
  - `DESC`：降序
  - 支持多字段排序，用逗号分隔。排序时先按第一个字段排序，如果第一个字段值相同，则按第二个字段排序，以此类推
  - 排序字段可以是标量字段或聚合函数结果
- `LIMIT limit`：可选，限制返回的最大行数
  - 适用于分页查询或限制结果集大小
  - 通常与 `ORDER BY` 配合使用，确保结果的确定性

**示例**

- 查询所有字段
```sql
SELECT * FROM products;
```

- 查询指定字段
```sql
SELECT id, name, price FROM products;
```

- 使用字段别名
```sql
SELECT id, name AS product_name, price AS product_price
FROM products
WHERE category = 'Electronics';
```

- 按条件查询
```sql
SELECT id, name, price
FROM products
WHERE category = 'Electronics' AND price > 1000;
```

- 使用 IN 查询
```sql
SELECT id, name, price
FROM products
WHERE category IN ('Electronics', 'Books', 'Clothing');
```

- 使用 BETWEEN 查询
```sql
SELECT id, name, price
FROM products
WHERE price BETWEEN 100 AND 500;
```

- 使用 LIKE 模糊查询
```sql
SELECT id, name
FROM products
WHERE name LIKE '%Product%';
```

- 排序查询
```sql
SELECT id, name, price
FROM products
WHERE category = 'Electronics'
ORDER BY price DESC;
```

- 多字段排序
```sql
SELECT id, name, price, created_at
FROM products
WHERE category = 'Electronics'
ORDER BY created_at ASC, id ASC;
```

- 限制返回数量
```sql
SELECT id, name, price
FROM products
WHERE category = 'Electronics'
ORDER BY price DESC
LIMIT 10;
```

- 查询向量字段
```sql
SELECT id, name, vector
FROM products
WHERE category = 'Electronics';
```

- 聚合函数查询
```sql
SELECT COUNT(*) FROM products WHERE category = 'Electronics';
```

- 多个聚合函数
```sql
SELECT 
    COUNT(*) as total,
    AVG(price) as avg_price,
    MAX(price) as max_price,
    MIN(price) as min_price
FROM products
WHERE category = 'Electronics';
```

- 分组聚合
```sql
SELECT category, COUNT(*) as count, AVG(price) as avg_price
FROM products
GROUP BY category;
```

- 多字段分组
```sql
SELECT category, status, COUNT(*) as count, SUM(price) as total_price
FROM products
WHERE price > 100
GROUP BY category, status;
```

- 分组聚合与排序
```sql
SELECT category, COUNT(*) as count, AVG(price) as avg_price
FROM products
GROUP BY category
ORDER BY count DESC;
```

- 分组聚合与条件过滤（HAVING）
```sql
SELECT category, COUNT(*) as count, AVG(price) as avg_price, MAX(price) as max_price
FROM products
WHERE created_at > '2024-01-01'
GROUP BY category
HAVING count > 10
ORDER BY avg_price DESC;
```

- 复杂聚合查询
```sql
SELECT 
    category,
    COUNT(*) as total_products,
    AVG(price) as avg_price,
    SUM(price) as total_value,
    MAX(price) as max_price,
    MIN(price) as min_price
FROM products
WHERE status = 'active'
GROUP BY category
ORDER BY total_products DESC
LIMIT 10;
```

**注意事项**

- 查询结果以表格形式返回，适合在终端显示
- 可以同时查询多个字段，字段之间用逗号分隔
- 支持使用字段别名（`AS alias`），别名可用于 `ORDER BY` 和 `HAVING` 子句
- **GROUP BY 使用规则**：
  - 使用 `GROUP BY` 时，`SELECT` 列表中的非聚合字段必须出现在 `GROUP BY` 子句中
  - 聚合函数（`COUNT`、`SUM`、`AVG` 等）可以与 `GROUP BY` 一起使用，对每个分组进行聚合计算
  - `GROUP BY` 可以按多个字段分组，分组时先按第一个字段分组，如果第一个字段值相同，则按第二个字段分组，以此类推
  - `WHERE` 子句在 `GROUP BY` 之前执行，用于过滤分组前的数据
  - `HAVING` 子句在 `GROUP BY` 之后执行，用于过滤分组后的结果，可以包含聚合函数
  - `ORDER BY` 子句在 `GROUP BY` 和 `HAVING` 之后执行，用于对分组结果进行排序
- 向量字段可以查询，但向量相似度搜索需要通过 API 实现
- 使用 `LIMIT` 时建议配合 `ORDER BY`，以确保结果的确定性

**可能的响应**

- 成功 - 查询到数据
```txt
+----+------------------+--------+
| id | name             | price  |
+----+------------------+--------+
| 1  | Product A        | 99.99  |
| 2  | Product B        | 199.99 |
| 3  | Product C        | 299.99 |
+----+------------------+--------+
3 rows in set (0.01 sec)
```

- 成功 - 没有匹配的数据
```txt
Empty set (0.00 sec)
```

- 成功 - 聚合查询结果
```txt
+------------+-------+-----------+
| category   | count | avg_price |
+------------+-------+-----------+
| Electronics|   150 |   599.99  |
| Clothing   |    80 |   199.99  |
| Books      |   200 |    29.99  |
+------------+-------+-----------+
3 rows in set (0.02 sec)
```

- 成功 - 分组聚合查询结果
```txt
+------------+--------+----------+-----------+-----------+
| category   | status | count    | avg_price | max_price |
+------------+--------+----------+-----------+-----------+
| Electronics| active |      120 |    699.99 |   1999.99 |
| Electronics| sold   |       30 |    499.99 |   1499.99 |
| Clothing   | active |       50 |    249.99 |    599.99 |
+------------+--------+----------+-----------+-----------+
3 rows in set (0.03 sec)
```

- 失败 - 集合不存在
```txt
Error: Unknown collection 'collection_name'
```

- 失败 - 未选择数据库
```txt
Error: No database selected
```

- 失败 - 字段不存在
```txt
Error: Unknown column 'column_name' in collection 'collection_name'
```

- 失败 - GROUP BY 错误：非聚合字段未出现在 GROUP BY 中
```txt
Error: Column 'name' must appear in GROUP BY clause or be used in an aggregate function
```

- 失败 - GROUP BY 中使用了向量字段
```txt
Error: Cannot use vector field 'vector' in GROUP BY clause
```
