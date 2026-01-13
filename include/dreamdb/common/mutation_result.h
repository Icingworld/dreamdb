#pragma once

#include <string>

namespace dreamdb
{

/**
 * @brief 操作结果类
 */
class MutationResult
{
public:
    explicit MutationResult();

    MutationResult(const MutationResult &) = default;

    MutationResult(MutationResult &&) noexcept = default;

    MutationResult & operator=(const MutationResult &) = default;

    MutationResult & operator=(MutationResult &&) noexcept = default;

    ~MutationResult() = default;

public:
    /**
     * @brief 创建成功结果
     * @return 成功的结果对象
     */
    static MutationResult make_success();

    /**
     * @brief 创建失败结果
     * @param message 消息
     * @return 失败的结果对象
     */
    static MutationResult make_failure(const std::string & message);

public:
    /**
     * @brief 设置操作成功状态
     * @param success 是否成功
     */
    void set_success(bool success) noexcept;

    /**
     * @brief 设置消息
     * @param message 消息
     */
    void set_message(const std::string & message);

    /**
     * @brief 设置受影响的行数
     * @param count 受影响的行数
     */
    void set_affected_count(std::size_t count) noexcept;

    /**
     * @brief 检查操作是否成功
     * @return 如果成功返回 true
     */
    bool is_success() const noexcept;

    /**
     * @brief 获取消息
     * @return 消息
     */
    const std::string & get_message() const noexcept;

    /**
     * @brief 获取受影响的行数
     * @return 受影响的行数
     */
    std::size_t get_affected_count() const noexcept;

private:
    bool success_;                       // 操作是否成功
    std::string message_;                // 消息
    std::size_t affected_count_;         // 受影响的记录数
};

} // namespace dreamdb
