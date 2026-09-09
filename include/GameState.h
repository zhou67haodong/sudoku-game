/**
 * @file GameState.h
 * @brief 游戏状态管理类 - 负责计时、存档、读档、统计
 * @author 马广东
 * @date 2026-09-02
 */

#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <QObject>
#include <QTimer>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>

/**
 * @enum Difficulty
 * @brief 游戏难度级别
 */
enum class Difficulty {
    Easy,   ///< 简单 - 35~40个提示数字
    Medium, ///< 中等 - 30~34个提示数字
    Hard    ///< 困难 - 25~29个提示数字
};

/**
 * @enum ActionType
 * @brief 用户操作类型
 */
enum class ActionType {
    FillNumber,   ///< 填入数字
    EraseNumber,  ///< 擦除数字
    UseHint,      ///< 使用提示
    CheckBoard,   ///< 检查棋盘
    UndoAction    ///< 撤销操作
};

/**
 * @struct GameStatistics
 * @brief 游戏统计数据结构
 */
struct GameStatistics {
    int elapsedSeconds;   ///< 用时（秒）
    int moveCount;        ///< 移动次数
    int hintCount;        ///< 使用提示次数
    int checkCount;       ///< 检查次数
    Difficulty difficulty;///< 难度级别
    bool isCompleted;     ///< 是否完成
};

/**
 * @struct GameCellState
 * @brief 单元格状态（用于存档）
 */
struct GameCellState {
    int value;       ///< 当前值（0表示空）
    bool isFixed;    ///< 是否为固定数字
};

/**
 * @class GameState
 * @brief 游戏状态管理类
 * 
 * 负责管理游戏的完整状态，包括：
 * - 棋盘数据（9x9矩阵）
 * - 计时器
 * - 游戏统计（移动次数、提示次数等）
 * - 存档与读档
 * 
 * @author 马广东
 */
class GameState : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父对象指针
     */
    explicit GameState(QObject* parent = nullptr);

    /**
     * @brief 析构函数
     */
    ~GameState();

    /**
     * @brief 开始新游戏
     * @param difficulty 游戏难度
     * @param puzzle 初始谜题（9x9矩阵，0表示空格）
     * @param solution 正确答案（9x9矩阵）
     */
    void startNewGame(Difficulty difficulty, const int puzzle[9][9], const int solution[9][9]);

    /**
     * @brief 暂停游戏
     */
    void pauseGame();

    /**
     * @brief 恢复游戏
     */
    void resumeGame();

    /**
     * @brief 重置游戏
     */
    void resetGame();

    /**
     * @brief 记录用户操作
     * @param action 操作类型
     * @param row 行索引
     * @param col 列索引
     * @param oldValue 旧值
     * @param newValue 新值
     */
    void recordAction(ActionType action, int row, int col, int oldValue, int newValue);

    /**
     * @brief 获取游戏统计信息
     * @return 游戏统计结构体
     */
    GameStatistics getStatistics() const;

    /**
     * @brief 获取已用时间（秒）
     * @return 秒数
     */
    int getElapsedTime() const;

    /**
     * @brief 获取当前难度
     * @return 难度级别
     */
    Difficulty getDifficulty() const;

    /**
     * @brief 获取棋盘数据
     * @return 当前棋盘（9x9矩阵）
     */
    const int (*getBoard() const)[9];

    /**
     * @brief 获取初始谜题
     * @return 谜题矩阵（9x9矩阵）
     */
    const int (*getPuzzle() const)[9];

    /**
     * @brief 获取正确答案
     * @return 解答矩阵（9x9矩阵）
     */
    const int (*getSolution() const)[9];

    /**
     * @brief 设置单元格值
     * @param row 行索引
     * @param col 列索引
     * @param value 值（0表示清除）
     * @return 是否设置成功
     */
    bool setCellValue(int row, int col, int value);

    /**
     * @brief 获取单元格值
     * @param row 行索引
     * @param col 列索引
     * @return 单元格值
     */
    int getCellValue(int row, int col) const;

    /**
     * @brief 判断单元格是否固定
     * @param row 行索引
     * @param col 列索引
     * @return 是否固定
     */
    bool isCellFixed(int row, int col) const;

    /**
     * @brief 保存游戏到文件
     * @param filename 文件名
     * @return 是否保存成功
     */
    bool saveGame(const QString& filename);

    /**
     * @brief 从文件加载游戏
     * @param filename 文件名
     * @return 是否加载成功
     */
    bool loadGame(const QString& filename);

    /**
     * @brief 获取存档目录
     * @return 存档目录路径
     */
    QString getSaveDirectory() const;

    /**
     * @brief 检查游戏是否胜利
     * @return 是否胜利
     */
    bool checkWin() const;

signals:
    /**
     * @brief 计时器更新信号
     * @param seconds 已用秒数
     */
    void timerUpdated(int seconds);

    /**
     * @brief 游戏胜利信号
     */
    void gameWon();

    /**
     * @brief 游戏状态改变信号
     */
    void gameStateChanged();

    /**
     * @brief 单元格值改变信号
     * @param row 行索引
     * @param col 列索引
     * @param value 新值
     */
    void cellValueChanged(int row, int col, int value);

    /**
     * @brief 游戏暂停/恢复信号
     * @param isPaused 是否暂停
     */
    void gamePaused(bool isPaused);

private slots:
    /**
     * @brief 计时器更新槽函数
     */
    void onTimerTick();

private:
    /**
     * @brief 将难度转换为字符串
     * @param difficulty 难度级别
     * @return 难度字符串
     */
    QString difficultyToString(Difficulty difficulty) const;

    /**
     * @brief 将字符串转换为难度
     * @param str 难度字符串
     * @return 难度级别
     */
    Difficulty stringToDifficulty(const QString& str) const;

    QTimer* m_timer;              ///< 计时器
    int m_elapsedSeconds;         ///< 已用时间（秒）
    bool m_isPaused;              ///< 是否暂停
    bool m_isActive;              ///< 游戏是否在进行中

    int m_board[9][9];            ///< 当前棋盘
    int m_puzzle[9][9];           ///< 初始谜题
    int m_solution[9][9];         ///< 正确答案
    bool m_fixedCells[9][9];      ///< 固定单元格标记

    int m_moveCount;              ///< 移动次数
    int m_hintCount;              ///< 提示次数
    int m_checkCount;             ///< 检查次数
    Difficulty m_difficulty;      ///< 当前难度

    QVector<QJsonObject> m_history; ///< 操作历史（用于撤销）
};

#endif // GAMESTATE_H
