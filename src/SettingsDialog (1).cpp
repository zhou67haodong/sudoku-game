/**
 * @file SettingsDialog.cpp
 * @brief 设置对话框实现
 * @author 靳凯
 * @date 2026-09-03
 */

#include "SettingsDialog.h"

#include <QFormLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QSlider>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QHBoxLayout>

/**
 * @brief 构造函数，构建设置界面
 */
SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
    , m_soundCheck(nullptr)
    , m_musicCheck(nullptr)
    , m_autoCheck(nullptr)
    , m_highlightCheck(nullptr)
    , m_themeCombo(nullptr)
    , m_fontSlider(nullptr)
{
    setWindowTitle("设置");
    setMinimumWidth(320);
    setupUi();
}

/**
 * @brief 构建 UI
 */
void SettingsDialog::setupUi()
{
    QFormLayout *layout = new QFormLayout(this);

    m_soundCheck = new QCheckBox("启用音效", this);
    m_soundCheck->setChecked(true);
    layout->addRow("音效：", m_soundCheck);

    m_musicCheck = new QCheckBox("启用背景音乐", this);
    layout->addRow("音乐：", m_musicCheck);

    m_themeCombo = new QComboBox(this);
    m_themeCombo->addItems(QStringList() << "Classic" << "Modern" << "Dark");
    layout->addRow("主题：", m_themeCombo);

    m_autoCheck = new QCheckBox("自动检查冲突", this);
    layout->addRow("自动检查：", m_autoCheck);

    m_highlightCheck = new QCheckBox("高亮冲突单元格", this);
    m_highlightCheck->setChecked(true);
    layout->addRow("高亮冲突：", m_highlightCheck);

    m_fontSlider = new QSlider(Qt::Horizontal, this);
    m_fontSlider->setRange(1, 5);
    m_fontSlider->setValue(3);
    layout->addRow("字号级别：", m_fontSlider);

    // 按钮盒：确定 / 取消
    QDialogButtonBox *buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
    layout->addRow(buttons);

    connect(buttons, &QDialogButtonBox::accepted, this, &SettingsDialog::onAccepted);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

/**
 * @brief 获取当前设置
 */
Settings SettingsDialog::getSettings() const
{
    Settings s;
    s.soundOn = m_soundCheck->isChecked();
    s.musicOn = m_musicCheck->isChecked();
    s.theme = m_themeCombo->currentText();
    s.autoCheck = m_autoCheck->isChecked();
    s.highlightConflicts = m_highlightCheck->isChecked();
    s.fontSize = m_fontSlider->value();
    return s;
}

/**
 * @brief 确定按钮处理：收集设置并发信号
 */
void SettingsDialog::onAccepted()
{
    emit settingsChanged(getSettings());
    accept();
}
