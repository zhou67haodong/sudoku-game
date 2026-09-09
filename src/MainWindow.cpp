/**
 * @file MainWindow.cpp
 * @brief 主窗口实现（升级版） - 三栏布局，整合四位成员模块
 * @author 周浩东
 * @date 2026-08-31
 *
 * 升级内容：
 * - 三栏布局：左侧信息面板（大字计时/进度条/统计）、中央棋盘、右侧输入面板
 * - 新增操作：笔记模式(N)、撤销(Ctrl+Z)、暂停(P)、擦除
 * - 胜利结算对话框美化（统计 + 排行榜录入）
 * - 背景音乐开关、数字剩余计数联动
 */

#include "MainWindow.h"
#include "SudokuBoard.h"
#include "NumberPad.h"
#include "SudokuGenerator.h"
#include "GameState.h"
#include "HighScore.h"
#include "ThemeManager.h"
#include "SoundManager.h"
#include "SettingsDialog.h"
#include "AboutDialog.h"

#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QActionGroup>
#include <QToolBar>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QWidget>
#include <QTime>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QLineEdit>
#include <QDialog>
#include <QFrame>
#include <QProgressBar>
#include <QPushButton>
#include <QGroupBox>

// ==================== 构造函数 ====================
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_board(nullptr)
    , m_numberPad(nullptr)
    , m_generator(new SudokuGenerator())
    , m_gameState(new GameState(this))
    , m_highScore(new HighScore(this))
    , m_theme(new ThemeManager(this))
    , m_timerLabel(nullptr)
    , m_difficultyLabel(nullptr)
    , m_errorLabel(nullptr)
    , m_hintLabel(nullptr)
    , m_progressBar(nullptr)
    , m_pauseButton(nullptr)
    , m_noteButton(nullptr)
    , m_undoButton(nullptr)
    , m_noteAction(nullptr)
    , m_timer(new QTimer(this))
    , m_elapsed(0)
    , m_difficulty(0)
    , m_hintCount(0)
    , m_errorCount(0)
{
    setWindowTitle("数独游戏 - Sudoku Game");

    // 升级：三栏横向布局
    QWidget *central = new QWidget(this);
    setCentralWidget(central);
    QHBoxLayout *mainLayout = new QHBoxLayout(central);
    mainLayout->setContentsMargins(12, 8, 12, 8);
    mainLayout->setSpacing(12);

    m_board = new SudokuBoard(this);
    m_numberPad = new NumberPad(this);

    mainLayout->addWidget(buildSidePanel());
    mainLayout->addWidget(m_board, 0, Qt::AlignVCenter | Qt::AlignHCenter);
    mainLayout->addWidget(buildInputPanel());

    // 升级布局：横向三栏
    resize(1120, 720);
    setMinimumSize(1080, 660);

    setupMenu();
    setupToolBar();
    setupStatusBar();

    // 信号槽连接（周浩东：UI 交互；张啸林/马广东/靳凯：逻辑与辅助）
    connect(m_numberPad, &NumberPad::numberSelected,
            this, &MainWindow::onNumberSelected);
    connect(m_board, &SudokuBoard::progressUpdated,
            this, &MainWindow::onProgressUpdated);
    connect(m_board, &SudokuBoard::gameCompleted,
            this, &MainWindow::onGameCompleted);
    connect(m_board, &SudokuBoard::cellChanged,
            this, &MainWindow::onCellChanged);
    connect(m_board, &SudokuBoard::undoAvailabilityChanged,
            this, &MainWindow::onUndoAvailability);
    connect(m_board, &SudokuBoard::noteModeChanged,
            this, &MainWindow::onNoteModeChanged);

    connect(m_timer, &QTimer::timeout, this, &MainWindow::onTimerTick);

    // 启动时让棋盘获得焦点
    m_board->setFocus();

    // 默认开局（简单）
    startNewGame(0);

    // 升级：启动时自动播放背景音乐（若用户未关闭音乐开关）
    SoundManager::instance()->playMusic();
}

// ==================== 析构函数 ====================
MainWindow::~MainWindow()
{
    // 子对象由 Qt 父子机制自动回收
}

// ==================== 左侧信息面板（升级新增） ====================
QWidget *MainWindow::buildSidePanel()
{
    QWidget *panel = new QWidget(this);
    panel->setFixedWidth(210);
    QVBoxLayout *lay = new QVBoxLayout(panel);
    lay->setContentsMargins(10, 14, 10, 10);
    lay->setSpacing(10);

    panel->setStyleSheet(
        "QWidget#sidePanel { background-color: #FFFFFF; border-radius: 12px; }"
    );

    // 标题
    QLabel *title = new QLabel("数 独", this);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet(
        "font-size: 26px; font-weight: bold; color: #1E293B;"
        "letter-spacing: 12px; background: transparent;"
    );
    lay->addWidget(title);

    QLabel *subtitle = new QLabel("SUDOKU GAME", this);
    subtitle->setAlignment(Qt::AlignCenter);
    subtitle->setStyleSheet("font-size: 11px; color: #94A3B8; letter-spacing: 3px; background: transparent;");
    lay->addWidget(subtitle);

    lay->addSpacing(6);

    // 分割线
    QFrame *line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setStyleSheet("color: #E2E8F0;");
    lay->addWidget(line);

    // 当前难度
    m_difficultyLabel = new QLabel("难度：简单", this);
    m_difficultyLabel->setStyleSheet("font-size: 15px; font-weight: bold; color: #3B82F6; background: transparent;");
    lay->addWidget(m_difficultyLabel);

    // 大字计时
    m_timerLabel = new QLabel("00:00", this);
    m_timerLabel->setAlignment(Qt::AlignCenter);
    m_timerLabel->setStyleSheet(
        "font-size: 42px; font-weight: bold; color: #0F172A;"
        "font-family: 'Consolas', 'Microsoft YaHei'; background: transparent;"
    );
    lay->addWidget(m_timerLabel);

    // 进度条
    m_progressBar = new QProgressBar(this);
    m_progressBar->setRange(0, 100);
    m_progressBar->setValue(0);
    m_progressBar->setTextVisible(true);
    m_progressBar->setStyleSheet(
        "QProgressBar {"
        "    border: none;"
        "    background-color: #F1F5F9;"
        "    border-radius: 8px;"
        "    height: 16px;"
        "    text-align: center;"
        "    color: #475569;"
        "    font-size: 11px;"
        "}"
        "QProgressBar::chunk {"
        "    background-color: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "        stop:0 #60A5FA, stop:1 #3B82F6);"
        "    border-radius: 8px;"
        "}"
    );
    lay->addWidget(m_progressBar);

    lay->addSpacing(4);

    // 统计信息
    m_errorLabel = new QLabel("冲突：0 处", this);
    m_errorLabel->setStyleSheet("font-size: 13px; color: #DC2626; background: transparent;");
    lay->addWidget(m_errorLabel);

    m_hintLabel = new QLabel("提示：已用 0 次", this);
    m_hintLabel->setStyleSheet("font-size: 13px; color: #D97706; background: transparent;");
    lay->addWidget(m_hintLabel);

    lay->addStretch(1);

    // 快捷键说明
    QLabel *help = new QLabel(
        "快捷键：\n"
        "  1-9  填入数字\n"
        "  0/⌫  擦除\n"
        "  N    笔记模式\n"
        "  Ctrl+Z  撤销\n"
        "  P    暂停/继续\n"
        "  方向键  移动", this);
    help->setStyleSheet(
        "font-size: 12px; color: #64748B; background: transparent;"
        "line-height: 20px;"
    );
    lay->addWidget(help);

    // 排行榜按钮
    QPushButton *rankBtn = new QPushButton("🏆 排行榜", this);
    rankBtn->setCursor(Qt::PointingHandCursor);
    rankBtn->setStyleSheet(
        "QPushButton {"
        "    background-color: #FEF3C7; color: #92400E;"
        "    border: 2px solid #FDE68A; border-radius: 8px;"
        "    padding: 8px; font-weight: bold;"
        "}"
        "QPushButton:hover { background-color: #FDE68A; }"
    );
    connect(rankBtn, &QPushButton::clicked, this, &MainWindow::onHighScores);
    lay->addWidget(rankBtn);

    return panel;
}

// ==================== 右侧输入面板（升级新增） ====================
QWidget *MainWindow::buildInputPanel()
{
    QWidget *panel = new QWidget(this);
    panel->setFixedWidth(230);
    QVBoxLayout *lay = new QVBoxLayout(panel);
    lay->setContentsMargins(4, 4, 4, 4);
    lay->setSpacing(10);

    lay->addWidget(m_numberPad, 0, Qt::AlignHCenter);
    lay->addSpacing(4);

    // 操作按钮组
    QGroupBox *ops = new QGroupBox("操作", this);
    ops->setStyleSheet(
        "QGroupBox {"
        "    font-weight: bold; color: #334155;"
        "    border: 1px solid #E2E8F0; border-radius: 10px;"
        "    margin-top: 12px; padding-top: 8px;"
        "}"
        "QGroupBox::title { subcontrol-origin: margin; left: 12px; }"
    );
    QGridLayout *opsLay = new QGridLayout(ops);
    opsLay->setSpacing(6);

    auto makeBtn = [this](const QString &text) {
        QPushButton *b = new QPushButton(text, this);
        b->setCursor(Qt::PointingHandCursor);
        b->setMinimumHeight(42);
        b->setStyleSheet(
            "QPushButton {"
            "    background-color: #FFFFFF; color: #1E293B;"
            "    border: 2px solid #CBD5E1; border-radius: 8px;"
            "    font-weight: bold; font-size: 13px;"
            "}"
            "QPushButton:hover { border-color: #3B82F6; color: #1D4ED8; }"
            "QPushButton:disabled { color: #CBD5E1; border-color: #E2E8F0; }"
        );
        return b;
    };

    m_noteButton = makeBtn("✏️ 笔记 (N)");
    connect(m_noteButton, &QPushButton::clicked, this, &MainWindow::onToggleNoteMode);
    opsLay->addWidget(m_noteButton, 0, 0, 1, 2);

    m_undoButton = makeBtn("↩️ 撤销 (Ctrl+Z)");
    m_undoButton->setEnabled(false);
    connect(m_undoButton, &QPushButton::clicked, this, &MainWindow::onUndo);
    opsLay->addWidget(m_undoButton, 1, 0, 1, 2);

    QPushButton *hintBtn = makeBtn("💡 提示");
    connect(hintBtn, &QPushButton::clicked, this, &MainWindow::onHint);
    opsLay->addWidget(hintBtn, 2, 0);

    QPushButton *checkBtn = makeBtn("✅ 检查");
    connect(checkBtn, &QPushButton::clicked, this, &MainWindow::onCheck);
    opsLay->addWidget(checkBtn, 2, 1);

    lay->addWidget(ops);
    lay->addStretch(1);

    return panel;
}

// ==================== 菜单栏 ====================
void MainWindow::setupMenu()
{
    QMenuBar *menuBar = new QMenuBar(this);
    setMenuBar(menuBar);

    // 文件菜单
    QMenu *fileMenu = menuBar->addMenu("文件(&F)");
    QAction *actNew = fileMenu->addAction("新建游戏(&N)");
    QAction *actLoad = fileMenu->addAction("加载(&L)");
    QAction *actSave = fileMenu->addAction("保存(&S)");
    fileMenu->addSeparator();
    QAction *actExit = fileMenu->addAction("退出(&E)");

    // 游戏菜单
    QMenu *gameMenu = menuBar->addMenu("游戏(&G)");
    QAction *actUndo = gameMenu->addAction("撤销(&U)");
    actUndo->setShortcut(QKeySequence::Undo);
    m_noteAction = gameMenu->addAction("笔记模式(&N)");
    m_noteAction->setCheckable(true);
    m_noteAction->setShortcut(QKeySequence(Qt::Key_N));
    QAction *actPause = gameMenu->addAction("暂停(&P)");
    actPause->setShortcut(QKeySequence(Qt::Key_P));
    gameMenu->addSeparator();
    QAction *actCheck = gameMenu->addAction("检查(&C)");
    QAction *actHint = gameMenu->addAction("提示(&H)");
    QAction *actSolve = gameMenu->addAction("解答(&S)");

    // 帮助菜单
    QMenu *helpMenu = menuBar->addMenu("帮助(&H)");
    QAction *actSettings = helpMenu->addAction("设置(&T)");
    QAction *actAbout = helpMenu->addAction("关于(&A)");

    connect(actNew, &QAction::triggered, this, [this]() { startNewGame(m_difficulty); });
    connect(actLoad, &QAction::triggered, this, &MainWindow::onLoad);
    connect(actSave, &QAction::triggered, this, &MainWindow::onSave);
    connect(actExit, &QAction::triggered, this, &MainWindow::onExit);
    connect(actUndo, &QAction::triggered, this, &MainWindow::onUndo);
    connect(m_noteAction, &QAction::toggled, this, [this](bool on) {
        if (m_board->isNoteMode() != on) m_board->setNoteMode(on);
    });
    connect(actPause, &QAction::triggered, this, &MainWindow::onTogglePause);
    connect(actCheck, &QAction::triggered, this, &MainWindow::onCheck);
    connect(actHint, &QAction::triggered, this, &MainWindow::onHint);
    connect(actSolve, &QAction::triggered, this, &MainWindow::onSolve);
    connect(actSettings, &QAction::triggered, this, &MainWindow::onSettings);
    connect(actAbout, &QAction::triggered, this, &MainWindow::onAbout);
}

// ==================== 工具栏 ====================
void MainWindow::setupToolBar()
{
    QToolBar *toolBar = new QToolBar("工具", this);
    toolBar->setMovable(false);
    toolBar->setStyleSheet(
        "QToolBar { spacing: 6px; padding: 4px; }"
        "QToolButton {"
        "    background-color: #FFFFFF; color: #1E293B;"
        "    border: 2px solid #CBD5E1; border-radius: 6px;"
        "    padding: 5px 12px; font-weight: bold;"
        "}"
        "QToolButton:hover { border-color: #3B82F6; color: #1D4ED8; }"
        "QToolButton:checked { background-color: #DBEAFE; border-color: #3B82F6; color: #1D4ED8; }"
    );
    addToolBar(toolBar);

    QActionGroup *group = new QActionGroup(this);
    group->setExclusive(true);
    QStringList names = {"🌱 简单", "🔥 中等", "💀 困难"};
    for (int i = 0; i < 3; ++i) {
        QAction *a = group->addAction(names[i]);
        a->setCheckable(true);
        a->setData(i);
        if (i == 0) a->setChecked(true);
        toolBar->addAction(a);
    }
    connect(group, &QActionGroup::triggered, this, [this](QAction *a) {
        onDifficultyChanged(a->data().toInt());
    });

    toolBar->addSeparator();

    QAction *actNew = toolBar->addAction("🔄 新游戏");
    connect(actNew, &QAction::triggered, this, [this]() { startNewGame(m_difficulty); });

    m_pauseButton = new QPushButton("⏸ 暂停", this);
    m_pauseButton->setCursor(Qt::PointingHandCursor);
    m_pauseButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #FFFFFF; color: #1E293B;"
        "    border: 2px solid #CBD5E1; border-radius: 6px;"
        "    padding: 5px 12px; font-weight: bold;"
        "}"
        "QPushButton:hover { border-color: #3B82F6; color: #1D4ED8; }"
    );
    connect(m_pauseButton, &QPushButton::clicked, this, &MainWindow::onTogglePause);
    toolBar->addWidget(m_pauseButton);

    toolBar->addSeparator();

    QAction *actMusic = toolBar->addAction("🎵 音乐");
    actMusic->setCheckable(true);
    actMusic->setChecked(true);
    connect(actMusic, &QAction::toggled, this, [this](bool on) {
        SoundManager::instance()->setMusicEnabled(on);
        if (on) SoundManager::instance()->playMusic();
    });
}

// ==================== 状态栏 ====================
void MainWindow::setupStatusBar()
{
    QStatusBar *bar = statusBar();
    bar->showMessage("欢迎来到数独游戏！点击格子填数字，N 切换笔记模式，Ctrl+Z 撤销。");
}

// ==================== 启动新游戏 ====================
void MainWindow::startNewGame(int difficulty)
{
    m_difficulty = difficulty;
    m_hintCount = 0;
    m_errorCount = 0;
    SudokuDifficulty diff = (difficulty == 0) ? SudokuDifficulty::Easy
                        : (difficulty == 1) ? SudokuDifficulty::Medium
                                            : SudokuDifficulty::Hard;

    // 张啸林：生成谜题与解答
    m_generator->generatePuzzle(diff);
    QVector<QVector<int>> puzzle = m_generator->getPuzzle();
    QVector<QVector<int>> solution = m_generator->getSolution();

    // 周浩东：棋盘加载
    m_board->setPaused(false);
    m_board->loadGame(puzzle, solution);

    // 马广东：状态管理
    int p[9][9], s[9][9];
    convertToArray(puzzle, p);
    convertToArray(solution, s);
    Difficulty gd = (difficulty == 0) ? Difficulty::Easy
                : (difficulty == 1) ? Difficulty::Medium
                                    : Difficulty::Hard;
    m_gameState->startNewGame(gd, p, s);

    // 重置计时与面板
    m_elapsed = 0;
    m_timer->start(1000);
    m_timerLabel->setText("00:00");
    m_difficultyLabel->setText(QString("难度：%1")
        .arg(QStringList{"简单", "中等", "困难"}[difficulty]));
    refreshStats();
    refreshNumberPad();
    SoundManager::instance()->playEffect(SoundType::CellSelect);
}

// ==================== 难度切换 ====================
void MainWindow::onDifficultyChanged(int difficulty)
{
    startNewGame(difficulty);
    statusBar()->showMessage(
        QString("难度已切换：%1").arg(QStringList{"简单","中等","困难"}[difficulty]), 3000);
}

// ==================== 数字输入 ====================
void MainWindow::onNumberSelected(int num)
{
    // 暂停时忽略输入
    if (m_board->isPaused()) return;

    // 将数字键盘的点击输入到当前选中的格子
    if (m_board->hasSelection()) {
        int r = m_board->selectedRow();
        int c = m_board->selectedCol();
        if (r >= 0 && c >= 0) {
            if (m_board->isNoteMode() && num >= 1) {
                // 笔记模式：由棋盘统一入口处理（toggleNote）
                m_board->inputNumber(r, c, num);
            } else {
                m_board->setValue(r, c, num);
            }
        }
    }
    m_numberPad->highlightButton(num);
    if (num == 0) {
        SoundManager::instance()->playEffect(SoundType::Error);
    } else {
        SoundManager::instance()->playEffect(SoundType::CellFill);
    }
    refreshNumberPad();

    // 数字键盘点击后把焦点交还棋盘，确保键盘输入继续有效
    m_board->setFocus();
}

// ==================== 进度更新 ====================
void MainWindow::onProgressUpdated(int filled, int total)
{
    if (m_progressBar && total > 0) {
        m_progressBar->setValue(int(double(filled) / total * 100));
    }
}

// ==================== 格子变化（升级：统计刷新） ====================
void MainWindow::onCellChanged(int row, int col, int value)
{
    Q_UNUSED(row); Q_UNUSED(col); Q_UNUSED(value);
    refreshStats();
    refreshNumberPad();
}

// ==================== 游戏完成（升级：美化结算） ====================
void MainWindow::onGameCompleted()
{
    m_timer->stop();
    m_board->setPaused(true);
    SoundManager::instance()->playEffect(SoundType::Victory);

    // 马广东：记录排行榜
    HighScore::Level lv = (m_difficulty == 0) ? HighScore::Level::Easy
                                : (m_difficulty == 1) ? HighScore::Level::Medium
                                                      : HighScore::Level::Hard;

    // 升级：自定义结算对话框
    QDialog dlg(this);
    dlg.setWindowTitle("🎉 恭喜通关");
    dlg.setFixedSize(360, 300);
    QVBoxLayout *lay = new QVBoxLayout(&dlg);
    lay->setSpacing(10);
    lay->setContentsMargins(24, 24, 24, 24);

    QLabel * trophy = new QLabel("🏆", &dlg);
    trophy->setAlignment(Qt::AlignCenter);
    trophy->setStyleSheet("font-size: 52px; background: transparent;");
    lay->addWidget(trophy);

    QString stats = QString(
        "<div style='font-size:16px; color:#1E293B; font-weight:bold;'>数独完成！</div>"
        "<div style='font-size:13px; color:#475569; margin-top:6px;'>"
        "难度：%1 &nbsp;|&nbsp; 用时：%2:%3<br>"
        "使用提示：%4 次 &nbsp;|&nbsp; 累计冲突：%5 次</div>")
        .arg(QStringList{"简单","中等","困难"}[m_difficulty])
        .arg(m_elapsed / 60, 2, 10, QChar('0'))
        .arg(m_elapsed % 60, 2, 10, QChar('0'))
        .arg(m_hintCount).arg(m_errorCount);
    QLabel *statLabel = new QLabel(stats, &dlg);
    statLabel->setAlignment(Qt::AlignCenter);
    lay->addWidget(statLabel);

    QLineEdit *nameEdit = new QLineEdit(&dlg);
    nameEdit->setPlaceholderText("留下你的大名（进入排行榜）");
    nameEdit->setStyleSheet(
        "QLineEdit { border: 2px solid #CBD5E1; border-radius: 6px;"
        "padding: 6px; font-size: 14px; }"
        "QLineEdit:focus { border-color: #3B82F6; }"
    );
    lay->addWidget(nameEdit);

    QPushButton *okBtn = new QPushButton("保存成绩", &dlg);
    okBtn->setStyleSheet(
        "QPushButton { background-color: #3B82F6; color: white;"
        "border: none; border-radius: 6px; padding: 9px; font-weight: bold; font-size: 14px; }"
        "QPushButton:hover { background-color: #2563EB; }"
    );
    lay->addWidget(okBtn);

    connect(okBtn, &QPushButton::clicked, &dlg, &QDialog::accept);
    dlg.exec();

    QString name = nameEdit->text().trimmed();
    if (name.isEmpty()) name = "无名英雄";
    m_highScore->saveScore(lv, name, m_elapsed);
}

// ==================== 计时 ====================
void MainWindow::onTimerTick()
{
    if (m_board->isPaused()) return; // 暂停时不计时
    ++m_elapsed;
    if (m_timerLabel) {
        int m = m_elapsed / 60, s = m_elapsed % 60;
        m_timerLabel->setText(QString("%1:%2")
            .arg(m, 2, 10, QChar('0')).arg(s, 2, 10, QChar('0')));
    }
}

// ==================== 检查 / 提示 / 求解 ====================
void MainWindow::onCheck()   { m_board->checkBoard(); }

void MainWindow::onHint()
{
    if (m_board->isPaused()) return;
    ++m_hintCount;
    m_board->giveHint();
    SoundManager::instance()->playEffect(SoundType::Hint);
    refreshStats();
}

void MainWindow::onSolve()
{
    if (QMessageBox::question(this, "确认", "确定自动解答？") != QMessageBox::Yes) return;
    QVector<QVector<int>> solution = m_board->getSolution();
    for (int r = 0; r < 9; ++r)
        for (int c = 0; c < 9; ++c)
            m_board->setValue(r, c, solution[r][c]);
    m_timer->stop();
}

// ==================== 撤销（升级新增） ====================
void MainWindow::onUndo()
{
    if (m_board->undo()) {
        SoundManager::instance()->playEffect(SoundType::CellSelect);
        refreshStats();
        refreshNumberPad();
    }
}

// ==================== 笔记模式切换（升级新增） ====================
void MainWindow::onToggleNoteMode()
{
    m_board->setNoteMode(!m_board->isNoteMode());
}

// ==================== 暂停（升级新增） ====================
void MainWindow::onTogglePause()
{
    bool paused = !m_board->isPaused();
    m_board->setPaused(paused);
    m_pauseButton->setText(paused ? "▶ 继续" : "⏸ 暂停");
    // 升级：暂停时暂停背景音乐，恢复时继续播放
    if (paused) SoundManager::instance()->stopMusic();
    else        SoundManager::instance()->playMusic();
    statusBar()->showMessage(paused ? "游戏已暂停" : "继续游戏！", 2000);
}

// ==================== 排行榜（升级新增） ====================
void MainWindow::onHighScores()
{
    // 马广东：排行榜模块
    QString text;
    QStringList levels = {"简单", "中等", "困难"};
    for (int i = 0; i < 3; ++i) {
        HighScore::Level lv = (i == 0) ? HighScore::Level::Easy
                            : (i == 1) ? HighScore::Level::Medium
                                       : HighScore::Level::Hard;
        text += QString("【%1】\n").arg(levels[i]);
        auto scores = m_highScore->getTopScores(lv, 5);
        if (scores.isEmpty()) {
            text += "  暂无记录\n";
        } else {
            for (int k = 0; k < scores.size(); ++k) {
                text += QString("  %1. %2  ——  %3 秒\n")
                            .arg(k + 1).arg(scores[k].name).arg(scores[k].time);
            }
        }
        text += "\n";
    }
    QMessageBox::information(this, "🏆 排行榜", text);
}

// ==================== 撤销可用性（升级新增） ====================
void MainWindow::onUndoAvailability(bool ok)
{
    if (m_undoButton) m_undoButton->setEnabled(ok);
}

// ==================== 笔记模式变化（升级新增） ====================
void MainWindow::onNoteModeChanged(bool on)
{
    if (m_noteButton) {
        m_noteButton->setText(on ? "✏️ 笔记开 (N)" : "✏️ 笔记 (N)");
        m_noteButton->setStyleSheet(m_noteButton->styleSheet());
    }
    if (m_noteAction) m_noteAction->setChecked(on);
    statusBar()->showMessage(on ? "笔记模式：输入数字将标记为候选小字" : "已退出笔记模式", 2500);
}

// ==================== 保存 / 加载 ====================
void MainWindow::onSave()
{
    QString file = QFileDialog::getSaveFileName(this, "保存游戏",
        m_gameState->getSaveDirectory(), "数独存档 (*.json)");
    if (!file.isEmpty()) {
        if (m_gameState->saveGame(file))
            QMessageBox::information(this, "成功", "游戏已保存！");
        else
            QMessageBox::warning(this, "失败", "保存失败！");
    }
}

void MainWindow::onLoad()
{
    QString file = QFileDialog::getOpenFileName(this, "加载游戏",
        m_gameState->getSaveDirectory(), "数独存档 (*.json)");
    if (!file.isEmpty()) {
        if (m_gameState->loadGame(file)) {
            QVector<QVector<int>> puzzle(9, QVector<int>(9));
            QVector<QVector<int>> solution(9, QVector<int>(9));
            const int (*p)[9] = m_gameState->getPuzzle();
            const int (*s)[9] = m_gameState->getSolution();
            for (int r = 0; r < 9; ++r)
                for (int c = 0; c < 9; ++c) { puzzle[r][c] = p[r][c]; solution[r][c] = s[r][c]; }
            m_board->loadGame(puzzle, solution);
            m_elapsed = m_gameState->getElapsedTime();
            QMessageBox::information(this, "成功", "游戏已加载！");
            refreshStats();
            refreshNumberPad();
        } else {
            QMessageBox::warning(this, "失败", "加载失败！");
        }
    }
}

// ==================== 设置 ====================
void MainWindow::onSettings()
{
    SettingsDialog dlg(this);
    connect(&dlg, &SettingsDialog::settingsChanged, this, &MainWindow::applySettings);
    dlg.exec();
}

void MainWindow::applySettings(const Settings &s)
{
    // 马广东：主题；靳凯：设置与音效
    m_theme->loadTheme(s.theme);
    SoundManager::instance()->setMuted(!s.soundOn);
    SoundManager::instance()->setMusicEnabled(s.musicOn);
    if (s.musicOn) SoundManager::instance()->playMusic();
    else SoundManager::instance()->stopMusic();
}

// ==================== 关于 ====================
void MainWindow::onAbout()
{
    AboutDialog dlg(this);
    dlg.exec();
}

// ==================== 退出 ====================
void MainWindow::onExit()
{
    if (QMessageBox::question(this, "退出", "确定退出数独游戏？",
        QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
        close();
}

// ==================== 统计刷新（升级新增） ====================
void MainWindow::refreshStats()
{
    // 冲突数 = 值不为 0 且与解不同 或 行列宫冲突，这里用简单口径：
    // 与解答不一致的非空可编辑格数视为"待修正"，冲突格以棋盘红色标记为准
    int conflicts = 0;
    QVector<QVector<int>> sol = m_board->getSolution();
    for (int r = 0; r < 9; ++r) {
        for (int c = 0; c < 9; ++c) {
            int v = m_board->getValue(r, c);
            if (v != 0 && v != sol[r][c]) ++conflicts;
        }
    }
    if (m_errorLabel)
        m_errorLabel->setText(QString("待修正：%1 处").arg(conflicts));
    if (m_hintLabel)
        m_hintLabel->setText(QString("提示：已用 %1 次").arg(m_hintCount));
}

// ==================== 数字键盘剩余刷新（升级新增） ====================
void MainWindow::refreshNumberPad()
{
    QVector<int> counts(10, 0);
    for (int n = 1; n <= 9; ++n) {
        counts[n] = m_board->numberCount(n);
    }
    m_numberPad->updateRemaining(counts);
}

// ==================== 工具函数 ====================
void MainWindow::convertToArray(const QVector<QVector<int>> &src, int dst[9][9])
{
    for (int r = 0; r < 9; ++r)
        for (int c = 0; c < 9; ++c)
            dst[r][c] = src[r][c];
}
