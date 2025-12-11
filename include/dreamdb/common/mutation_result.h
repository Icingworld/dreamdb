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
    MutationResult() = default;

    MutationResult(const MutationResult &) = default;

    MutationResult(MutationResult &&) noexcept = default;

    MutationResult & operator=(const MutationResult &) = default;

    MutationResult & operator=(MutationResult &&) noexcept = default;

    ~MutationResult() = default;

public:
    /**
     * @brief 创建成功结果
     * @param affected_count 受影响的行数
     * @return 成功的结果对象
     */
    static MutationResult make_success(std::size_t affected_count = 1);

    /**
     * @brief 创建失败结果
     * @param error_message 错误信息
     * @return 失败的结果对象
     */
    static MutationResult make_failure(const std::string & error_message);

public:
    /**
     * @brief 设置操作成功状态
     * @param success 是否成功
     */
    void set_success(bool success) noexcept;

    /**
     * @brief 设置错误信息
     * @param message 错误信息
     */
    void set_error_message(const std::string & message);

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
     * @brief 获取错误信息
     * @return 错误信息，如果成功则为空字符串
     */
    const std::string & get_error_message() const noexcept;

    /**
     * @brief 获取受影响的行数
     * @return 受影响的行数
     */
    std::size_t get_affected_count() const noexcept;

private:
    bool success_ = false;                    // 操作是否成功
    std::string error_message_;               // 错误信息
    std::size_t affected_count_ = 0;          // 受影响的记录数
};

} // namespace dreamdb
