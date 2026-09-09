/**
 * @file SoundManager.cpp
 * @brief 音效管理器实现（单例，升级版）
 * @author 靳凯
 * @date 2026-09-03
 *
 * 升级内容：
 * - 撤销音效（复用柔和的 select 提示音色资源）
 * - 背景音乐 bgm.wav 无限循环播放，音量独立调低，支持开关
 */

#include "SoundManager.h"

#include <QSoundEffect>
#include <QUrl>
#include <QFile>
#include <QDebug>

/**
 * @brief 获取单例（线程安全，C++11 局部静态）
 */
SoundManager *SoundManager::instance()
{
    static SoundManager inst;
    return &inst;
}

/**
 * @brief 私有构造函数，初始化音效映射
 */
SoundManager::SoundManager(QObject *parent)
    : QObject(parent)
    , m_muted(false)
    , m_volume(80)
    , m_musicEnabled(true)
    , m_bgm(nullptr)
{
    initSounds();

    // 升级：背景音乐播放器（循环）
    if (QFile::exists(":/sounds/bgm.wav")) {
        m_bgm = new QSoundEffect(this);
        m_bgm->setSource(QUrl("qrc:/sounds/bgm.wav"));
        m_bgm->setLoopCount(QSoundEffect::Infinite);
        m_bgm->setVolume(0.25); // 背景音乐音量压低，不干扰思考
    }
}

/**
 * @brief 初始化音效文件映射
 *
 * 资源放在 :/sounds/ 资源前缀下（需在 .qrc 中注册）。
 * 若文件不存在，playEffect 会安全跳过。
 */
void SoundManager::initSounds()
{
    m_soundFiles[SoundType::CellSelect] = "qrc:/sounds/select.wav";
    m_soundFiles[SoundType::CellFill]   = "qrc:/sounds/fill.wav";
    m_soundFiles[SoundType::Error]      = "qrc:/sounds/error.wav";
    m_soundFiles[SoundType::Hint]       = "qrc:/sounds/hint.wav";
    m_soundFiles[SoundType::Undo]       = "qrc:/sounds/select.wav"; // 撤销复用轻提示音
    m_soundFiles[SoundType::Victory]    = "qrc:/sounds/victory.wav";

    // 升级：为每种音效创建并缓存播放器实例，避免每次点击临时创建
    // （临时创建在资源异步加载时可能无声，且播放失败时对象无法释放）
    for (auto it = m_soundFiles.cbegin(); it != m_soundFiles.cend(); ++it) {
        m_effects[it.key()] = createEffect(it.value());
    }
}

/**
 * @brief 创建并缓存一个音效播放器实例
 */
QSoundEffect *SoundManager::createEffect(const QString &path)
{
    if (!QFile::exists(path)) return nullptr; // 资源缺失时安全降级
    QSoundEffect *effect = new QSoundEffect(this);
    effect->setSource(QUrl(path));
    effect->setVolume(m_volume / 100.0);
    effect->setLoopCount(1);
    return effect;
}

/**
 * @brief 播放音效
 */
void SoundManager::playEffect(SoundType type)
{
    if (m_muted) return;
    QSoundEffect *effect = m_effects.value(type, nullptr);
    if (!effect) return; // 资源缺失或未初始化时静默跳过
    effect->setVolume(m_volume / 100.0); // 音量实时生效
    effect->play();
}

/**
 * @brief 设置静音
 */
void SoundManager::setMuted(bool muted)
{
    if (m_muted != muted) {
        m_muted = muted;
        emit mutedChanged(m_muted);
    }
}

/**
 * @brief 设置音量
 */
void SoundManager::setVolume(int volume)
{
    m_volume = qBound(0, volume, 100);
}

/**
 * @brief 是否静音
 */
bool SoundManager::isMuted() const
{
    return m_muted;
}

// ==================== 背景音乐（升级新增） ====================

void SoundManager::playMusic()
{
    if (!m_musicEnabled || m_muted || !m_bgm) return;
    if (!m_bgm->isPlaying()) m_bgm->play();
}

void SoundManager::stopMusic()
{
    if (m_bgm && m_bgm->isPlaying()) m_bgm->stop();
}

void SoundManager::setMusicEnabled(bool on)
{
    m_musicEnabled = on;
    if (!on) stopMusic();
}
