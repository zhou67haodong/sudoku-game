/**
 * @file HighScore.cpp
 * @brief 排行榜管理实现
 * @author 马广东
 * @date 2026-09-02
 */

#include "HighScore.h"
#include <algorithm>

/**
 * @brief 构造函数，自动加载已有成绩
 */
HighScore::HighScore(QObject *parent)
    : QObject(parent)
{
    loadScores();
}

/**
 * @brief 加载成绩文件；若不存在则初始化空结构
 */
void HighScore::loadScores()
{
    QString path = getFilePath();
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        // 文件不存在 → 初始化空分组
        m_data = QJsonObject();
        m_data["Easy"]   = QJsonArray();
        m_data["Medium"] = QJsonArray();
        m_data["Hard"]   = QJsonArray();
        return;
    }
    QByteArray bytes = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(bytes);
    if (doc.isNull() || !doc.isObject()) {
        m_data = QJsonObject();
        m_data["Easy"]   = QJsonArray();
        m_data["Medium"] = QJsonArray();
        m_data["Hard"]   = QJsonArray();
        return;
    }
    m_data = doc.object();
    // 确保三个难度键都存在
    if (!m_data.contains("Easy"))   m_data["Easy"]   = QJsonArray();
    if (!m_data.contains("Medium")) m_data["Medium"] = QJsonArray();
    if (!m_data.contains("Hard"))   m_data["Hard"]   = QJsonArray();
}

/**
 * @brief 保存一条成绩并写回文件
 */
void HighScore::saveScore(Level level, const QString &name, int time)
{
    QString key = levelKey(level);
    QJsonArray arr = m_data[key].toArray();

    ScoreEntry entry;
    entry.name = name.isEmpty() ? "玩家" : name;
    entry.time = time;
    entry.date = QDateTime::currentDateTime();
    arr.append(entry.toJson());

    // 按时间升序排序（用时越短越靠前）
    QVector<ScoreEntry> list;
    for (const QJsonValue &v : arr) {
        list.append(ScoreEntry::fromJson(v.toObject()));
    }
    std::sort(list.begin(), list.end(), [](const ScoreEntry &a, const ScoreEntry &b) {
        return a.time < b.time;
    });

    // 仅保留前 100 条
    QJsonArray sorted;
    for (int i = 0; i < list.size() && i < 100; ++i) {
        sorted.append(list[i].toJson());
    }
    m_data[key] = sorted;
    persist();
}

/**
 * @brief 获取前 N 名
 */
QVector<ScoreEntry> HighScore::getTopScores(Level level, int count) const
{
    QVector<ScoreEntry> result;
    QString key = levelKey(level);
    QJsonArray arr = m_data[key].toArray();
    for (int i = 0; i < arr.size() && i < count; ++i) {
        result.append(ScoreEntry::fromJson(arr[i].toObject()));
    }
    return result;
}

/**
 * @brief 查询排名
 */
int HighScore::getRank(Level level, int time) const
{
    QString key = levelKey(level);
    QJsonArray arr = m_data[key].toArray();
    int rank = 1;
    for (const QJsonValue &v : arr) {
        ScoreEntry e = ScoreEntry::fromJson(v.toObject());
        if (e.time <= time) {
            ++rank;
        }
    }
    // 若 rank 超过 100 视为未上榜
    return rank <= 100 ? rank : -1;
}

/**
 * @brief 排行榜文件路径（用户文档目录）
 */
QString HighScore::getFilePath() const
{
    QString dir = QDir::homePath() + "/SudokuSaves";
    QDir d(dir);
    if (!d.exists()) d.mkpath(".");
    return dir + "/highscores.json";
}

/**
 * @brief 难度键名
 */
QString HighScore::levelKey(Level level) const
{
    switch (level) {
    case Level::Easy:   return "Easy";
    case Level::Medium: return "Medium";
    case Level::Hard:   return "Hard";
    default:            return "Easy";
    }
}

/**
 * @brief 持久化到文件
 */
void HighScore::persist()
{
    QString path = getFilePath();
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        return;
    }
    QJsonDocument doc(m_data);
    file.write(doc.toJson());
    file.close();
}
