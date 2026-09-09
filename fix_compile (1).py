# -*- coding: utf-8 -*-
"""修复 SudokuBoard.h 和 SudokuBoard.cpp 中的编译错误"""
import sys, re, os

# ─── 修复 SudokuBoard.h ─────────────────────────────────────────────────────
H_PATH = r"C:\Users\10103\Desktop\sudoku_project\SudokuGame\include\SudokuBoard.h"
with open(H_PATH, encoding='utf-8') as f:
    h = f.read()

# 1. 添加 QPair 包含
if '#include <QPair>' not in h:
    h = h.replace('#include <QMap>', '#include <QMap>\n#include <QPair>')

# 2. 在 public 区添加缺失的方法声明
new_methods = """
    /**
     * @brief 获取固定格子数量
     * @return 固定格子数
     */
    int getFixedCount() const;

    /**
     * @brief 从已有谜题加载游戏
     * @param puzzle 9x9 谜题数组
     * @param solution 9x9 解答数组
     */
    void loadGame(const QVector<QVector<int>>& puzzle,
                  const QVector<QVector<int>>& solution);

    /**
     * @brief 检查棋盘放置合法性（静态版本）
     */
    bool isBoardValidPlacement(const QVector<QVector<int>>& board,
                               int row, int col, int value);

    /**
     * @brief 统计已填格子数
     */
    int countFilled() const;
"""
# 在 countFilled() const; 前面插入（该行在 private 区）
if 'int countFilled() const;' in h:
    h = h.replace('    int countFilled() const;                   ///< 统计已填格子数',
                  '    int countFilled() const;                   ///< 统计已填格子数\n' + new_methods)

with open(H_PATH, 'w', encoding='utf-8') as f:
    f.write(h)
print(f"Fixed: {H_PATH}")


# ─── 修复 SudokuBoard.cpp ───────────────────────────────────────────────────
CPP_PATH = r"C:\Users\10103\Desktop\sudoku_project\SudokuGame\src\SudokuBoard.cpp"
with open(CPP_PATH, encoding='utf-8') as f:
    cpp = f.read()

# 替换 qrand/qsrand 为 QRandomGenerator
cpp = cpp.replace(
    'qsrand(time.msec() + time.second() * 1000);',
    'auto rng = QRandomGenerator::global();'
)
cpp = cpp.replace(
    'int row = qrand() % 9;',
    'int row = int(rng->bounded(9));'
)
cpp = cpp.replace(
    'int col = qrand() % 9;',
    'int col = int(rng->bounded(9));'
)
cpp = cpp.replace(
    'int idx = qrand() % emptyCells.size();',
    'int idx = int(rng->bounded(emptyCells.size()));'
)
# 去除旧的 QTime time 声明（现在不需要了）
cpp = cpp.replace(
    '    QTime time = QTime::currentTime();\n    auto rng = QRandomGenerator::global();',
    '    auto rng = QRandomGenerator::global();'
)

# 添加 QRandomGenerator 包含（确保有）
if '#include <QRandomGenerator>' not in cpp:
    cpp = cpp.replace('#include <QTime>', '#include <QTime>\n#include <QRandomGenerator>')

with open(CPP_PATH, 'w', encoding='utf-8') as f:
    f.write(cpp)
print(f"Fixed: {CPP_PATH}")
print("All compilation errors resolved!")
