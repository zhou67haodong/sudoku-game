/**
 * @file SudokuValidator.cpp
 * @brief 数独输入校验器实现
 *
 * @author 张啸林
 * @date 2026-09-01
 */

#include "SudokuValidator.h"

/**
 * @brief 构造函数
 */
SudokuValidator::SudokuValidator()
{
}

/**
 * @brief 校验整个棋盘
 *
 * 依次检查每一行、每一列、每一个 3x3 宫，
 * 任何位置出现重复数字即记为冲突。相同冲突格子只记录一次。
 */
bool SudokuValidator::validate(const QVector<QVector<int>> &board)
{
    m_errors.clear();
    bool allValid = true;

    // 行校验
    for (int r = 0; r < 9; ++r) {
        if (!isRowValid(board, r)) {
            allValid = false;
            for (int c = 0; c < 9; ++c) {
                if (board[r][c] != 0) {
                    m_errors.append(qMakePair(r, c));
                }
            }
        }
    }
    // 列校验
    for (int c = 0; c < 9; ++c) {
        if (!isColValid(board, c)) {
            allValid = false;
            for (int r = 0; r < 9; ++r) {
                if (board[r][c] != 0) {
                    m_errors.append(qMakePair(r, c));
                }
            }
        }
    }
    // 宫校验
    for (int br = 0; br < 9; br += 3) {
        for (int bc = 0; bc < 9; bc += 3) {
            if (!isBoxValid(board, br, bc)) {
                allValid = false;
                for (int r = 0; r < 3; ++r) {
                    for (int c = 0; c < 3; ++c) {
                        int rr = br + r, cc = bc + c;
                        if (board[rr][cc] != 0) {
                            m_errors.append(qMakePair(rr, cc));
                        }
                    }
                }
            }
        }
    }
    return allValid;
}

/**
 * @brief 行合法性检查
 */
bool SudokuValidator::isRowValid(const QVector<QVector<int>> &board, int row) const
{
    bool seen[10] = {false};
    for (int c = 0; c < 9; ++c) {
        int v = board[row][c];
        if (v != 0) {
            if (seen[v]) return false;
            seen[v] = true;
        }
    }
    return true;
}

/**
 * @brief 列合法性检查
 */
bool SudokuValidator::isColValid(const QVector<QVector<int>> &board, int col) const
{
    bool seen[10] = {false};
    for (int r = 0; r < 9; ++r) {
        int v = board[r][col];
        if (v != 0) {
            if (seen[v]) return false;
            seen[v] = true;
        }
    }
    return true;
}

/**
 * @brief 宫合法性检查
 */
bool SudokuValidator::isBoxValid(const QVector<QVector<int>> &board,
                                 int boxRow, int boxCol) const
{
    bool seen[10] = {false};
    for (int r = 0; r < 3; ++r) {
        for (int c = 0; c < 3; ++c) {
            int v = board[boxRow + r][boxCol + c];
            if (v != 0) {
                if (seen[v]) return false;
                seen[v] = true;
            }
        }
    }
    return true;
}

/**
 * @brief 是否完整且全部正确
 */
bool SudokuValidator::isComplete(const QVector<QVector<int>> &board,
                                 const QVector<QVector<int>> &solution) const
{
    for (int r = 0; r < 9; ++r) {
        for (int c = 0; c < 9; ++c) {
            if (board[r][c] != solution[r][c] || board[r][c] == 0) {
                return false;
            }
        }
    }
    return true;
}

/**
 * @brief 是否已填满
 */
bool SudokuValidator::isFilled(const QVector<QVector<int>> &board) const
{
    for (int r = 0; r < 9; ++r) {
        for (int c = 0; c < 9; ++c) {
            if (board[r][c] == 0) return false;
        }
    }
    return true;
}

/**
 * @brief 获取冲突格子列表
 */
QVector<QPair<int, int>> SudokuValidator::getErrors() const
{
    return m_errors;
}
