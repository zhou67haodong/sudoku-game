/**
 * @file SudokuBoard.h
 * @brief 数独棋盘控件头文件（升级版）
 * @author 周浩东
 * @date 2026-08-31
 *
 * 功能说明：
 * - 继承自 QWidget，作为数独游戏的 9x9 棋盘界面
 * - 支持鼠标点击选中格子、键盘输入数字、自动校验、高亮行列宫与同数字
 * - 支持笔记模式（铅笔标记候选数）、撤销（Ctrl+Z）、暂停遮罩
 * - 格子状态包括：普通、选中、固定、错误、提示、笔记
 */

#ifndef SUDOKUBOARD_H
#define SUDOKUBOARD_H

#include <QWidget>
#include <QVector>
#include <QGridLayout>
#include <QLabel>
#include <QSet>
#include <QMap>
#include <QPair>
#include <QRandomGenerator>
#include <QStack>

/**
 * @brief 格子单元格状态枚举
 */
enum class CellState {
    Normal,   ///< 普通状态（可编辑且未选中）
    Selected, ///< 选中状态
    Fixed,    ///< 固定格子（题目预置，不可编辑）
    Error,    ///< 错误状态（与规则冲突）
    Hint      ///< 提示状态（系统给出的提示）
};

/**
 * @brief 单元格数据结构
 */
struct SudokuCell {
    int value;           ///< 当前填入的数字（0 表示空）
    bool isFixed;        ///< 是否为固定格子（题目预置）
    CellState state;     ///< 当前状态
    QLabel *label;       ///< 关联的 QLabel 控件指针
    QSet<int> notes;     ///< 笔记候选数字集合（升级新增）
};

/**
 * @brief 一步操作记录（用于撤销）
 */
struct MoveRecord {
    int row, col;        ///< 操作位置
    int prevValue;       ///< 操作前的值
    QSet<int> prevNotes; ///< 操作前的笔记
};

/**
 * @brief 数独棋盘类
 *
 * 负责管理 9x9 的数独格子，提供用户交互、数独逻辑验证、
 * 行列宫/同数字高亮、提示、笔记、撤销等功能。
 */
class SudokuBoard : public QWidget
{
    Q_OBJECT

public:
    explicit SudokuBoard(QWidget *parent = nullptr);
    ~SudokuBoard();

    /** @brief 开始一局新游戏 */
    void newGame(int difficulty = 0);
    /** @brief 获取当前格子值 */
    int getValue(int row, int col) const;
    /** @brief 设置格子值 */
    void setValue(int row, int col, int value);
    /** @brief 获取谜题初始数据 */
    QVector<QVector<int>> getPuzzle() const;
    /** @brief 获取解答数据 */
    QVector<QVector<int>> getSolution() const;
    /** @brief 从已有谜题加载游戏 */
    void loadGame(const QVector<QVector<int>>& puzzle,
                  const QVector<QVector<int>>& solution);
    /** @brief 获取完成进度 */
    double getProgress() const;
    /** @brief 检查游戏是否完成 */
    bool isCompleted() const;
    /** @brief 获取固定格子数 */
    int getFixedCount() const;
    /** @brief 统计已填格子数 */
    int countFilled() const;
    /** @brief 检查棋盘放置合法性 */
    bool isBoardValidPlacement(const QVector<QVector<int>>& board,
                               int row, int col, int value);
    /** @brief 是否有选中的格子 */
    bool hasSelection() const;
    int selectedRow() const;
    int selectedCol() const;

    // ---------- 升级新增 API ----------
    /** @brief 统一数字输入入口：笔记模式下记笔记，否则填数 */
    void inputNumber(int row, int col, int n);
    /** @brief 设置笔记模式开关（铅笔标记） */
    void setNoteMode(bool on);
    bool isNoteMode() const { return m_noteMode; }
    /** @brief 撤销上一步（返回是否撤销成功） */
    bool undo();
    /** @brief 是否可撤销 */
    bool canUndo() const { return !m_undoStack.isEmpty(); }
    /** @brief 撤销栈深度 */
    int undoDepth() const { return m_undoStack.size(); }
    /** @brief 统计某数字在棋盘上已出现的次数 */
    int numberCount(int n) const;
    /** @brief 暂停/恢复（暂停时显示遮罩隐藏盘面） */
    void setPaused(bool paused);
    bool isPaused() const { return m_paused; }
    /** @brief 清空撤销历史（新局/读档时调用） */
    void clearUndoHistory();

signals:
    void progressUpdated(int filled, int total);
    void gameCompleted();
    void cellChanged(int row, int col, int value);
    void noteModeChanged(bool on);          ///< 笔记模式切换
    void undoAvailabilityChanged(bool ok);  ///< 撤销可用性变化

public slots:
    void giveHint();
    void checkBoard();
    void clearErrors();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    QSize sizeHint() const override;

private slots:
    void selectCell(int row, int col);

private:
    QVector<QVector<SudokuCell>> m_cells;
    QVector<QVector<int>> m_puzzle;
    QVector<QVector<int>> m_solution;
    SudokuCell *m_selectedCell;
    int m_currentRow;
    int m_currentCol;

    // 升级新增状态
    bool m_noteMode;                    ///< 笔记模式
    bool m_paused;                      ///< 暂停状态
    QStack<MoveRecord> m_undoStack;     ///< 撤销栈
    QLabel *m_pauseMask;                ///< 暂停遮罩

    static const QString COLOR_NORMAL;
    static const QString COLOR_SELECTED;
    static const QString COLOR_FIXED;
    static const QString COLOR_ERROR;
    static const QString COLOR_HINT;
    static const QString COLOR_RELATED_BG;
    static const QString COLOR_SAME_VALUE;   ///< 同数字高亮色

    void setupUi();
    void createCells();
    void updateCellStyle(SudokuCell &cell);
    /** 统一刷新全盘样式（含行列宫高亮与同数字高亮） */
    void refreshAllStyles();
    void highlightRelated(int row, int col);
    void clearHighlights();
    void validateBoard();
    bool isValidPlacement(int row, int col, int value);
    void generateSudoku(int difficulty);
    void generateSolution();
    void moveSelection(int row, int col, int dRow, int dCol);
    bool eventFilter(QObject *watched, QEvent *event) override;
    bool fillBoard(QVector<QVector<int>> &board);
    QPair<int, int> findEmpty(const QVector<QVector<int>> &board);
    /** 在指定格子切换笔记数字 */
    void toggleNote(int row, int col, int n);
    /** 刷新指定格子的显示文本（值或笔记） */
    void refreshCellText(int row, int col);
    inline int boxIndex(int row, int col) {
        return (row / 3) * 3 + (col / 3);
    }
};

#endif // SUDOKUBOARD_H
