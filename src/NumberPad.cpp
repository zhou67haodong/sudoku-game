/**
 * @file NumberPad.cpp
 * @brief 数字输入键盘控件实现（升级版）
 * @author 周浩东
 * @date 2026-08-31
 *
 * 升级内容：
 * - 现代化按钮样式（渐变蓝、圆角、hover/按下效果）
 * - 每个数字下方显示剩余数量徽章，数字用完自动变灰禁用
 */

#include "NumberPad.h"
#include <QGridLayout>
#include <QFont>
#include <QVBoxLayout>

/**
 * @brief 构造函数，初始化 1-9 数字按钮与擦除按钮
 */
NumberPad::NumberPad(QWidget *parent)
    : QWidget(parent), m_clearButton(nullptr)
{
    setupUi();
}

/**
 * @brief 析构函数
 */
NumberPad::~NumberPad()
{
}

/**
 * @brief 初始化 UI 布局
 *
 * 升级版：使用 3x9 网格，每列 = 数字按钮(56px) + 剩余徽章(小字)，
 * 最后一列为擦除按钮。
 */
void NumberPad::setupUi()
{
    QGridLayout *layout = new QGridLayout(this);
    layout->setSpacing(6);
    layout->setContentsMargins(0, 0, 0, 0);

    QFont btnFont;
    btnFont.setPointSize(20);
    btnFont.setBold(true);

    QFont badgeFont;
    badgeFont.setPointSize(9);

    // 创建 1~9 数字按钮 + 剩余徽章
    for (int i = 1; i <= 9; ++i) {
        QPushButton *btn = new QPushButton(QString::number(i), this);
        btn->setFixedSize(56, 56);
        btn->setFont(btnFont);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setStyleSheet(
            "QPushButton {"
            "    background-color: #FFFFFF;"
            "    color: #1E293B;"
            "    border: 2px solid #CBD5E1;"
            "    border-radius: 10px;"
            "    font-weight: bold;"
            "}"
            "QPushButton:hover {"
            "    background-color: #DBEAFE;"
            "    border-color: #3B82F6;"
            "}"
            "QPushButton:pressed {"
            "    background-color: #BFDBFE;"
            "}"
            "QPushButton:disabled {"
            "    background-color: #F1F5F9;"
            "    color: #CBD5E1;"
            "    border-color: #E2E8F0;"
            "}"
        );
        m_numberButtons.append(btn);

        // 剩余数量徽章
        QLabel *badge = new QLabel("×9", this);
        badge->setAlignment(Qt::AlignCenter);
        badge->setFont(badgeFont);
        badge->setStyleSheet("color: #64748B; background: transparent;");
        m_badges.append(badge);

        int row = (i - 1) / 3;
        int col = (i - 1) % 3;
        layout->addWidget(btn, row * 2, col);
        layout->addWidget(badge, row * 2 + 1, col);

        // 点击数字按钮 → 发出 numberSelected 信号
        connect(btn, &QPushButton::clicked, this, [this, i]() {
            emit numberSelected(i);
        });
    }

    // 擦除按钮（最后一行，跨3列）
    m_clearButton = new QPushButton("⌫ 擦除", this);
    m_clearButton->setFixedSize(188, 40);
    QFont clearFont; clearFont.setPointSize(12); clearFont.setBold(true);
    m_clearButton->setFont(clearFont);
    m_clearButton->setCursor(Qt::PointingHandCursor);
    m_clearButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #FEF2F2;"
        "    color: #DC2626;"
        "    border: 2px solid #FECACA;"
        "    border-radius: 8px;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background-color: #FEE2E2;"
        "    border-color: #DC2626;"
        "}"
    );
    layout->addWidget(m_clearButton, 6, 0, 1, 3);

    connect(m_clearButton, &QPushButton::clicked, this, [this]() {
        emit clearRequested();
        emit numberSelected(0);
    });

    setLayout(layout);
}

/**
 * @brief 启用/禁用所有按钮
 */
void NumberPad::setEnabled(bool enabled)
{
    for (QPushButton *btn : m_numberButtons) {
        btn->setEnabled(enabled);
    }
    if (m_clearButton) {
        m_clearButton->setEnabled(enabled);
    }
}

/**
 * @brief 设置指定数字按钮是否可用
 */
void NumberPad::setButtonEnabled(int number, bool enabled)
{
    if (number == 0) {
        if (m_clearButton) m_clearButton->setEnabled(enabled);
        return;
    }
    int idx = number - 1;
    if (idx >= 0 && idx < m_numberButtons.size()) {
        m_numberButtons[idx]->setEnabled(enabled);
    }
}

/**
 * @brief 更新数字剩余计数（升级新增）
 *
 * 修复：原先只做“用完→禁用”，从不恢复，导致撤销/擦除/重开一局后
 * 数字键仍是灰色点不了。现在每次刷新都按剩余数量同步 enabled 状态。
 */
void NumberPad::updateRemaining(const QVector<int> &counts)
{
    for (int n = 1; n <= 9; ++n) {
        if (n - 1 >= m_badges.size()) break;
        int used = (n < counts.size()) ? counts[n] : 0;
        int remain = 9 - used;
        m_badges[n - 1]->setText(QString("×%1").arg(remain));
        if (n - 1 < m_numberButtons.size()) {
            // 关键修复：剩余 >0 即恢复可用（先清除旧的禁用状态）
            m_numberButtons[n - 1]->setEnabled(remain > 0);
        }
    }
}

/**
 * @brief 高亮指定数字按钮（视觉反馈）
 */
void NumberPad::highlightButton(int number)
{
    for (int i = 0; i < m_numberButtons.size(); ++i) {
        if (i + 1 == number) {
            m_numberButtons[i]->setStyleSheet(
                "QPushButton {"
                "    background-color: #BFDBFE;"
                "    color: #1D4ED8;"
                "    border: 2px solid #3B82F6;"
                "    border-radius: 10px;"
                "    font-weight: bold;"
                "}"
            );
        } else {
            m_numberButtons[i]->setStyleSheet("");
        }
    }
}

/**
 * @brief 重置所有按钮状态
 */
void NumberPad::reset()
{
    for (QPushButton *btn : m_numberButtons) {
        btn->setEnabled(true);
        btn->setStyleSheet("");
    }
    if (m_clearButton) {
        m_clearButton->setEnabled(true);
        m_clearButton->setStyleSheet("");
    }
    updateRemaining(QVector<int>(10, 0));
}
