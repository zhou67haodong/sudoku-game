/**
 * @file MainWindow.h
 * @brief 数独游戏主窗口（升级版） - 整合四位成员模块的总控界面
 * @author 周浩东
 * @date 2026-08-31
 *
 * 整合说明（四人协作）：
 * - SudokuBoard / NumberPad（周浩东）：棋盘与输入控件
 * - SudokuGenerator / SudokuSolver / SudokuValidator（张啸林）：生成、求解、校验
 * - GameState / HighScore / ThemeManager（马广东）：状态、存档、排行榜、主题
 * - SettingsDialog / AboutDialog / SoundManager（靳凯）：设置、关于、音效、背景音乐
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QLabel>
#include <QProgressBar>

#include "SudokuBoard.h"
#include "NumberPad.h"
#include "SudokuGenerator.h"
#include "GameState.h"
#include "HighScore.h"
#include "ThemeManager.h"
#include "SoundManager.h"
#include "SettingsDialog.h"
#include "AboutDialog.h"

/**
 * @brief 主窗口类（升级版：三栏布局）
 *
 * 作为数独游戏的总控界面，负责：
 * - 左侧信息面板：难度、大字计时、进度条、统计信息、快捷键说明
 * - 中央棋盘 + 右侧数字键盘与操作按钮（笔记/撤销/提示/擦除/暂停）
 * - 菜单栏（文件 / 游戏 / 帮助）
 * - 串联各成员模块：生成、计时、存档、提示、求解、音效、主题、排行榜
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父窗口指针
     */
    explicit MainWindow(QWidget *parent = nullptr);

    /**
     * @brief 析构函数
     */
    ~MainWindow();

private slots:
    /** 难度切换后开始新游戏 */
    void onDifficultyChanged(int difficulty);
    /** 数字键盘选中数字 */
    void onNumberSelected(int num);
    /** 棋盘进度更新 */
    void onProgressUpdated(int filled, int total);
    /** 棋盘任意格子变化（更新统计） */
    void onCellChanged(int row, int col, int value);
    /** 游戏完成 */
    void onGameCompleted();
    /** 计时器跳动 */
    void onTimerTick();
    /** 检查棋盘 */
    void onCheck();
    /** 请求提示 */
    void onHint();
    /** 自动求解 */
    void onSolve();
    /** 保存游戏 */
    void onSave();
    /** 读取存档 */
    void onLoad();
    /** 打开设置 */
    void onSettings();
    /** 打开关于 */
    void onAbout();
    /** 退出 */
    void onExit();
    /** 应用设置（来自设置对话框） */
    void applySettings(const Settings &s);

    // ---------- 升级新增槽 ----------
    /** 撤销一步 */
    void onUndo();
    /** 切换笔记模式 */
    void onToggleNoteMode();
    /** 暂停/继续 */
    void onTogglePause();
    /** 查看排行榜 */
    void onHighScores();
    /** 撤销可用性变化 */
    void onUndoAvailability(bool ok);
    /** 笔记模式变化 */
    void onNoteModeChanged(bool on);

private:
    /** 初始化菜单栏 */
    void setupMenu();
    /** 初始化工具栏 */
    void setupToolBar();
    /** 初始化状态栏 */
    void setupStatusBar();
    /** 构建左侧信息面板（升级新增） */
    QWidget *buildSidePanel();
    /** 构建右侧输入面板（升级新增） */
    QWidget *buildInputPanel();
    /** 根据难度启动新一局 */
    void startNewGame(int difficulty);
    /** 将 QVector 棋盘转为 int[9][9]（供 GameState 使用） */
    void convertToArray(const QVector<QVector<int>> &src, int dst[9][9]);
    /** 刷新统计信息（错误数/已填/数字剩余） */
    void refreshStats();
    /** 刷新数字键盘剩余计数 */
    void refreshNumberPad();

    SudokuBoard      *m_board;         ///< 棋盘控件（周浩东）
    NumberPad        *m_numberPad;     ///< 数字键盘（周浩东）
    SudokuGenerator  *m_generator;     ///< 数独生成器（张啸林）
    GameState        *m_gameState;     ///< 游戏状态管理（马广东）
    HighScore        *m_highScore;     ///< 排行榜（马广东）
    ThemeManager     *m_theme;         ///< 主题管理（马广东）

    // 左侧信息面板控件（升级新增）
    QLabel      *m_timerLabel;      ///< 大字计时标签
    QLabel      *m_difficultyLabel; ///< 当前难度标签
    QLabel      *m_errorLabel;      ///< 错误统计标签
    QLabel      *m_hintLabel;       ///< 已用提示标签
    QProgressBar *m_progressBar;    ///< 进度条
    QPushButton *m_pauseButton;     ///< 暂停按钮（工具栏）
    QPushButton *m_noteButton;      ///< 笔记模式按钮
    QPushButton *m_undoButton;      ///< 撤销按钮
    QAction     *m_noteAction;      ///< 笔记模式工具栏动作

    QTimer           *m_timer;         ///< 计时器
    int               m_elapsed;       ///< 已用秒数
    int               m_difficulty;    ///< 当前难度（0简单/1中等/2困难）
    int               m_hintCount;     ///< 本局已用提示次数（升级新增）
    int               m_errorCount;    ///< 本局累计错误次数（升级新增）
};

#endif // MAINWINDOW_H
