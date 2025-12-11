# DreamDB SQL 语法规范

本文档定义了`DreamDB`向量数据库支持的`SQL`语法规范。作为向量数据库，`DreamDB`仅支持部分标准`SQL`语句，并对标准`SQL`进行了扩展。

## 概述

`DreamDB`是一个向量数据库，同时支持：
- 传统关系型数据：标量字段
- 向量数据：高维浮点向量

## 数据定义语言 DDL

### USE

#### USE DATABASE

**说明**

切换当前使用的数据库，后续操作默认在该数据库中进行

**语法**

```sql
USE <database_name>;
```

**参数说明**

- `database_name`：要切换的数据库名称

**响应**

- 成功
```txt
Database changed
```

- 失败
```txt
Error: Unknown database 'database_name'
```

### CREATE

#### CREATE DATABASE

**说明**

创建新的数据库，数据库名称必须唯一

**语法**

```sql
CREATE DATABASE <database_name>;
```

**参数说明**

- `database_name`：要创建的数据库名称

**响应**

- 成功
```txt
Query OK, 1 row affected (0.00 sec)
```

- 失败 - 数据库已存在
```txt
Error: Database 'database_name' already exists
```

#### CREATE COLLECTION

**说明**

创建新的集合，集合名必须唯一。作为向量数据库，集合拥有特殊的构造，它默认强制包含`__id`和`__vector`字段：

```sql
...
    __id CHAR(n) NOT NULL,
    __vector VECTOR(m) NOT NULL
...
```

其中，`__id`字段是长度为`n`的定长字符串；`__vector`是`m`维的向量。这两个字段为关键字段，不允许再次声明。

**字段类型**

- `TINYINT`: 8位整数
- `SMALLINT`: 16位整数
- `INTEGER`: 32位整数
- `BIGINT`: 64位整数
- `FLOAT`: 单精度浮点数
- `DOUBLE`: 双精度浮点数
- `DECIMAL(p, s)`: 定点数，精度为`p`，小数位数为`s`
- `CHAR(n)`: 定长字符串，长度为`n`
- `VARCHAR(n)`: 变长字符串，最大长度为`n`
- `BOOLEAN`: 布尔值，取值为`TRUE`或`FALSE`
- `TIMESTAMP`: 时间戳，取值为时间戳
- `ENUM(value1, value2, ...)`: 枚举类型，取值为`value1`、`value2`、...
- `VECTOR(n)`: 向量，维度为`n`，该类型暂只用于内置`__vector`字段

**列约束**

- `PRIMARY KEY`：主键约束
- `NOT NULL`：非空约束
- `AUTO_INCREMENT`：自增（仅限整数类型）
- `DEFAULT value`：默认值
- `COMMENT 'text'`：列注释

**语法**

```sql
CREATE COLLECTION <collection_name> (
    <column_name> <column_type> [<constraints>] [COMMENT '<comment>'],
    ...
) [WITH (
    id_len = <length>,
    vector_dim = <dimension>
)];
```

**参数说明**

- `collection_name`：要创建的集合名称
- `column_name`：要创建的字段名称
- `column_type`：字段类型
- `constraints`：字段约束
- `COMMENT '<comment>'`：字段注释
- `id_len = <length>`：`__id`字段长度，没有默认值，必须指定，不可修改
- `vector_dim = <dimension>`：`__vector`字段维度，没有默认值，必须指定，不可修改

**响应**

- 成功
```txt
Query OK, 0 row affected (0.01 sec)
```

- 失败 - 集合已存在
```txt
Error: Collection 'collection_name' already exists
```

#### CREATE INDEX

**说明**

用于创建标量字段索引，以提升标量数据查询效率。标量索引遵循标准`SQL`语法，支持对集合中的标量字段（非向量字段）建立索引，加速等值查询、范围查询和排序操作。

**索引类型**

- `BTREE`（默认）: `B-Tree`索引，支持等值查询、范围查询和排序
- `HASH`: `Hash`索引，仅支持等值查询，查询速度更快但功能受限

**语法**

```sql
CREATE [UNIQUE] INDEX <index_name>
ON <collection_name> (<column_name>)
[USING BTREE | HASH];
```

**参数说明**

- `index_name`：索引名称，必须唯一
- `collection_name`：集合名称
- `column_name`：要建立索引的标量字段名（不能是`__vector`字段）
- `UNIQUE`：可选，创建唯一索引，确保索引字段的值唯一
- `USING BTREE | HASH`：可选，指定索引类型，默认为`BTREE`

**支持的查询类型**

- **等值查询**：`WHERE column = value`
- **不等查询**：`WHERE column != value`
- **范围查询**：`WHERE column > value`、`WHERE column < value`、`WHERE column BETWEEN min AND max`
- **IN 查询**：`WHERE column IN (value1, value2, ...)`
- **排序**：`ORDER BY column`

**注意**

- `Hash`索引仅支持等值查询（`=` 和 `IN`），不支持范围查询和排序
- 唯一索引（`UNIQUE`）会自动检查字段值的唯一性
- 向量字段 `__vector` 不支持标量索引，请使用 `CREATE VINDEX` 创建向量索引

**示例**

- **基本索引（B-Tree）**
```sql
CREATE INDEX index_name ON collection_name (column_name);
```

- **Hash 索引**
```sql
CREATE INDEX index_name ON documents (column_name) USING HASH;
```

- **唯一索引**
```sql
CREATE UNIQUE INDEX index_name ON users (column_name);
```

**响应**

- 成功
```txt
Query OK, 0 row affected (0.01 sec)
```

- 失败 - 索引已存在
```txt
Error: Index 'index_name' already exists
```

- 失败 - 字段不存在
```txt
Error: Unknown column 'column_name' in collection 'collection_name'
```

- 失败 - 不能对向量字段创建标量索引
```txt
Error: Cannot create scalar index on vector field '__vector', use CREATE VINDEX instead
```

#### CREATE VINDEX

**说明**

用于创建向量索引，以提升数据查询效率。该语句为扩展语法，非标准`SQL`语法。

**索引类型**

- `FLAT`: 暴力搜索
- `IVF_FLAT`: 倒排文件索引
- `HNSW`: 分层导航小世界图

**距离度量**

- `L2`: 欧氏距离
- `IP`: 内积
- `COSINE`: 余弦相似度

**示例**

```sql
CREATE VINDEX <vindex_name>
ON <collection_name>
USING [FLAT | IVF_FLAT | HNSW]
WITH (
    metric = ['L2' | 'IP' | 'COSINE'],
    [index_specific_params...]
);
```

**参数说明**

- `vindex_name`：向量索引名称，必须唯一
- `collection_name`：集合名称
- `USING`：指定索引类型，必须为 `FLAT`、`IVF_FLAT` 或 `HNSW` 之一
- `metric`：距离度量类型，必须为 `L2`、`IP` 或 `COSINE` 之一
- `index_specific_params`：索引特定参数，不同索引类型需要不同的参数

**注意**

- 索引自动基于集合的 `__vector` 内置字段建立，无需显式指定字段名
- 每个集合只能创建一个向量索引，创建新的会自动替换旧的
- 不同索引类型需要不同的参数，详见下方示例

**示例**

以下给出不同索引类型的具体示例：

- `FLAT`
```sql
CREATE VINDEX vindex_name
ON collection_name
USING FLAT
WITH (metric = 'L2');
```

- `IVF_FLAT`
```sql
CREATE VINDEX vindex_name
ON collection_name
USING IVF_FLAT
WITH (
    metric = 'L2',
    nlist = 1024
);
```

其中，`nlist`参数代表聚类中心的数量，即倒排列表的数量。该参数影响索引构建时的聚类精度和内存占用。查询时的搜索簇数量由`nprobe`参数控制，该参数于查询时指定。

- `HNSW`
```sql
CREATE VINDEX vindex_name
ON collection_name
USING HNSW
WITH (
    metric = 'L2',
    M = 16,
    ef_construction = 200
);
```

其中，`M`参数代表每个节点在图中连接的最大邻居数，用于控制图的连通性和搜索路径；`ef_construction`参数代表构建索引时动态候选列表的大小，用于控制构建时的搜索范围，影响索引质量。

**响应**

- 成功
```txt
Query OK, 0 row affected (0.01 sec)
```

- 失败 - 向量索引已存在
```txt
Error: Vindex 'vindex_name' already exists
```

### DROP

#### DROP DATABASE

**说明**

删除数据库，数据库名称必须唯一

**语法**

```sql
DROP DATABASE <database_name>;
```

**响应**

- 成功
```txt
Query OK, 0 row affected (0.01 sec)
```

- 失败 - 数据库不存在
```txt
Error: Unknown database 'database_name'
```

#### DROP COLLECTION

**说明**

删除集合，集合名称必须唯一

**语法**

```sql
DROP COLLECTION <collection_name>;
```

**响应**

- 成功
```txt
Query OK, 0 row affected (0.01 sec)
```

- 失败 - 集合不存在
```txt
Error: Unknown collection 'collection_name'
```

#### DROP INDEX

**说明**

删除索引，索引名称必须唯一

**语法**

```sql
DROP INDEX <index_name>;
```

**响应**

- 成功
```txt
Query OK, 0 row affected (0.01 sec)
```

- 失败 - 索引不存在
```txt
Error: Unknown index 'index_name'
```

#### DROP VINDEX

**说明**

删除向量索引，索引名称必须唯一

**语法**

```sql
DROP VINDEX <vindex_name>;
```

**响应**

- 成功
```txt
Query OK, 0 row affected (0.01 sec)
```

- 失败 - 索引不存在
```txt
Error: Unknown vindex 'vindex_name'
```

## 数据操作语言 DML

### INSERT

**说明**

由于向量字段通常为高维度，难以通过`SQL`语句直接写出，因此当前数据库实现不允许直接插入数据，需要通过`API`进行插入。

### DELETE

#### DELETE FROM

**说明**

从集合中删除符合条件的数据记录。

**语法**

```sql
DELETE FROM <collection_name> [WHERE <condition>] [ORDER BY <column> [ASC | DESC]] [LIMIT <limit>];
```

**参数说明**

- `collection_name`：要删除数据的集合名称
- `WHERE condition`：可选，删除条件表达式。只有满足条件的记录才会被删除
  - 支持标量字段的条件判断（如 `id = 1`、`name = 'value'`）
  - 不支持向量字段的条件判断，如需使用向量字段，请使用`API`进行删除
- `ORDER BY column [ASC | DESC]`：可选，指定删除的排序顺序。当使用`LIMIT`时，建议使用`ORDER BY`以确保删除顺序的可预测性
  - `ASC`：升序（默认）
  - `DESC`：降序
- `LIMIT limit`：可选，限制删除的最大行数。如果不指定，将删除所有匹配的记录

**注意**

- 如果不指定`WHERE`子句，将删除集合中的所有数据，操作不可逆
- `WHERE`条件只能基于标量字段，不能基于向量字段
- 当使用`LIMIT`时，如果没有`ORDER BY`，删除的行是不确定的
- 当需要按特定顺序删除数据时，必须同时使用`ORDER BY`和`LIMIT`。`ORDER BY`确保删除顺序的可预测性，`LIMIT`限制删除的数量

**示例**

- **按主键删除**
```sql
DELETE FROM products WHERE id = 123;
```

- **按标量字段删除**
```sql
DELETE FROM products WHERE category = 'Electronics';
```

- **多条件删除**
```sql
DELETE FROM products WHERE category = 'Electronics' AND price > 1000;
```

- **限制删除数量**
```sql
DELETE FROM products WHERE status = 'obsolete' LIMIT 100;
```

- **按顺序删除（ORDER BY + LIMIT）**
```sql
DELETE FROM products 
WHERE status = 'obsolete' 
ORDER BY created_at ASC 
LIMIT 10;
```

- **删除最旧的记录（多字段排序）**
```sql
DELETE FROM products 
WHERE category = 'Electronics' 
ORDER BY created_at ASC, id ASC 
LIMIT 5;
```

- **删除最新的记录（降序）**
```sql
DELETE FROM products 
WHERE price < 100 
ORDER BY created_at DESC 
LIMIT 20;
```

- **删除所有数据（危险操作）**
```sql
DELETE FROM products;
```

**响应**

- 成功 - 删除了数据
```txt
Query OK, 5 rows affected (0.01 sec)
```

- 成功 - 没有匹配的数据
```txt
Query OK, 0 rows affected (0.00 sec)
```

- 失败 - 集合不存在
```txt
Error: Unknown collection 'collection_name'
```

- 失败 - 未选择数据库
```txt
Error: No database selected
```

- 失败 - WHERE 条件中使用了向量字段
```txt
Error: Cannot use '__vector' field in WHERE clause, use API for vector-based operations
```

### UPDATE

#### UPDATE SET

**说明**

更新集合中符合条件的数据记录的标量字段值。不支持修改`__id`和`__vector`字段，这两个字段需要通过`API`进行修改。

**语法**

```sql
UPDATE <collection_name> 
SET <column_name> = <value> [, <column_name> = <value> ...]
[WHERE <condition>]
[ORDER BY <column> [ASC | DESC]]
[LIMIT <limit>];
```

**参数说明**

- `collection_name`：要更新数据的集合名称
- `SET column_name = value`：指定要更新的字段和新值。可以同时更新多个字段，用逗号分隔
  - 支持更新标量字段（如 `name = 'new_value'`、`price = 100`）
  - 不允许更新`__id`和`__vector`字段，这两个字段需要通过`API`进行修改
- `WHERE condition`：可选，更新条件表达式。只有满足条件的记录才会被更新
  - 支持标量字段的条件判断（如 `id = 1`、`name = 'value'`）
  - 不支持向量字段的条件判断，如需使用向量字段，请使用`API`进行查询和更新
- `ORDER BY column [ASC | DESC]`：可选，指定更新的排序顺序。当使用`LIMIT`时，建议使用`ORDER BY`以确保更新顺序的可预测性
  - `ASC`：升序（默认）
  - `DESC`：降序
- `LIMIT limit`：可选，限制更新的最大行数。如果不指定，将更新所有匹配的记录

**示例**

- **按主键更新单个字段**
```sql
UPDATE products 
SET price = 99.99 
WHERE id = 123;
```

- **更新多个字段**
```sql
UPDATE products 
SET name = 'New Product', price = 199.99, category = 'Electronics' 
WHERE id = 123;
```

- **按条件批量更新**
```sql
UPDATE products 
SET status = 'discontinued', discount = 0.5 
WHERE category = 'Old Electronics';
```

- **更新所有数据（危险操作）**
```sql
UPDATE products 
SET status = 'active';
```

- **使用表达式更新**
```sql
UPDATE products 
SET price = price * 0.9 
WHERE category = 'Electronics' AND price > 1000;
```

**响应**

- 成功 - 更新了数据
```txt
Query OK, 5 rows affected (0.01 sec)
```

- 成功 - 没有匹配的数据
```txt
Query OK, 0 rows affected (0.00 sec)
```

- 失败 - 集合不存在
```txt
Error: Unknown collection 'collection_name'
```

- 失败 - 未选择数据库
```txt
Error: No database selected
```

- 失败 - 尝试更新`__id`字段
```txt
Error: Cannot update '__id' field, use API for id modification
```

- 失败 - 尝试更新`__vector`字段
```txt
Error: Cannot update '__vector' field, use API for vector modification
```

- 失败 - WHERE 条件中使用了向量字段
```txt
Error: Cannot use '__vector' field in WHERE clause, use API for vector-based operations
```

## 数据查询语言 DQL

### SELECT

#### SELECT FROM

**说明**

从集合中查询符合条件的数据记录。仅支持查询标量字段，不支持查询`__vector`字段（向量数据需要通过`API`进行查询和相似度搜索）。

**语法**

```sql
SELECT <column_list> 
FROM <collection_name> 
[WHERE <condition>] 
[GROUP BY <column> [, <column> ...]]
[HAVING <condition>]
[ORDER BY <column> [ASC | DESC]] 
[LIMIT <limit>];
```

**参数说明**

- `column_list`：要查询的字段列表，多个字段用逗号分隔
  - 支持查询标量字段（如 `id`、`name`、`price`）
  - 支持查询`__id`字段（如 `__id`）
  - 支持聚合函数
    - `COUNT(*)`: 返回查询到的记录数
    - `SUM(column)`: 返回指定字段的总和
    - `AVG(column)`: 返回指定字段的平均值
    - `MIN(column)`: 返回指定字段的最小值
    - `MAX(column)`: 返回指定字段的最大值
  - 支持使用`*`查询所有标量字段（但不返回`__vector`字段）
  - 不支持查询`__vector`字段，向量数据需要通过`API`进行查询
- `collection_name`：要查询的集合名称
- `WHERE condition`：可选，查询条件表达式。只有满足条件的记录才会被返回
  - 支持标量字段的条件判断（如 `id = 1`、`name = 'value'`、`price > 100`）
  - 不支持向量字段的条件判断，如需使用向量字段，请使用`API`进行向量相似度搜索
- `GROUP BY column [, column ...]`：可选，按指定字段分组。通常与聚合函数一起使用
  - 支持按单个字段分组（如 `GROUP BY category`）
  - 支持按多个字段分组（如 `GROUP BY category, status`）
  - 分组字段必须是标量字段，不能是向量字段
  - 使用`GROUP BY`时，`SELECT`列表中的非聚合字段必须出现在`GROUP BY`子句中
- `HAVING condition`：可选，对分组后的结果进行过滤。与`WHERE`的区别是，`HAVING`在分组后执行，可以包含聚合函数
  - 支持对聚合结果进行条件判断（如 `HAVING COUNT(*) > 10`、`HAVING AVG(price) > 1000`）
  - 支持标量字段的条件判断（如 `HAVING category = 'Electronics'`）
  - 不支持向量字段的条件判断
- `ORDER BY column [ASC | DESC]`：可选，指定结果的排序顺序
  - `ASC`：升序（默认）
  - `DESC`：降序
  - 支持多字段排序，用逗号分隔。排序时先按第一个字段排序，如果第一个字段值相同，则按第二个字段排序，以此类推
- `LIMIT limit`：可选，限制返回的最大行数

**注意**

- 查询结果以表格形式返回，适合在终端显示
- 可以同时查询多个字段，字段之间用逗号分隔
- 支持使用字段别名（`AS alias`）
- **GROUP BY 使用规则**：
  - 使用`GROUP BY`时，`SELECT`列表中的非聚合字段必须出现在`GROUP BY`子句中
  - 聚合函数（`COUNT`、`SUM`、`AVG`等）可以与`GROUP BY`一起使用，对每个分组进行聚合计算
  - `GROUP BY`可以按多个字段分组，分组时先按第一个字段分组，如果第一个字段值相同，则按第二个字段分组，以此类推
  - `WHERE`子句在`GROUP BY`之前执行，用于过滤分组前的数据
  - `HAVING`子句在`GROUP BY`之后执行，用于过滤分组后的结果，可以包含聚合函数
  - `ORDER BY`子句在`GROUP BY`和`HAVING`之后执行，用于对分组结果进行排序

**示例**

- **查询所有标量字段**
```sql
SELECT * FROM products;
```

- **查询指定字段**
```sql
SELECT id, name, price FROM products;
```

- **按条件查询**
```sql
SELECT id, name, price
FROM products
WHERE category = 'Electronics' AND price > 1000;
```

- **排序查询**
```sql
SELECT id, name, price
FROM products
WHERE category = 'Electronics'
ORDER BY price DESC;
```

- **多字段排序**
```sql
SELECT id, name, price, created_at
FROM products
WHERE category = 'Electronics'
ORDER BY created_at ASC, id ASC;
```

- **限制返回数量**
```sql
SELECT id, name, price
FROM products
WHERE category = 'Electronics'
ORDER BY price DESC
LIMIT 10;
```

- **使用字段别名**
```sql
SELECT id, name AS product_name, price AS product_price
FROM products
WHERE category = 'Electronics';
```

- **聚合函数查询**
```sql
SELECT COUNT(*) FROM products WHERE category = 'Electronics';
```

- **分组聚合**
```sql
SELECT category, COUNT(*) as count, AVG(price) as avg_price
FROM products
GROUP BY category;
```

- **多字段分组**
```sql
SELECT category, status, COUNT(*) as count, SUM(price) as total_price
FROM products
WHERE price > 100
GROUP BY category, status;
```

- **分组聚合与排序**
```sql
SELECT category, COUNT(*) as count, AVG(price) as avg_price
FROM products
GROUP BY category
ORDER BY count DESC;
```

- **分组聚合与条件过滤（HAVING）**
```sql
SELECT category, COUNT(*) as count, AVG(price) as avg_price, MAX(price) as max_price
FROM products
WHERE created_at > '2024-01-01'
GROUP BY category
HAVING count > 10
ORDER BY avg_price DESC;
```

- **复杂聚合查询**
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

**响应**

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

- 失败 - 尝试查询`__vector`字段
```txt
Error: Cannot select '__vector' field, use API for vector queries
```

- 失败 - WHERE 条件中使用了向量字段
```txt
Error: Cannot use '__vector' field in WHERE clause, use API for vector-based operations
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
Error: Cannot use '__vector' field in GROUP BY clause
```

## 元数据查询语言 MQL

### SHOW

#### SHOW DATABASES

**说明**

显示所有数据库

**语法**

```sql
SHOW DATABASES;
```

**响应**

- 成功
```txt
+--------------------+
| Database           |
+--------------------+
| information_schema |
| dreamdb            |
+--------------------+
2 rows in set (0.00 sec)
```

#### SHOW COLLECTIONS

**说明**

显示当前数据库中的所有集合

**语法**

```sql
SHOW COLLECTIONS;
```

**响应**

- 成功
```txt
+--------------------+
| Collection         |
+--------------------+
| products           |
| users              |
+--------------------+
2 rows in set (0.00 sec)
```

- 失败 - 未选择数据库
```txt
Error: No database selected
```

#### SHOW INDEXES

**说明**

显示当前数据库中的所有索引

**语法**

```sql
SHOW INDEXES FROM <collection_name>;
```

**参数说明**

- `collection_name`：要显示的集合名称

**注意**

- 需要先选择数据库，才能显示索引

**响应**

- 成功
```txt
+--------------------+
| Index              |
+--------------------+
| index_name         |
| index_name_2       |
+--------------------+
2 rows in set (0.00 sec)
```

- 失败 - 集合不存在
```txt
Error: Unknown collection 'collection_name'
```

- 失败 - 未选择数据库
```txt
Error: No database selected
```

#### SHOW VINDEXES

**说明**

显示当前集合中的所有向量索引

**语法**

```sql
SHOW VINDEXES FROM <collection_name>;
```

**参数说明**

- `collection_name`：要显示的集合名称

**注意**

- 需要先选择数据库，才能显示向量索引

**响应**

- 成功
```txt
+--------------------+
| VIndex             |
+--------------------+
| vindex_name        |
| vindex_name_2      |
+--------------------+
2 rows in set (0.00 sec)
```

- 失败 - 集合不存在
```txt
Error: Unknown collection 'collection_name'
```

- 失败 - 未选择数据库
```txt
Error: No database selected
```

### DESC

#### DESC COLLECTION

**说明**

显示集合的详细信息

**语法**

```sql
DESC COLLECTION <collection_name>;
```

**参数说明**

- `collection_name`：要显示的集合名称

**响应**

- 成功
```txt
+-------------+-------------+------+-----+---------+---------+
| Field       | Type        | Null | Key | Default | Comment |
+-------------+-------------+------+-----+---------+---------+
| __id        | CHAR(64)    | NO   | PRI | NULL    |         |
| __vector    | VECTOR(128) | NO   |     | NULL    |         |
| name        | VARCHAR(255)| YES  |     | NULL    |         |
| price       | DOUBLE      | YES  |     | NULL    |         |
| category    | VARCHAR(50) | YES  |     | NULL    |         |
+-------------+-------------+------+-----+---------+---------+
5 rows in set (0.00 sec)
```

- 失败 - 集合不存在
```txt
Error: Unknown collection 'collection_name'
```
