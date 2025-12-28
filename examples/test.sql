-- 示例测试 SQL 文件
-- 这个文件包含了初始化一个简单集合的 SQL 语句

-- 创建数据库
CREATE DATABASE IF NOT EXISTS test_db;

-- 使用数据库
USE test_db;

-- 创建集合
CREATE COLLECTION IF NOT EXISTS test (
    id INTEGER NOT NULL PRIMARY KEY,
    name VARCHAR(100),
    age INTEGER
);

-- 插入测试数据
INSERT INTO test (id, name, age) VALUES (1, 'Alice', 25);
INSERT INTO test (id, name, age) VALUES (2, 'Bob', 30);
INSERT INTO test (id, name, age) VALUES (3, 'Charlie', 35);
