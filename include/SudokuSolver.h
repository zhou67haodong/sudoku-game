/**
 * @file SudokuSolver.h
 * @brief 数独求解器 - 回溯 + 约束传播，支持逐步求解与提示
 *
 * @author 张啸林
 * @date 2026-09-01
 *
 * 功能说明：
 *   - solve()        : 对给定棋盘执行完整回溯求解，返回是否唯一可解
 *   - isValid()      : 检查指定格子填入数字是否合法（行/列/宫无冲突）
 *   - isSolved()     : 判断当前棋盘是否已完全且正确填满
 *   - getHint()      : 返回一个合法单元格的行列提示（用于"提示"功能）
 *   - countSolutions(): 统计解的数量（上限 2，用于判断唯一解）
 *   - solveStep()    : 逐步求解一次（单步回溯），用于动画演示
 */

#ifndef SUDOKU_SOLVER_H
#define SUDOKU_SOLVER_H

#include <QVector>
#include <QPair>

/**
 * @class SudokuSolver
 * @brief 数独求解器：提供求解、验证、提示、多解统计等功能
 */
class SudokuSolver
{
public:
    /**
     * @brief 默认构造
     */
    SudokuSolver();

    /**
     * @brief 对给定棋盘执行完整回溯求解
     * @param board 输入棋盘（0=空格）；求解成功后 board 变为解
     * @return true 有解（至少一个），false 无解
     */
    bool solve(QVector<QVector<int>> &board);

    /**
     * @brief 检查 board 中 (row, col) 位置填入 num 是否合法
     * @param board 棋盘（只读）
     * @param row   行 0~8
     * @param col   列 0~8
     * @param num   数字 1~9
     * @return true 合法（不违反行/列/宫约束）
     */
    bool isValid(const QVector<QVector<int>> &board,
                 int row, int col, int num) const;

    /**
     * @brief 判断当前 board 是否已完全、正确填满
     * @param board 棋盘
     * @return true 完整且无冲突
     */
    bool isSolved(const QVector<QVector<int>> &board) const;

    /**
     * @brief 获取一个空格的合法提示
     * @param board    当前棋盘
     * @param outRow   输出行号
     * @param outCol   输出列号
     * @param outNum   输出建议数字
     * @return true 成功获取提示，false 棋盘已满或无合法格子
     */
    bool getHint(const QVector<QVector<int>> &board,
                 int &outRow, int &outCol, int &outNum);

    /**
     * @brief 统计给定棋盘的解的数量（最多计数到 2，效率优先）
     * @param board 棋盘
     * @return 0 无解，1 唯一解，2 两个及以上解
     */
    int countSolutions(const QVector<QVector<int>> &board);

    /**
     * @brief 单步求解：执行一次回溯尝试（用于动画/演示）
     *
     * 每次调用尝试找到一个空格并填入一个可能的数字。
     * 若无空格可填，则自动回溯清除上一次填入。
     *
     * @param board 棋盘（就地修改）
     * @return true 进行了有效操作（填入或回溯），false 求解结束
     */
    bool solveStep(QVector<QVector<int>> &board);

    /**
     * @brief 重置求解器内部状态（清除 solveStep 的历史记录）
     */
    void reset();

private:
    /**
     * @brief 递归回溯求解的核心实现
     * @param board 棋盘
     * @return true 找到至少一个解
     */
    bool solveRecursive(QVector<QVector<int>> &board);

    /**
     * @brief 统计解数量的内部递归实现
     * @param board  棋盘
     * @param count  当前已找到的解数量（超过 limit 即可停止）
     * @param limit  计数上限
     * @return 已找到的解数量
     */
    int countRecursive(const QVector<QVector<int>> &board,
                       int count, int limit);

    /**
     * @brief 找到下一个待填空格（按行优先）
     * @param board  棋盘
     * @param outRow 输出行
     * @param outCol 输出列
     * @return true 找到空格，false 无空格（已满）
     */
    bool findNextEmpty(const QVector<QVector<int>> &board,
                       int &outRow, int &outCol) const;

    // solveStep 内部状态（用于动画演示）
    QVector<QPair<int, int>> m_history;   ///< 已填格子的历史（支持回退）
    int m_lastRow;   ///< 上次操作行
    int m_lastCol;   ///< 上次操作列
};

#endif // SUDOKU_SOLVER_H
