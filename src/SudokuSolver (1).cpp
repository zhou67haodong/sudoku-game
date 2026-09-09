/**
 * @file SudokuSolver.cpp
 * @brief 数独求解器实现 - 回溯 + 约束传播，支持逐步求解与提示
 *
 * @author 张啸林
 * @date 2026-09-01
 */

#include "SudokuSolver.h"
#include <algorithm>
#include <cstdlib>

/**
 * @brief 构造函数，初始化内部状态
 */
SudokuSolver::SudokuSolver()
    : m_lastRow(-1), m_lastCol(-1)
{
    reset();
}

/**
 * @brief 递归回溯求解核心
 */
bool SudokuSolver::solveRecursive(QVector<QVector<int>> &board)
{
    int row = 0, col = 0;
    // 找到一个空格
    if (!findNextEmpty(board, row, col)) {
        return true; // 没有空格，说明已填满
    }
    // 随机尝试 1~9，增加求解的多样性
    QVector<int> nums = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    std::random_shuffle(nums.begin(), nums.end());
    for (int num : nums) {
        if (isValid(board, row, col, num)) {
            board[row][col] = num;
            if (solveRecursive(board)) {
                return true;
            }
            board[row][col] = 0; // 回溯
        }
    }
    return false;
}

/**
 * @brief 统计解数量内部递归
 */
int SudokuSolver::countRecursive(const QVector<QVector<int>> &board,
                                 int count, int limit)
{
    // 复制棋盘（本函数对 board 只读，但需要在副本上放置数字）
    QVector<QVector<int>> b = board;
    int row = 0, col = 0;
    if (!findNextEmpty(b, row, col)) {
        return count + 1; // 找到一个完整解
    }
    for (int num = 1; num <= 9; ++num) {
        if (isValid(b, row, col, num)) {
            b[row][col] = num;
            count = countRecursive(b, count, limit);
            if (count >= limit) {
                return count; // 已达上限，提前结束
            }
        }
    }
    return count;
}

/**
 * @brief 找到下一个空格（行优先）
 */
bool SudokuSolver::findNextEmpty(const QVector<QVector<int>> &board,
                                 int &outRow, int &outCol) const
{
    for (int r = 0; r < 9; ++r) {
        for (int c = 0; c < 9; ++c) {
            if (board[r][c] == 0) {
                outRow = r;
                outCol = c;
                return true;
            }
        }
    }
    return false;
}

/**
 * @brief 完整求解
 */
bool SudokuSolver::solve(QVector<QVector<int>> &board)
{
    return solveRecursive(board);
}

/**
 * @brief 判断 (row, col) 放 num 是否合法
 */
bool SudokuSolver::isValid(const QVector<QVector<int>> &board,
                           int row, int col, int num) const
{
    // 行检查
    for (int c = 0; c < 9; ++c) {
        if (board[row][c] == num) return false;
    }
    // 列检查
    for (int r = 0; r < 9; ++r) {
        if (board[r][col] == num) return false;
    }
    // 3x3 宫检查
    int boxRow = (row / 3) * 3;
    int boxCol = (col / 3) * 3;
    for (int r = 0; r < 3; ++r) {
        for (int c = 0; c < 3; ++c) {
            if (board[boxRow + r][boxCol + c] == num) return false;
        }
    }
    return true;
}

/**
 * @brief 判断是否已完整且正确
 */
bool SudokuSolver::isSolved(const QVector<QVector<int>> &board) const
{
    // 检查是否有空格
    for (int r = 0; r < 9; ++r) {
        for (int c = 0; c < 9; ++c) {
            if (board[r][c] == 0) return false;
        }
    }
    // 检查每一行、列、宫合法性
    for (int i = 0; i < 9; ++i) {
        bool rowSeen[10] = {false};
        bool colSeen[10] = {false};
        bool boxSeen[10] = {false};
        for (int j = 0; j < 9; ++j) {
            int rv = board[i][j];
            int cv = board[j][i];
            int br = (i / 3) * 3 + j / 3;
            int bc = (i % 3) * 3 + j % 3;
            int bv = board[br][bc];
            if (rv < 1 || rv > 9 || rowSeen[rv]) return false;
            if (cv < 1 || cv > 9 || colSeen[cv]) return false;
            if (bv < 1 || bv > 9 || boxSeen[bv]) return false;
            rowSeen[rv] = colSeen[cv] = boxSeen[bv] = true;
        }
    }
    return true;
}

/**
 * @brief 获取一个空格的合法提示
 */
bool SudokuSolver::getHint(const QVector<QVector<int>> &board,
                           int &outRow, int &outCol, int &outNum)
{
    // 复制棋盘并求解，得到正确数字
    QVector<QVector<int>> b = board;
    if (!solve(b)) return false;
    // 找到一个与当前棋盘不同的空格
    for (int r = 0; r < 9; ++r) {
        for (int c = 0; c < 9; ++c) {
            if (board[r][c] == 0 && b[r][c] != 0) {
                outRow = r;
                outCol = c;
                outNum = b[r][c];
                return true;
            }
        }
    }
    return false;
}

/**
 * @brief 统计解数量
 */
int SudokuSolver::countSolutions(const QVector<QVector<int>> &board)
{
    return countRecursive(board, 0, 2);
}

/**
 * @brief 单步求解（动画演示用）
 */
bool SudokuSolver::solveStep(QVector<QVector<int>> &board)
{
    int row = 0, col = 0;
    if (m_history.isEmpty()) {
        // 第一次调用：找一个空格
        if (!findNextEmpty(board, row, col)) return false;
        m_lastRow = row;
        m_lastCol = col;
    } else {
        row = m_lastRow;
        col = m_lastCol;
        // 若当前格已有值但不合法，先清空再尝试
        if (board[row][col] != 0 && !isValid(board, row, col, board[row][col])) {
            board[row][col] = 0;
        }
    }

    // 从当前值+1 开始尝试
    int start = (board[row][col] == 0) ? 1 : board[row][col] + 1;
    for (int num = start; num <= 9; ++num) {
        if (isValid(board, row, col, num)) {
            board[row][col] = num;
            m_history.append(qMakePair(row, col));
            m_lastRow = row;
            m_lastCol = col;
            return true;
        }
    }
    // 当前格无可行数字 → 回溯到上一步
    if (!m_history.isEmpty()) {
        QPair<int, int> last = m_history.takeLast();
        board[last.first][last.second] = 0;
        m_lastRow = last.first;
        m_lastCol = last.second;
        return true;
    }
    return false;
}

/**
 * @brief 重置内部状态
 */
void SudokuSolver::reset()
{
    m_history.clear();
    m_lastRow = -1;
    m_lastCol = -1;
}
