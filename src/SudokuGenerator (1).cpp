/**
 * @file SudokuGenerator.cpp
 * @brief 数独生成器实现 - 回溯算法 + 随机挖洞
 *
 * @author 张啸林
 * @date 2026-09-01
 */

#include "SudokuGenerator.h"
#include <QRandomGenerator>
#include <algorithm>
#include <vector>
#include <QTime>

// ---------------------------------------------------------------
// 构造函数
// ---------------------------------------------------------------
SudokuGenerator::SudokuGenerator()
    : m_difficulty(SudokuDifficulty::Easy)
{
    // 用当前时间戳初始化随机种子
    // Qt6: RNG auto-seeds
}

// ---------------------------------------------------------------
// 公开接口
// ---------------------------------------------------------------

bool SudokuGenerator::generateFullBoard()
{
    // 初始化 9x9 空棋盘（0 表示空格）
    m_solution = QVector<QVector<int>>(9, QVector<int>(9, 0));

    // 从左上角 (0,0) 开始回溯生成
    return solveBacktrack(m_solution, 0, 0);
}

bool SudokuGenerator::generatePuzzle(SudokuDifficulty difficulty)
{
    m_difficulty = difficulty;

    // Step 1: 先生成一个完整的合法解
    if (!generateFullBoard()) {
        return false;
    }

    // Step 2: 深拷贝完整解作为谜题基底
    m_puzzle = m_solution;

    // Step 3: 根据难度确定保留的已知格子数量，并挖洞
    int clues = 0;
    switch (difficulty) {
        case SudokuDifficulty::Easy:
            // Easy: 35~40 个格子（随机选一个中间值 37）
            clues = 35 + (QRandomGenerator::global()->bounded(6));  // 35~40
            break;
        case SudokuDifficulty::Medium:
            // Medium: 30~34 个格子
            clues = 30 + (QRandomGenerator::global()->bounded(5));  // 30~34
            break;
        case SudokuDifficulty::Hard:
            // Hard: 25~29 个格子
            clues = 25 + (QRandomGenerator::global()->bounded(5));  // 25~29
            break;
    }

    carvePuzzle(m_solution, clues);
    return true;
}

const QVector<QVector<int>> & SudokuGenerator::getSolution() const
{
    return m_solution;
}

const QVector<QVector<int>> & SudokuGenerator::getPuzzle() const
{
    return m_puzzle;
}

QString SudokuGenerator::getDifficulty() const
{
    switch (m_difficulty) {
        case SudokuDifficulty::Easy:   return QStringLiteral("Easy");
        case SudokuDifficulty::Medium: return QStringLiteral("Medium");
        case SudokuDifficulty::Hard:   return QStringLiteral("Hard");
        default:                        return QStringLiteral("Unknown");
    }
}

// ---------------------------------------------------------------
// 私有实现
// ---------------------------------------------------------------

/**
 * @brief 主回溯入口
 *
 * 数独按行优先填数：
 *   - 填满第 9 列时自动跳到下一行第 0 列
 *   - 填满最后格（第 9 行第 9 列）时表示成功
 *
 * @param board 棋盘（就地修改）
 * @param row   当前行
 * @param col   当前列
 * @return true 成功填完
 */
bool SudokuGenerator::solveBacktrack(QVector<QVector<int>> &board, int row, int col)
{
    // 递归终止条件：所有格子已填满
    if (row == 9) {
        return true;
    }

    // 计算下一个格子位置（先行后列）
    int nextRow = (col == 8) ? (row + 1) : row;
    int nextCol = (col == 8) ? 0 : (col + 1);

    // 如果当前格子已有数字（生成谜题后回填阶段），直接跳过
    if (board[row][col] != 0) {
        return solveBacktrack(board, nextRow, nextCol);
    }

    // 生成随机 1~9 的顺序尝试填入
    QVector<int> nums = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    shuffle(nums);

    for (int num : nums) {
        if (isValid(board, row, col, num)) {
            board[row][col] = num;
            if (solveBacktrack(board, nextRow, nextCol)) {
                return true;
            }
            // 回溯：清除并尝试下一个数字
            board[row][col] = 0;
        }
    }

    // 所有数字都无法填入，无解
    return false;
}

/**
 * @brief Fisher-Yates 洗牌算法（打乱 1~9 顺序，增加随机性）
 * @param arr 待打乱数组
 */
void SudokuGenerator::shuffle(QVector<int> &arr)
{
    int n = arr.size();
    for (int i = n - 1; i > 0; --i) {
        // 生成 [0, i] 范围内的随机索引
        int j = QRandomGenerator::global()->bounded(i + 1);
        qSwap(arr[i], arr[j]);
    }
}

/**
 * @brief 检查在 (row, col) 放入 num 是否满足数独约束
 *
 * 检查范围：
 *   1. 同一行无重复
 *   2. 同一列无重复
 *   3. 所在 3x3 宫格内无重复
 *
 * @param board 棋盘（只读）
 * @param row   行
 * @param col   列
 * @param num   数字 1~9
 * @return true 合法
 */
bool SudokuGenerator::isValid(const QVector<QVector<int>> &board,
                               int row, int col, int num) const
{
    // 检查同一行
    for (int c = 0; c < 9; ++c) {
        if (board[row][c] == num) return false;
    }

    // 检查同一列
    for (int r = 0; r < 9; ++r) {
        if (board[r][col] == num) return false;
    }

    // 检查所在 3x3 宫格
    int boxRowStart = (row / 3) * 3;
    int boxColStart = (col / 3) * 3;
    for (int r = boxRowStart; r < boxRowStart + 3; ++r) {
        for (int c = boxColStart; c < boxColStart + 3; ++c) {
            if (board[r][c] == num) return false;
        }
    }

    return true;
}

/**
 * @brief 从完整解中随机挖去若干格子，形成谜题
 *
 * 挖洞策略：随机选取格子置零，确保保留足够多的已知格子。
 * 为避免挖去格子导致多解，挖洞顺序随机化。
 *
 * @param solution 完整解（被原地修改为零）
 * @param clues    目标保留的已知格子数量
 */
void SudokuGenerator::carvePuzzle(const QVector<QVector<int>> &solution, int clues)
{
    // 将完整解拷贝到谜题中
    m_puzzle = solution;

    // 生成所有 81 个格子的索引列表
    QVector<QPair<int, int>> positions;
    for (int r = 0; r < 9; ++r) {
        for (int c = 0; c < 9; ++c) {
            positions.append(qMakePair(r, c));
        }
    }

    // 随机打乱挖洞顺序（用 std::shuffle）
    std::vector<std::pair<int,int>> tmp(positions.begin(), positions.end());
    std::shuffle(tmp.begin(), tmp.end(), *QRandomGenerator::global());
    for (int i = 0; i < (int)tmp.size(); ++i) positions[i] = tmp[i];

    // 需要挖去的格子数量 = 81 - clues
    int toRemove = 81 - clues;

    // 依次挖去格子
    for (int i = 0; i < toRemove && i < positions.size(); ++i) {
        int r = positions[i].first;
        int c = positions[i].second;
        m_puzzle[r][c] = 0;
    }

    // 同步更新 m_solution 为挖洞后的谜题（使 getSolution 返回完整解）
    // 注意：这里我们保留完整解不变，只是 m_puzzle 被挖洞
}
