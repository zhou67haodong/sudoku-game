/**
 * @file SudokuGenerator.h
 * @brief 数独棋盘生成器 - 使用回溯算法生成完整解并挖洞生成谜题
 *
 * @author 张啸林
 * @date 2026-09-01
 *
 * 功能说明：
 *   - generateFullBoard()  : 使用回溯 + 随机洗牌生成一个完整的 9x9 合法数独解
 *   - generatePuzzle()    : 基于难度等级从完整解中挖去若干格子，生成谜题
 *   - getSolution()       : 返回当前完整解
 *   - getPuzzle()         : 返回当前谜题（部分格子为空）
 *   - getDifficulty()     : 返回当前难度等级字符串
 *
 * 难度对照：
 *   Easy   : 35~40 个已知格子（挖去 41~46 个）
 *   Medium : 30~34 个已知格子（挖去 47~50 个）
 *   Hard   : 25~29 个已知格子（挖去 51~55 个）
 */

#ifndef SUDOKU_GENERATOR_H
#define SUDOKU_GENERATOR_H

#include <QVector>
#include <QString>

/**
 * @brief 数独难度等级枚举
 */
enum class SudokuDifficulty {
    Easy,
    Medium,
    Hard
};

/**
 * @class SudokuGenerator
 * @brief 数独生成器：负责生成完整解和谜题
 */
class SudokuGenerator
{
public:
    /**
     * @brief 默认构造函数，随机构造生成器
     */
    SudokuGenerator();

    /**
     * @brief 生成完整的 9x9 数独解
     * @return true 生成成功，false 生成失败（理论上不会失败）
     */
    bool generateFullBoard();

    /**
     * @brief 根据难度生成谜题
     * @param difficulty 难度等级
     * @return true 生成成功
     */
    bool generatePuzzle(SudokuDifficulty difficulty);

    /**
     * @brief 获取完整解（9x9 二维向量，0 表示空格）
     * @return 常量引用
     */
    const QVector<QVector<int>> & getSolution() const;

    /**
     * @brief 获取谜题（9x9 二维向量，0 表示空格）
     * @return 常量引用
     */
    const QVector<QVector<int>> & getPuzzle() const;

    /**
     * @brief 获取当前难度字符串
     * @return "Easy"、"Medium" 或 "Hard"
     */
    QString getDifficulty() const;

private:
    /**
     * @brief 回溯求解：在 board 上尝试在 (row, col) 位置填入合法数字
     * @param board 当前棋盘状态（就地修改）
     * @param row   行号 (0~8)
     * @param col   列号 (0~8)
     * @return true 成功填完棋盘，false 无解
     */
    bool solveBacktrack(QVector<QVector<int>> &board, int row, int col);

    /**
     * @brief 辅助回溯：尝试在指定行继续填数
     * @param board 当前棋盘状态
     * @param row   行号
     * @param col   列号
     * @return true 成功
     */
    bool solveBacktrackRow(QVector<QVector<int>> &board, int row, int col);

    /**
     * @brief 随机洗牌 1~9 的数组（Fisher-Yates 洗牌）
     * @param arr 要洗牌的数组（就地修改）
     */
    void shuffle(QVector<int> &arr);

    /**
     * @brief 检查在 (row, col) 位置放入 num 是否合法
     * @param board 棋盘
     * @param row   行号
     * @param col   列号
     * @param num   数字 1~9
     * @return true 合法
     */
    bool isValid(const QVector<QVector<int>> &board, int row, int col, int num) const;

    /**
     * @brief 从完整解中挖去格子生成谜题
     * @param solution 完整解
     * @param clues    目标已知格子数量
     */
    void carvePuzzle(const QVector<QVector<int>> &solution, int clues);

    QVector<QVector<int>> m_solution;  ///< 完整解
    QVector<QVector<int>> m_puzzle;    ///< 谜题
    SudokuDifficulty      m_difficulty; ///< 当前难度
};

#endif // SUDOKU_GENERATOR_H
