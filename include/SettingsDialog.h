/**
 * @file SettingsDialog.h
 * @brief 设置对话框 - 音效、主题、自动检查等用户偏好设置
 * @author 靳凯
 * @date 2026-09-03
 */

#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QString>

/**
 * @brief 设置项数据结构（跨模块传递）
 */
struct Settings {
    bool soundOn;             ///< 音效开关
    bool musicOn;             ///< 背景音乐开关
    QString theme;            ///< 主题（Classic/Modern/Dark）
    bool autoCheck;           ///< 自动检查冲突
    bool highlightConflicts;  ///< 高亮冲突
    int fontSize;             ///< 字号级别（1~5）
};

/**
 * @class SettingsDialog
 * @brief 设置对话框，提供音效、主题、字号等配置
 * @author 靳凯
 */
class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父窗口
     */
    explicit SettingsDialog(QWidget *parent = nullptr);

    /**
     * @brief 获取当前设置
     */
    Settings getSettings() const;

signals:
    /**
     * @brief 设置改变信号
     * @param s 新的设置
     */
    void settingsChanged(const Settings &s);

private slots:
    void onAccepted();  ///< 点击确定时收集设置并发出信号

private:
    void setupUi();     ///< 构建界面

    class QCheckBox *m_soundCheck;        ///< 音效开关
    class QCheckBox *m_musicCheck;        ///< 音乐开关
    class QCheckBox *m_autoCheck;         ///< 自动检查
    class QCheckBox *m_highlightCheck;    ///< 高亮冲突
    class QComboBox *m_themeCombo;        ///< 主题选择
    class QSlider   *m_fontSlider;        ///< 字号滑块
};

#endif // SETTINGSDIALOG_H
