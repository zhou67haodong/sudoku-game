#include <QJsonArray>
/**
 * @file GameState.cpp
 * @brief 游戏状态管理类实现 - 计时、存档、读档、统计
 * @author 马广东
 * @date 2026-09-02
 */

#include "GameState.h"

/**
 * @brief 构造函数，初始化计时器与状态变量
 */
GameState::GameState(QObject *parent)
    : QObject(parent)
    , m_timer(new QTimer(this))
    , m_elapsedSeconds(0)
    , m_isPaused(false)
    , m_isActive(false)
    , m_moveCount(0)
    , m_hintCount(0)
    , m_checkCount(0)
    , m_difficulty(Difficulty::Easy)
{
    // 连接计时器
    connect(m_timer, &QTimer::timeout, this, &GameState::onTimerTick);
    m_timer->setInterval(1000); // 每秒触发一次

    // 初始化棋盘
    for (int r = 0; r < 9; ++r) {
        for (int c = 0; c < 9; ++c) {
            m_board[r][c] = 0;
            m_puzzle[r][c] = 0;
            m_solution[r][c] = 0;
            m_fixedCells[r][c] = false;
        }
    }
}

/**
 * @brief 析构函数
 */
GameState::~GameState()
{
    if (m_timer && m_timer->isActive()) {
        m_timer->stop();
    }
}

/**
 * @brief 开始新游戏
 */
void GameState::startNewGame(Difficulty difficulty,
                             const int puzzle[9][9],
                             const int solution[9][9])
{
    m_difficulty = difficulty;
    m_elapsedSeconds = 0;
    m_moveCount = 0;
    m_hintCount = 0;
    m_checkCount = 0;
    m_history.clear();

    for (int r = 0; r < 9; ++r) {
        for (int c = 0; c < 9; ++c) {
            m_puzzle[r][c] = puzzle[r][c];
            m_solution[r][c] = solution[r][c];
            m_board[r][c] = puzzle[r][c];
            m_fixedCells[r][c] = (puzzle[r][c] != 0);
        }
    }

    m_isActive = true;
    m_isPaused = false;
    m_timer->start();
    emit timerUpdated(m_elapsedSeconds);
    emit gameStateChanged();
}

/**
 * @brief 暂停游戏
 */
void GameState::pauseGame()
{
    if (m_isActive && !m_isPaused) {
        m_isPaused = true;
        m_timer->stop();
        emit gamePaused(true);
    }
}

/**
 * @brief 恢复游戏
 */
void GameState::resumeGame()
{
    if (m_isActive && m_isPaused) {
        m_isPaused = false;
        m_timer->start();
        emit gamePaused(false);
    }
}

/**
 * @brief 重置游戏
 */
void GameState::resetGame()
{
    m_isActive = false;
    m_isPaused = false;
    m_timer->stop();
    m_elapsedSeconds = 0;
    m_moveCount = 0;
    m_hintCount = 0;
    m_checkCount = 0;
    m_history.clear();
    for (int r = 0; r < 9; ++r) {
        for (int c = 0; c < 9; ++c) {
            m_board[r][c] = 0;
            m_puzzle[r][c] = 0;
            m_solution[r][c] = 0;
            m_fixedCells[r][c] = false;
        }
    }
    emit gameStateChanged();
}

/**
 * @brief 记录用户操作
 */
void GameState::recordAction(ActionType action, int row, int col, int oldValue, int newValue)
{
    QJsonObject rec;
    rec["action"] = static_cast<int>(action);
    rec["row"] = row;
    rec["col"] = col;
    rec["old"] = oldValue;
    rec["new"] = newValue;
    rec["time"] = m_elapsedSeconds;
    m_history.append(rec);

    switch (action) {
    case ActionType::FillNumber:
    case ActionType::EraseNumber:
        ++m_moveCount;
        break;
    case ActionType::UseHint:
        ++m_hintCount;
        break;
    case ActionType::CheckBoard:
        ++m_checkCount;
        break;
    default:
        break;
    }
    emit gameStateChanged();
}

/**
 * @brief 获取游戏统计信息
 */
GameStatistics GameState::getStatistics() const
{
    GameStatistics s;
    s.elapsedSeconds = m_elapsedSeconds;
    s.moveCount = m_moveCount;
    s.hintCount = m_hintCount;
    s.checkCount = m_checkCount;
    s.difficulty = m_difficulty;
    s.isCompleted = checkWin();
    return s;
}

/**
 * @brief 获取已用时间
 */
int GameState::getElapsedTime() const
{
    return m_elapsedSeconds;
}

/**
 * @brief 获取当前难度
 */
Difficulty GameState::getDifficulty() const
{
    return m_difficulty;
}

/**
 * @brief 获取棋盘数据（返回指针）
 */
const int (*GameState::getBoard() const)[9]
{
    return m_board;
}

/**
 * @brief 获取初始谜题
 */
const int (*GameState::getPuzzle() const)[9]
{
    return m_puzzle;
}

/**
 * @brief 获取正确答案
 */
const int (*GameState::getSolution() const)[9]
{
    return m_solution;
}

/**
 * @brief 设置单元格值
 */
bool GameState::setCellValue(int row, int col, int value)
{
    if (row < 0 || row > 8 || col < 0 || col > 8) return false;
    if (m_fixedCells[row][col]) return false; // 固定格子不可改
    if (value < 0 || value > 9) return false;
    int old = m_board[row][col];
    m_board[row][col] = value;
    recordAction(value == 0 ? ActionType::EraseNumber : ActionType::FillNumber,
                 row, col, old, value);
    emit cellValueChanged(row, col, value);
    if (checkWin()) {
        emit gameWon();
    }
    return true;
}

/**
 * @brief 获取单元格值
 */
int GameState::getCellValue(int row, int col) const
{
    if (row < 0 || row > 8 || col < 0 || col > 8) return 0;
    return m_board[row][col];
}

/**
 * @brief 判断单元格是否固定
 */
bool GameState::isCellFixed(int row, int col) const
{
    if (row < 0 || row > 8 || col < 0 || col > 8) return false;
    return m_fixedCells[row][col];
}

/**
 * @brief 保存游戏到 JSON 文件
 */
bool GameState::saveGame(const QString &filename)
{
    QJsonObject root;
    root["elapsed"] = m_elapsedSeconds;
    root["difficulty"] = difficultyToString(m_difficulty);

    QJsonArray boardArr, puzzleArr, solutionArr, fixedArr;
    for (int r = 0; r < 9; ++r) {
        QJsonArray rowB, rowP, rowS, rowF;
        for (int c = 0; c < 9; ++c) {
            rowB.append(m_board[r][c]);
            rowP.append(m_puzzle[r][c]);
            rowS.append(m_solution[r][c]);
            rowF.append(m_fixedCells[r][c] ? 1 : 0);
        }
        boardArr.append(rowB);
        puzzleArr.append(rowP);
        solutionArr.append(rowS);
        fixedArr.append(rowF);
    }
    root["board"] = boardArr;
    root["puzzle"] = puzzleArr;
    root["solution"] = solutionArr;
    root["fixed"] = fixedArr;
    {
        QJsonArray histArr;
        for (const QJsonObject &obj : m_history) histArr.append(obj);
        root["history"] = histArr;
    }

    QJsonDocument doc(root);
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }
    file.write(doc.toJson());
    file.close();
    return true;
}

/**
 * @brief 从 JSON 文件加载游戏
 */
bool GameState::loadGame(const QString &filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }
    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull() || !doc.isObject()) return false;
    QJsonObject root = doc.object();

    m_elapsedSeconds = root["elapsed"].toInt();
    m_difficulty = stringToDifficulty(root["difficulty"].toString());
    m_moveCount = 0; m_hintCount = 0; m_checkCount = 0;

    QJsonArray boardArr = root["board"].toArray();
    QJsonArray puzzleArr = root["puzzle"].toArray();
    QJsonArray solutionArr = root["solution"].toArray();
    QJsonArray fixedArr = root["fixed"].toArray();

    for (int r = 0; r < 9 && r < boardArr.size(); ++r) {
        QJsonArray rowB = boardArr[r].toArray();
        QJsonArray rowP = puzzleArr[r].toArray();
        QJsonArray rowS = solutionArr[r].toArray();
        QJsonArray rowF = fixedArr[r].toArray();
        for (int c = 0; c < 9 && c < rowB.size(); ++c) {
            m_board[r][c] = rowB[c].toInt();
            m_puzzle[r][c] = rowP[c].toInt();
            m_solution[r][c] = rowS[c].toInt();
            m_fixedCells[r][c] = rowF[c].toInt() != 0;
        }
    }

    m_history = root["history"].toVariant().value<QVector<QJsonObject>>();
    m_isActive = true;
    m_isPaused = false;
    m_timer->start();
    emit gameStateChanged();
    emit timerUpdated(m_elapsedSeconds);
    return true;
}

/**
 * @brief 获取存档目录
 */
QString GameState::getSaveDirectory() const
{
    QString dir = QDir::homePath() + "/SudokuSaves";
    QDir d(dir);
    if (!d.exists()) {
        d.mkpath(".");
    }
    return dir;
}

/**
 * @brief 检查是否胜利
 */
bool GameState::checkWin() const
{
    for (int r = 0; r < 9; ++r) {
        for (int c = 0; c < 9; ++c) {
            if (m_board[r][c] != m_solution[r][c]) {
                return false;
            }
        }
    }
    return true;
}

/**
 * @brief 计时器槽函数
 */
void GameState::onTimerTick()
{
    if (!m_isPaused) {
        ++m_elapsedSeconds;
        emit timerUpdated(m_elapsedSeconds);
    }
}

/**
 * @brief 难度转字符串
 */
QString GameState::difficultyToString(Difficulty difficulty) const
{
    switch (difficulty) {
    case Difficulty::Easy:   return "Easy";
    case Difficulty::Medium: return "Medium";
    case Difficulty::Hard:   return "Hard";
    default:                 return "Easy";
    }
}

/**
 * @brief 字符串转难度
 */
Difficulty GameState::stringToDifficulty(const QString &str) const
{
    if (str == "Medium") return Difficulty::Medium;
    if (str == "Hard")   return Difficulty::Hard;
    return Difficulty::Easy;
}
