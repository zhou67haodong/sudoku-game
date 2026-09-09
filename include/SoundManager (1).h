/**
 * @file SoundManager.h
 * @brief 音效管理器 - 统一管理游戏反馈音与背景音乐（单例，升级版）
 * @author 靳凯
 * @date 2026-09-03
 */

#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include <QObject>
#include <QMap>
#include <QString>

class QSoundEffect;

/**
 * @brief 音效类型
 */
enum class SoundType {
    CellSelect,  ///< 选中格子
    CellFill,    ///< 正确填入数字
    Error,       ///< 出错
    Hint,        ///< 使用提示
    Undo,        ///< 撤销（升级新增）
    Victory      ///< 通关
};

/**
 * @class SoundManager
 * @brief 音效管理器（单例模式，升级版）
 *
 * 通过 QSoundEffect 播放 resources/ 下的 .wav 音效文件。
 * 升级新增：背景音乐（bgm.wav）无缝循环播放与独立开关。
 * 若资源缺失则静默跳过，保证程序不崩溃。
 * @author 靳凯
 */
class SoundManager : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief 获取单例实例
     */
    static SoundManager *instance();

    /**
     * @brief 播放指定类型音效
     * @param type 音效类型
     */
    void playEffect(SoundType type);

    /**
     * @brief 设置音效静音
     * @param muted true 静音
     */
    void setMuted(bool muted);

    /**
     * @brief 设置音效音量
     * @param volume 0~100
     */
    void setVolume(int volume);

    /**
     * @brief 是否已静音
     */
    bool isMuted() const;

    // ---------- 升级新增：背景音乐 ----------
    /** @brief 播放背景音乐（循环，淡入） */
    void playMusic();
    /** @brief 停止背景音乐 */
    void stopMusic();
    /** @brief 背景音乐开关 */
    void setMusicEnabled(bool on);
    bool isMusicEnabled() const { return m_musicEnabled; }

signals:
    void mutedChanged(bool muted);

private:
    /**
     * @brief 私有构造函数（单例）
     */
    explicit SoundManager(QObject *parent = nullptr);

    /**
     * @brief 初始化音效资源映射（资源缺失时安全降级）
     */
    void initSounds();

    /**
     * @brief 创建并缓存一个音效播放器实例
     * @param path 资源路径（:/sounds/xxx.wav）
     * @return 缓存实例，资源缺失时返回 nullptr
     */
    QSoundEffect *createEffect(const QString &path);

    bool m_muted;             ///< 是否静音
    int  m_volume;            ///< 音量 0~100
    QMap<SoundType, QString> m_soundFiles; ///< 类型→资源路径
    QMap<SoundType, QSoundEffect *> m_effects; ///< 类型→音效播放器实例（升级：缓存复用）

    bool m_musicEnabled;      ///< 背景音乐开关（升级新增）
    QSoundEffect *m_bgm;      ///< 背景音乐播放器（升级新增）
};

#endif // SOUNDMANAGER_H
