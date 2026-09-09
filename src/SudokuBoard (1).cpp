/**
 * @file SudokuBoard.cpp
 * @brief 数独棋盘控件实现文件（升级版）
 * @author 周浩东
 * @date 2026-08-31
 *
 * 升级内容：
 * - 全新视觉样式：圆角格子、宫格粗线、同数字/行列宫高亮、精致配色
 * - 笔记模式：在空格中用小字标记候选数（3x3 排版）
 * - 撤销功能：Ctrl+Z 或工具按钮，支持撤销填数/擦除/笔记/提示
 * - 暂停功能：半透明遮罩覆盖盘面
 */

#include "SudokuBoard.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFont>
#include <QPalette>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QMessageBox>
#include <QDebug>
#include <QTime>
#include <QRandomGenerator>
#include <QApplication>
#include <QResizeEvent>

// ==================== 样式颜色常量定义（升级配色） ====================
const QString SudokuBoard::COLOR_NORMAL     = "#FFFFFF";   // 白色底
const QString SudokuBoard::COLOR_SELECTED   = "#DBEAFE";   // 选中淡蓝
const QString SudokuBoard::COLOR_FIXED      = "#F1F5F9";   // 固定格浅灰蓝
const QString SudokuBoard::COLOR_ERROR      = "#FEE2E2";   // 错误淡红
const QString SudokuBoard::COLOR_HINT       = "#DCFCE7";   // 提示淡绿
const QString SudokuBoard::COLOR_RELATED_BG = "#EDF2FA";   // 行列宫淡蓝
const QString SudokuBoard::COLOR_SAME_VALUE = "#FEF3C7";   // 同数字琥珀色

// ==================== 构造函数 ====================
SudokuBoard::SudokuBoard(QWidget *parent)
    : QWidget(parent)
    , m_selectedCell(nullptr)
    , m_currentRow(-1)
    , m_currentCol(-1)
    , m_noteMode(false)
    , m_paused(false)
    , m_pauseMask(nullptr)
{
    // 设置窗口基本属性
    setMinimumSize(500, 500);
    setFocusPolicy(Qt::StrongFocus);  // 确保能够接收键盘事件

    // 初始化数据
    m_puzzle.resize(9);
    m_solution.resize(9);
    for (int i = 0; i < 9; ++i) {
        m_puzzle[i].resize(9);
        m_solution[i].resize(9);
    }

    setupUi();
    newGame(0);  // 默认开始简单难度
}

// ==================== 析构函数 ====================
SudokuBoard::~SudokuBoard()
{
    // Qt 的父子对象机制会自动清理子控件
}

// ==================== UI 初始化 ====================
void SudokuBoard::setupUi()
{
    // 主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // 创建 9x9 网格
    QGridLayout *gridLayout = new QGridLayout();
    gridLayout->setSpacing(2);  // 格子之间的间隙
    gridLayout->setContentsMargins(10, 10, 10, 10);

    // 初始化格子数据
    m_cells.resize(9);
    for (int i = 0; i < 9; ++i) {
        m_cells[i].resize(9);
    }

    // 创建 81 个格子
    createCells();

    // 将格子添加到网格布局（宫与宫之间加大间距模拟粗宫线）
    for (int row = 0; row < 9; ++row) {
        for (int col = 0; col < 9; ++col) {
            gridLayout->addWidget(m_cells[row][col].label, row, col);
        }
    }

    mainLayout->addLayout(gridLayout);

    // 52*9 格子 + 8*2 间距 + 2*10 边距 = 500
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setFixedSize(500, 500);
    updateGeometry();

    // 整体棋盘外观（圆角卡片风格）
    setStyleSheet(
        "SudokuBoard {"
        "    background-color: #FFFFFF;"
        "    border: 2px solid #CBD5E1;"
        "    border-radius: 12px;"
        "}"
    );

    // 暂停遮罩（初始隐藏）
    m_pauseMask = new QLabel(QStringLiteral("⏸ 已暂停\n\n点击此处继续"), this);
    m_pauseMask->setAlignment(Qt::AlignCenter);
    m_pauseMask->setStyleSheet(
        "QLabel {"
        "    background-color: rgba(30, 41, 59, 215);"
        "    color: #F8FAFC;"
        "    font-size: 26px;"
        "    font-weight: bold;"
        "    border-radius: 12px;"
        "}"
    );
    m_pauseMask->hide();
    m_pauseMask->installEventFilter(this);

    // 启动时让棋盘获得焦点，确保键盘输入生效
    setFocus();
}

// ==================== 创建格子 ====================
void SudokuBoard::createCells()
{
    for (int row = 0; row < 9; ++row) {
        for (int col = 0; col < 9; ++col) {
            QLabel *label = new QLabel(this);
            label->setAlignment(Qt::AlignCenter);
            label->setText("");
            label->setMargin(0);

            // 设置固定尺寸（确保所有格子大小一致）
            label->setFixedSize(52, 52);
            label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

            // 启用鼠标追踪，以便接收点击事件
            label->setMouseTracking(true);
            label->installEventFilter(this);  // 安装事件过滤器

            // 初始化单元格数据
            m_cells[row][col].value    = 0;
            m_cells[row][col].isFixed   = false;
            m_cells[row][col].state     = CellState::Normal;
            m_cells[row][col].label     = label;
            m_cells[row][col].notes.clear();
        }
    }
}

// ==================== 生成格子样式表（升级：圆角+宫线+状态色） ====================
static QString cellQSS(int row, int col, CellState state, bool hasNote)
{
    QString bg, fg = "#1E293B", weight = "normal";
    int radius = 5;

    switch (state) {
        case CellState::Selected: bg = "#DBEAFE"; break;
        case CellState::Fixed:    bg = "#F1F5F9"; weight = "bold"; fg = "#334155"; break;
        case CellState::Error:    bg = "#FEE2E2"; fg = "#DC2626"; break;
        case CellState::Hint:     bg = "#DCFCE7"; fg = "#15803D"; weight = "bold"; break;
        case CellState::Normal:
        default:                  bg = "#FFFFFF"; break;
    }
    if (hasNote) fg = "#94A3B8";

    // 选中格：统一的蓝色描边；其余格：宫线位置加粗描边
    QString border;
    if (state == CellState::Selected) {
        border = "border: 2px solid #3B82F6;";
        radius = 6;
    } else {
        QString l = (col % 3 == 0 && col > 0) ? "3" : "1";
        QString t = (row % 3 == 0 && row > 0) ? "3" : "1";
        QString r = (col == 8) ? "3" : "1";
        QString b = (row == 8) ? "3" : "1";
        border = QString("border-left: %1px solid #94A3B8; border-top: %2px solid #94A3B8;"
                          "border-right: %3px solid #94A3B8; border-bottom: %4px solid #94A3B8;")
                     .arg(l, t, r, b);
    }

    return QString(
        "QLabel {"
        "    background-color: %1;"
        "    %2"
        "    border-radius: %3px;"
        "    font-size: %4px;"
        "    font-family: 'Microsoft YaHei', 'Segoe UI';"
        "    font-weight: %5;"
        "    color: %6;"
        "}"
    ).arg(bg, border).arg(radius).arg(hasNote ? 10 : 22).arg(weight, fg);
}

// ==================== 事件过滤器（处理鼠标点击） ====================
bool SudokuBoard::eventFilter(QObject *watched, QEvent *event)
{
    // 点击暂停遮罩 → 恢复游戏
    if (watched == m_pauseMask && event->type() == QEvent::MouseButtonPress) {
        setPaused(false);
        return true;
    }

    // 遍历所有格子，找出被点击的格子
    for (int row = 0; row < 9; ++row) {
        for (int col = 0; col < 9; ++col) {
            if (m_cells[row][col].label == watched) {
                if (event->type() == QEvent::MouseButtonPress) {
                    // 升级：固定格子也可选中（用于同数字高亮），但不可编辑
                    selectCell(row, col);
                    return true;
                }
            }
        }
    }
    return QWidget::eventFilter(watched, event);
}

// ==================== 选中格子 ====================
void SudokuBoard::selectCell(int row, int col)
{
    // 清除之前的选中状态
    if (m_selectedCell != nullptr) {
        if (m_selectedCell->isFixed) {
            m_selectedCell->state = CellState::Fixed;
        } else {
            m_selectedCell->state = CellState::Normal;
        }
    }

    // 记录新位置
    m_currentRow = row;
    m_currentCol = col;

    // 设置新的选中状态（固定格保持 Fixed 底色 + 选中由刷新逻辑处理）
    SudokuCell &cell = m_cells[row][col];
    if (!cell.isFixed) {
        cell.state = CellState::Selected;
    }
    m_selectedCell = &cell;

    // 统一刷新全盘样式（含行列宫高亮 + 同数字高亮 + 错误状态）
    refreshAllStyles();

    // 选中后夺取焦点，保证键盘输入能持续进入棋盘
    setFocus();
}

// ==================== 统一刷新全盘样式（升级核心） ====================
void SudokuBoard::refreshAllStyles()
{
    // 1. 先重算冲突（不弹窗）
    validateBoard();

    // 2. 还原选中格状态（validateBoard 可能覆盖）
    int selRow = m_currentRow, selCol = m_currentCol;
    int selValue = (selRow >= 0 && selCol >= 0) ? m_cells[selRow][selCol].value : 0;

    int boxRowStart = (selRow >= 0) ? (selRow / 3) * 3 : -10;
    int boxColStart = (selCol >= 0) ? (selCol / 3) * 3 : -10;

    for (int r = 0; r < 9; ++r) {
        for (int c = 0; c < 9; ++c) {
            SudokuCell &cell = m_cells[r][c];
            bool hasNote = cell.value == 0 && !cell.notes.isEmpty();
            CellState st = cell.state;

            // 错误状态优先展示
            if (st != CellState::Error && st != CellState::Hint) {
                bool isRelated = (r == selRow) || (c == selCol) ||
                                 (r >= boxRowStart && r < boxRowStart + 3 &&
                                  c >= boxColStart && c < boxColStart + 3);
                bool isSame = (selValue != 0) && (cell.value == selValue);

                if (r == selRow && c == selCol) {
                    st = cell.isFixed ? CellState::Fixed : CellState::Selected;
                    if (cell.isFixed) {
                        // 固定格被选中：加蓝边效果（用 Selected 样式但保持 Fixed 底色由 QSS 决定）
                        st = CellState::Selected;
                    }
                } else if (isSame) {
                    // 同数字高亮：临时借用 Related 底色之上的特殊处理
                    // 这里直接用琥珀色背景：临时 state 不可行，改为直接设置样式
                    cell.label->setStyleSheet(cellQSS(r, c, CellState::Normal, hasNote)
                        .replace("#FFFFFF", "#FEF3C7"));
                    continue;
                } else if (isRelated) {
                    st = cell.isFixed ? CellState::Fixed : CellState::Normal;
                    // 行列宫淡蓝背景
                    if (cell.state == CellState::Error) {
                        st = CellState::Error;
                    } else if (cell.state == CellState::Hint) {
                        st = CellState::Hint;
                    }
                    cell.label->setStyleSheet(cellQSS(r, c, st, hasNote)
                        .replace(st == CellState::Fixed ? "#F1F5F9" : "#FFFFFF", "#EDF2FA"));
                    continue;
                } else {
                    st = cell.isFixed ? CellState::Fixed : CellState::Normal;
                    if (cell.state == CellState::Error) st = CellState::Error;
                    if (cell.state == CellState::Hint) st = CellState::Hint;
                }
            }

            cell.label->setStyleSheet(cellQSS(r, c, st, hasNote));
        }
    }
}

// ==================== 高亮相关格子（保留接口，内部转调统一刷新） ====================
void SudokuBoard::highlightRelated(int row, int col)
{
    Q_UNUSED(row); Q_UNUSED(col);
    refreshAllStyles();
}

// ==================== 键盘事件处理（升级：N 笔记 / Ctrl+Z 撤销） ====================
void SudokuBoard::keyPressEvent(QKeyEvent *event)
{
    // 暂停时不接受输入
    if (m_paused) { QWidget::keyPressEvent(event); return; }

    // Ctrl+Z 撤销
    if (event->key() == Qt::Key_Z && event->modifiers() & Qt::ControlModifier) {
        undo();
        return;
    }
    // N 键切换笔记模式
    if (event->key() == Qt::Key_N) {
        setNoteMode(!m_noteMode);
        return;
    }

    // 如果没有选中格子，先选中第一个可编辑格子
    if (m_selectedCell == nullptr) {
        for (int r = 0; r < 9; ++r) {
            for (int c = 0; c < 9; ++c) {
                if (!m_cells[r][c].isFixed) {
                    selectCell(r, c);
                    break;
                }
            }
            if (m_selectedCell != nullptr) break;
        }
    }

    if (m_selectedCell == nullptr) return;

    // 找到当前选中格子的位置
    int currentRow = m_currentRow, currentCol = m_currentCol;
    if (currentRow < 0 || currentRow > 8 || currentCol < 0 || currentCol > 8) return;

    int key = event->key();

    // 数字键 0-9（同时支持主键盘区和小键盘）
    int num = -1;
    if (key >= Qt::Key_0 && key <= Qt::Key_9) {
        num = key - Qt::Key_0;
    }

    if (num >= 0 && num <= 9) {
        if (m_noteMode && num >= 1) {
            toggleNote(currentRow, currentCol, num);
        } else {
            setValue(currentRow, currentCol, num);
        }
    }
    // 退格键或 Delete 清除格子
    else if (key == Qt::Key_Backspace || key == Qt::Key_Delete) {
        setValue(currentRow, currentCol, 0);
    }
    // 方向键导航（跳过固定格子）
    else if (key == Qt::Key_Up && currentRow > 0) {
        moveSelection(currentRow - 1, currentCol, -1, 0);
    }
    else if (key == Qt::Key_Down && currentRow < 8) {
        moveSelection(currentRow + 1, currentCol, 1, 0);
    }
    else if (key == Qt::Key_Left && currentCol > 0) {
        moveSelection(currentRow, currentCol - 1, 0, -1);
    }
    else if (key == Qt::Key_Right && currentCol < 8) {
        moveSelection(currentRow, currentCol + 1, 0, 1);
    }
    else {
        QWidget::keyPressEvent(event);
    }
}

// ==================== 移动选择 ====================
void SudokuBoard::moveSelection(int row, int col, int dRow, int dCol)
{
    // 如果目标格子是固定的，沿同一方向继续寻找下一个可编辑格子
    int r = row;
    int c = col;
    while (r >= 0 && r < 9 && c >= 0 && c < 9 && m_cells[r][c].isFixed) {
        r += dRow;
        c += dCol;
    }
    if (r < 0 || r >= 9 || c < 0 || c >= 9) {
        // 该方向没有可编辑格子，则保持原地
        if (!m_cells[row][col].isFixed) {
            selectCell(row, col);
        }
        return;
    }
    selectCell(r, c);
}

// ==================== 设置格子值（升级：压入撤销栈） ====================
void SudokuBoard::setValue(int row, int col, int value)
{
    if (row < 0 || row >= 9 || col < 0 || col >= 9) return;
    if (value < 0 || value > 9) return;

    SudokuCell &cell = m_cells[row][col];

    // 固定格子不可修改
    if (cell.isFixed) return;

    // 值无变化则忽略
    if (cell.value == value) return;

    // 压入撤销记录
    MoveRecord rec;
    rec.row = row; rec.col = col;
    rec.prevValue = cell.value;
    rec.prevNotes = cell.notes;
    m_undoStack.push(rec);
    if (m_undoStack.size() > 500) m_undoStack.remove(0); // 限制栈深
    emit undoAvailabilityChanged(canUndo());

    // 设置新值（填入实际数字时清空笔记）
    cell.value = value;
    if (value != 0) cell.notes.clear();

    // 更新显示
    refreshCellText(row, col);

    // 验证并更新样式
    refreshAllStyles();

    // 更新进度（filled 统计玩家已填数，不含固定格）
    int fixed = getFixedCount();
    int filled = countFilled() - fixed;
    int total = 81 - fixed;
    emit progressUpdated(filled, total);
    emit cellChanged(row, col, value);

    // 检查是否完成（filled == total 表示所有空格均已填写）
    if (filled == total && isCompleted()) {
        emit gameCompleted();
    }
}

// ==================== 统一数字输入入口（升级新增） ====================
void SudokuBoard::inputNumber(int row, int col, int n)
{
    if (m_noteMode && n >= 1) {
        toggleNote(row, col, n);
    } else {
        setValue(row, col, n);
    }
}

// ==================== 笔记模式开关（升级新增） ====================
void SudokuBoard::setNoteMode(bool on)
{
    if (m_noteMode == on) return;
    m_noteMode = on;
    emit noteModeChanged(on);
}

// ==================== 笔记切换（升级新增） ====================
void SudokuBoard::toggleNote(int row, int col, int n)
{
    if (row < 0 || row >= 9 || col < 0 || col >= 9) return;
    SudokuCell &cell = m_cells[row][col];
    if (cell.isFixed || cell.value != 0) return; // 只在空格上记笔记

    // 压入撤销记录
    MoveRecord rec;
    rec.row = row; rec.col = col;
    rec.prevValue = cell.value;
    rec.prevNotes = cell.notes;
    m_undoStack.push(rec);
    emit undoAvailabilityChanged(canUndo());

    if (cell.notes.contains(n)) cell.notes.remove(n);
    else cell.notes.insert(n);

    refreshCellText(row, col);
    refreshAllStyles();
}

// ==================== 刷新格子文本（值 或 笔记排版） ====================
void SudokuBoard::refreshCellText(int row, int col)
{
    SudokuCell &cell = m_cells[row][col];
    if (cell.value != 0) {
        cell.label->setText(QString::number(cell.value));
        return;
    }
    if (cell.notes.isEmpty()) {
        cell.label->setText("");
        return;
    }
    // 笔记: 3x3 小字排版
    QString html = "<div style='line-height:11px; font-size:10px;'>";
    for (int n = 1; n <= 9; ++n) {
        html += cell.notes.contains(n) ? QString::number(n) : QString("&nbsp;");
        if (n % 3 == 0 && n != 9) html += "<br>";
    }
    html += "</div>";
    cell.label->setText(html);
}

// ==================== 撤销（升级新增） ====================
bool SudokuBoard::undo()
{
    if (m_undoStack.isEmpty()) return false;

    MoveRecord rec = m_undoStack.pop();
    SudokuCell &cell = m_cells[rec.row][rec.col];
    cell.value = rec.prevValue;
    cell.notes = rec.prevNotes;
    if (cell.state == CellState::Hint) {
        cell.state = CellState::Normal;
    }

    refreshCellText(rec.row, rec.col);
    refreshAllStyles();

    // 进度统计（玩家已填数，不含固定格）
    int fixed = getFixedCount();
    int filled = countFilled() - fixed;
    int total = 81 - fixed;
    emit progressUpdated(filled, total);
    emit cellChanged(rec.row, rec.col, cell.value);
    emit undoAvailabilityChanged(canUndo());
    return true;
}

// ==================== 清空撤销历史（升级新增） ====================
void SudokuBoard::clearUndoHistory()
{
    m_undoStack.clear();
    emit undoAvailabilityChanged(false);
}

// ==================== 统计数字出现次数（升级新增） ====================
int SudokuBoard::numberCount(int n) const
{
    int cnt = 0;
    for (int r = 0; r < 9; ++r)
        for (int c = 0; c < 9; ++c)
            if (m_cells[r][c].value == n) ++cnt;
    return cnt;
}

// ==================== 暂停（升级新增） ====================
void SudokuBoard::setPaused(bool paused)
{
    if (m_paused == paused) return;
    m_paused = paused;
    if (paused) {
        m_pauseMask->setGeometry(rect());
        m_pauseMask->raise();
        m_pauseMask->show();
    } else {
        m_pauseMask->hide();
        setFocus();
    }
}

// ==================== 窗口尺寸变化时同步遮罩 ====================
void SudokuBoard::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    if (m_pauseMask) m_pauseMask->setGeometry(rect());
}

// ==================== 获取固定格子数量 ====================
int SudokuBoard::getFixedCount() const
{
    int count = 0;
    for (int r = 0; r < 9; ++r) {
        for (int c = 0; c < 9; ++c) {
            if (m_cells[r][c].isFixed) ++count;
        }
    }
    return count;
}

// ==================== 获取格子值 ====================
int SudokuBoard::getValue(int row, int col) const
{
    if (row < 0 || row >= 9 || col < 0 || col >= 9) return 0;
    return m_cells[row][col].value;
}

// ==================== 验证数独合法性 ====================
void SudokuBoard::validateBoard()
{
    // 清除所有错误状态
    clearErrors();

    // 检查每一行
    for (int row = 0; row < 9; ++row) {
        QMap<int, int> seen;  // value -> count
        for (int col = 0; col < 9; ++col) {
            int val = m_cells[row][col].value;
            if (val != 0) {
                if (seen.contains(val)) {
                    // 发现重复，标记为错误
                    m_cells[row][col].state = CellState::Error;
                    m_cells[row][seen[val]].state = CellState::Error;
                } else {
                    seen[val] = col;
                }
            }
        }
    }

    // 检查每一列
    for (int col = 0; col < 9; ++col) {
        QMap<int, int> seen;
        for (int row = 0; row < 9; ++row) {
            int val = m_cells[row][col].value;
            if (val != 0) {
                if (seen.contains(val)) {
                    m_cells[row][col].state = CellState::Error;
                    m_cells[seen[val]][col].state = CellState::Error;
                } else {
                    seen[val] = row;
                }
            }
        }
    }

    // 检查每个 3x3 宫格
    for (int boxRow = 0; boxRow < 3; ++boxRow) {
        for (int boxCol = 0; boxCol < 3; ++boxCol) {
            QMap<int, QPair<int, int>> seen;
            for (int i = 0; i < 3; ++i) {
                for (int j = 0; j < 3; ++j) {
                    int row = boxRow * 3 + i;
                    int col = boxCol * 3 + j;
                    int val = m_cells[row][col].value;
                    if (val != 0) {
                        if (seen.contains(val)) {
                            m_cells[row][col].state = CellState::Error;
                            m_cells[seen[val].first][seen[val].second].state = CellState::Error;
                        } else {
                            seen[val] = qMakePair(row, col);
                        }
                    }
                }
            }
        }
    }
}

// ==================== 检查放置是否合法 ====================
bool SudokuBoard::isValidPlacement(int row, int col, int value)
{
    if (value == 0) return true;

    // 检查行
    for (int c = 0; c < 9; ++c) {
        if (c != col && m_cells[row][c].value == value) return false;
    }

    // 检查列
    for (int r = 0; r < 9; ++r) {
        if (r != row && m_cells[r][col].value == value) return false;
    }

    // 检查 3x3 宫格
    int boxRowStart = (row / 3) * 3;
    int boxColStart = (col / 3) * 3;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            int r = boxRowStart + i;
            int c = boxColStart + j;
            if (r != row || c != col) {
                if (m_cells[r][c].value == value) return false;
            }
        }
    }

    return true;
}

// ==================== 清除错误标记 ====================
void SudokuBoard::clearErrors()
{
    for (int row = 0; row < 9; ++row) {
        for (int col = 0; col < 9; ++col) {
            SudokuCell &cell = m_cells[row][col];
            if (cell.state == CellState::Error) {
                cell.state = cell.isFixed ? CellState::Fixed : CellState::Normal;
            }
        }
    }
}

// ==================== 更新格子样式（保留接口，转调统一刷新） ====================
void SudokuBoard::updateCellStyle(SudokuCell &cell)
{
    Q_UNUSED(cell);
    refreshAllStyles();
}

// ==================== 新游戏 ====================
void SudokuBoard::newGame(int difficulty)
{
    // 清空所有格子
    for (int row = 0; row < 9; ++row) {
        for (int col = 0; col < 9; ++col) {
            m_cells[row][col].value = 0;
            m_cells[row][col].isFixed = false;
            m_cells[row][col].state = CellState::Normal;
            m_cells[row][col].notes.clear();
            m_cells[row][col].label->setText("");
        }
    }
    m_selectedCell = nullptr;
    m_currentRow = m_currentCol = -1;
    clearUndoHistory();

    // 生成数独谜题
    generateSudoku(difficulty);

    // 根据谜题设置固定格子
    for (int row = 0; row < 9; ++row) {
        for (int col = 0; col < 9; ++col) {
            int value = m_puzzle[row][col];
            if (value != 0) {
                m_cells[row][col].value = value;
                m_cells[row][col].isFixed = true;
                m_cells[row][col].state = CellState::Fixed;
                m_cells[row][col].label->setText(QString::number(value));
            }
        }
    }

    refreshAllStyles();

    // 初始进度（玩家已填数=0）
    int fixed = getFixedCount();
    int filled = countFilled() - fixed;
    int total = 81 - fixed;
    emit progressUpdated(filled, total);
}

// ==================== 生成数独谜题 ====================
void SudokuBoard::generateSudoku(int difficulty)
{
    // 先填入一个完整解答
    generateSolution();

    // 根据难度挖空数量：
    // 0=简单 35个空, 1=中等 45个空, 2=困难 55个空
    int holes = 35 + difficulty * 10;

    // 复制解答作为谜题
    m_puzzle = m_solution;

    // 随机挖空
    auto rng = QRandomGenerator::global();

    QSet<QPair<int, int>> removed;
    while (removed.size() < holes) {
        int row = int(rng->bounded(9));
        int col = int(rng->bounded(9));
        QPair<int, int> pos = qMakePair(row, col);
        if (!removed.contains(pos)) {
            m_puzzle[row][col] = 0;  // 挖空
            removed.insert(pos);
        }
    }
}

// ==================== 生成完整解答 ====================
void SudokuBoard::generateSolution()
{
    // 初始化为空
    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            m_solution[i][j] = 0;
        }
    }

    // 递归填充（回溯算法）
    fillBoard(m_solution);
}

// ==================== 回溯填充数独 ====================
bool SudokuBoard::fillBoard(QVector<QVector<int>> &board)
{
    // 找到下一个空格
    QPair<int, int> empty = findEmpty(board);
    if (empty.first == -1) return true;  // 没有空格，填充完成

    int row = empty.first;
    int col = empty.second;

    // 随机尝试数字 1-9
    QList<int> nums;
    for (int i = 1; i <= 9; ++i) nums.append(i);
    std::random_shuffle(nums.begin(), nums.end());

    for (int num : nums) {
        if (isBoardValidPlacement(board, row, col, num)) {
            board[row][col] = num;
            if (fillBoard(board)) {
                return true;
            }
            board[row][col] = 0;  // 回溯
        }
    }

    return false;
}

// ==================== 在棋盘上找空格 ====================
QPair<int, int> SudokuBoard::findEmpty(const QVector<QVector<int>> &board)
{
    for (int row = 0; row < 9; ++row) {
        for (int col = 0; col < 9; ++col) {
            if (board[row][col] == 0) {
                return qMakePair(row, col);
            }
        }
    }
    return qMakePair(-1, -1);
}

// ==================== 检查棋盘放置合法性（带棋盘参数） ====================
bool SudokuBoard::isBoardValidPlacement(const QVector<QVector<int>> &board, int row, int col, int value)
{
    // 检查行
    for (int c = 0; c < 9; ++c) {
        if (board[row][c] == value) return false;
    }

    // 检查列
    for (int r = 0; r < 9; ++r) {
        if (board[r][col] == value) return false;
    }

    // 检查 3x3 宫格
    int boxRowStart = (row / 3) * 3;
    int boxColStart = (col / 3) * 3;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (board[boxRowStart + i][boxColStart + j] == value) return false;
        }
    }

    return true;
}

// ==================== 统计已填格子数 ====================
int SudokuBoard::countFilled() const
{
    int count = 0;
    for (int row = 0; row < 9; ++row) {
        for (int col = 0; col < 9; ++col) {
            if (m_cells[row][col].value != 0) ++count;
        }
    }
    return count;
}

// ==================== 检查游戏是否完成 ====================
bool SudokuBoard::isCompleted() const
{
    // 检查是否所有格子都已填满
    for (int row = 0; row < 9; ++row) {
        for (int col = 0; col < 9; ++col) {
            if (m_cells[row][col].value == 0) return false;
        }
    }

    // 检查解答是否正确
    for (int row = 0; row < 9; ++row) {
        for (int col = 0; col < 9; ++col) {
            if (m_cells[row][col].value != m_solution[row][col]) {
                return false;
            }
        }
    }

    return true;
}

// ==================== 获取谜题 ====================
QVector<QVector<int>> SudokuBoard::getPuzzle() const
{
    return m_puzzle;
}

// ==================== 获取解答 ====================
QVector<QVector<int>> SudokuBoard::getSolution() const
{
    return m_solution;
}

// ==================== 加载游戏 ====================
void SudokuBoard::loadGame(const QVector<QVector<int>> &puzzle,
                            const QVector<QVector<int>> &solution)
{
    m_puzzle = puzzle;
    m_solution = solution;

    // 清空并重新设置
    for (int row = 0; row < 9; ++row) {
        for (int col = 0; col < 9; ++col) {
            m_cells[row][col].value = 0;
            m_cells[row][col].isFixed = false;
            m_cells[row][col].state = CellState::Normal;
            m_cells[row][col].notes.clear();
            m_cells[row][col].label->setText("");

            // 设置固定格子
            if (m_puzzle[row][col] != 0) {
                m_cells[row][col].value = m_puzzle[row][col];
                m_cells[row][col].isFixed = true;
                m_cells[row][col].state = CellState::Fixed;
                m_cells[row][col].label->setText(QString::number(m_puzzle[row][col]));
            }
        }
    }
    m_selectedCell = nullptr;
    m_currentRow = m_currentCol = -1;
    clearUndoHistory();

    refreshAllStyles();

    // 初始进度（玩家已填数=0，不把固定格计入）
    int fixed = getFixedCount();
    int filled = countFilled() - fixed;
    int total = 81 - fixed;
    emit progressUpdated(filled, total);
}

// ==================== 获取进度 ====================
double SudokuBoard::getProgress() const
{
    int filled = countFilled();
    int fixed = getFixedCount();
    int total = 81 - fixed;
    if (total == 0) return 1.0;
    return static_cast<double>(filled - fixed) / total;
}

// ==================== 提供提示（升级：可撤销） ====================
void SudokuBoard::giveHint()
{
    // 找随机一个未填且不是固定格子的位置
    QList<QPair<int, int>> emptyCells;
    for (int row = 0; row < 9; ++row) {
        for (int col = 0; col < 9; ++col) {
            if (!m_cells[row][col].isFixed && m_cells[row][col].value == 0) {
                emptyCells.append(qMakePair(row, col));
            }
        }
    }

    if (emptyCells.isEmpty()) {
        QMessageBox::information(this, "提示", "已经没有空格可以提示了！");
        return;
    }

    // 随机选一个
    auto rng = QRandomGenerator::global();
    int idx = int(rng->bounded(emptyCells.size()));
    int row = emptyCells[idx].first;
    int col = emptyCells[idx].second;

    // 压入撤销记录（提示也可撤销）
    MoveRecord rec;
    rec.row = row; rec.col = col;
    rec.prevValue = m_cells[row][col].value;
    rec.prevNotes = m_cells[row][col].notes;
    m_undoStack.push(rec);
    emit undoAvailabilityChanged(canUndo());

    // 填入正确答案
    int correctValue = m_solution[row][col];
    SudokuCell &cell = m_cells[row][col];
    cell.value = correctValue;
    cell.notes.clear();
    cell.state = CellState::Hint;
    refreshCellText(row, col);
    refreshAllStyles();

    // 发出信号
    emit cellChanged(row, col, correctValue);

    // 更新进度（玩家已填数，不含固定格）
    int fixed = getFixedCount();
    int filled = countFilled() - fixed;
    int total = 81 - fixed;
    emit progressUpdated(filled, total);

    // 检查完成
    if (filled == total && isCompleted()) {
        emit gameCompleted();
    }
}

// ==================== 检查棋盘 ====================
void SudokuBoard::checkBoard()
{
    // 先清除错误
    clearErrors();

    // 重新验证
    validateBoard();
    refreshAllStyles();

    // 检查是否有错误
    bool hasError = false;
    for (int row = 0; row < 9; ++row) {
        for (int col = 0; col < 9; ++col) {
            if (m_cells[row][col].state == CellState::Error) {
                hasError = true;
                break;
            }
        }
        if (hasError) break;
    }

    if (hasError) {
        QMessageBox::warning(this, "检查结果", "发现冲突！红色格子存在重复数字。");
    } else {
        QMessageBox::information(this, "检查结果", "目前没有发现冲突，继续加油！");
    }
}

// ==================== 尺寸提示 ====================
QSize SudokuBoard::sizeHint() const
{
    return QSize(500, 500);
}

bool SudokuBoard::hasSelection() const
{
    return m_selectedCell != nullptr;
}

int SudokuBoard::selectedRow() const
{
    return m_selectedCell ? m_currentRow : -1;
}

int SudokuBoard::selectedCol() const
{
    return m_selectedCell ? m_currentCol : -1;
}
