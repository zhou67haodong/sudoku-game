/**
 * @file HighScore.h
 * @brief 排行榜管理 - 记录各难度下的最佳成绩
 * @author 马广东
 * @date 2026-09-02
 */

#ifndef HIGHSCORE_H
#define HIGHSCORE_H

#include <QObject>
#include <QVector>
#include <QString>
#include <QDateTime>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDir>

/**
 * @brief 单条成绩记录
 */
struct ScoreEntry {
    QString     name;   ///< 玩家名字
    int         time;   ///< 用时（秒，越小越好）
    QDateTime   date;   ///< 完成日期

    /**
     * @brief 序列化为 JSON 对象
     */
    QJsonObject toJson() const {
        QJsonObject obj;
        obj["name"] = name;
        obj["time"] = time;
        obj["date"] = date.toString(Qt::ISODate);
        return obj;
    }

    /**
     * @brief 从 JSON 对象反序列化
     */
    static ScoreEntry fromJson(const QJsonObject &obj) {
        ScoreEntry e;
        e.name = obj["name"].toString();
        e.time = obj["time"].toInt();
        e.date = QDateTime::fromString(obj["date"].toString(), Qt::ISODate);
        return e;
    }
};

/**
 * @class HighScore
 * @brief 排行榜管理器，按难度分别保存/读取前 N 名
 * @author 马广东
 */
class HighScore : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief 难度键（用于文件内分组）
     */
    enum class Level { Easy, Medium, Hard };

    /**
     * @brief 构造函数
     * @param parent 父对象
     */
    explicit HighScore(QObject *parent = nullptr);

    /**
     * @brief 加载所有成绩（启动程序时调用）
     */
    void loadScores();

    /**
     * @brief 保存一条新成绩
     * @param level 难度
     * @param name 玩家名
     * @param time 用时（秒）
     */
    void saveScore(Level level, const QString &name, int time);

    /**
     * @brief 获取某难度的前 N 名
     * @param level 难度
     * @param count 返回数量（默认 10）
     * @return 成绩列表（按时间升序）
     */
    QVector<ScoreEntry> getTopScores(Level level, int count = 10) const;

    /**
     * @brief 查询某成绩在某难度的排名（从 1 开始）
     * @param level 难度
     * @param time 用时
     * @return 排名；若未进前 100 返回 -1
     */
    int getRank(Level level, int time) const;

    /**
     * @brief 获取排行榜文件路径
     */
    QString getFilePath() const;

private:
    /**
     * @brief 难度转字符串键
     */
    QString levelKey(Level level) const;

    /**
     * @brief 写回文件
     */
    void persist();

    QJsonObject m_data;  ///< 完整数据结构（按难度分组）
};

#endif // HIGHSCORE_H
