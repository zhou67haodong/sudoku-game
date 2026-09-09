/**
 * @file SudokuValidator.h
 * @brief 数独输入校验器 - 实时检测行/列/宫冲突，判断完成状态
 *
 * @author 张啸林
 * @date 2026-09-01
 *
 * 功能说明：
 *   - validate()        : 校验整个棋盘，返回所有冲突格子的坐标
 *   - getErrors()       : 获取上次校验的冲突格子列表
 *   - isRowValid()      : 单独校验某一行
 *   - isColValid()      : 单独校验某一列
 *   - isBoxValid()      : 单独校验某一宫（3x3）
 *   - isComplete()      : 判断棋盘是否完整且正确（与答案一致）
 */

#ifndef SUDOKU_VALIDATOR_H
#define SUDOKU_VALIDATOR_H

#include <QVector>
#include <QPair>

/**
 * @class SudokuValidator
 * @brief 数独合法性校验器
 */
class SudokuValidator
{
public:
    /**
     * @brief 默认构造
     */
    SudokuValidator();

    /**
     * @brief 校验整个棋盘，记录所有冲突格子
     * @param board 当前棋盘（0=空格）
     * @return 是否完全没有冲突
     */
    bool validate(const QVector<QVector<int>> &board);

    /**
     * @brief 获取校验发现的冲突格子坐标列表
     * @return 冲突格子 (row, col) 列表
     */
    QVector<QPair<int, int>> getErrors() const;

    /**
     * @brief 校验指定行是否合法（无重复数字）
     * @param board 棋盘
     * @param row 行号 0~8
     * @return true 合法
     */
    bool isRowValid(const QVector<QVector<int>> &board, int row) const;

    /**
     * @brief 校验指定列是否合法
     * @param board 棋盘
     * @param col 列号 0~8
     * @return true 合法
     */
    bool isColValid(const QVector<QVector<int>> &board, int col) const;

    /**
     * @brief 校验指定 3x3 宫是否合法
     * @param board 棋盘
     * @param boxRow 宫起始行（0,3,6）
     * @param boxCol 宫起始列（0,3,6）
     * @return true 合法
     */
    bool isBoxValid(const QVector<QVector<int>> &board, int boxRow, int boxCol) const;

    /**
     * @brief 判断棋盘是否完整且全部正确
     * @param board 当前棋盘
     * @param solution 正确解
     * @return true 完全正确
     */
    bool isComplete(const QVector<QVector<int>> &board,
                    const QVector<QVector<int>> &solution) const;

    /**
     * @brief 判断棋盘是否已被填满（无空格）
     * @param board 棋盘
     * @return true 已填满
     */
    bool isFilled(const QVector<QVector<int>> &board) const;

private:
    QVector<QPair<int, int>> m_errors;  ///< 最近一次校验的冲突格子
};

#endif // SUDOKU_VALIDATOR_H
