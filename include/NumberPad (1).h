/**
 * @file NumberPad.h
 * @brief 数字输入键盘控件头文件（升级版）
 * @author 周浩东
 * @date 2026-08-31
 *
 * 功能说明：
 * - 继承自 QWidget，作为数独游戏的数字输入面板
 * - 提供 1-9 数字按钮和擦除按钮
 * - 升级：每个数字按钮下方显示剩余可用数量，用完自动变灰
 */

#ifndef NUMBERPAD_H
#define NUMBERPAD_H

#include <QWidget>
#include <QVector>
#include <QPushButton>
#include <QLabel>

/**
 * @brief 数字键盘类
 *
 * 提供一个 3x4 布局的数字输入面板，包含 1-9 数字按钮
 * 和一个擦除按钮。按钮下方有剩余数量徽章。
 */
class NumberPad : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父控件指针
     */
    explicit NumberPad(QWidget *parent = nullptr);

    /**
     * @brief 析构函数
     */
    ~NumberPad();

    /**
     * @brief 启用/禁用所有按钮
     * @param enabled 是否启用
     */
    void setEnabled(bool enabled);

    /**
     * @brief 设置某个数字按钮是否可用
     * @param number 数字（1-9），0 表示擦除按钮
     * @param enabled 是否可用
     */
    void setButtonEnabled(int number, bool enabled);

    /**
     * @brief 更新数字剩余计数（升级新增）
     * @param counts counts[n] = 数字 n 已用次数（下标 1~9 有效）
     */
    void updateRemaining(const QVector<int> &counts);

signals:
    /**
     * @brief 当用户点击数字按钮时发出
     * @param num 点击的数字（1-9），0 表示擦除
     */
    void numberSelected(int num);

    /**
     * @brief 当用户请求清除时发出
     */
    void clearRequested();

public slots:
    /**
     * @brief 高亮指定数字按钮（视觉反馈）
     * @param number 数字（1-9），-1 表示清除高亮
     */
    void highlightButton(int number);

    /**
     * @brief 重置所有按钮状态
     */
    void reset();

private:
    /**
     * @brief 初始化 UI
     */
    void setupUi();

    QVector<QPushButton *> m_numberButtons;  ///< 1-9 数字按钮
    QVector<QLabel *>     m_badges;           ///< 剩余数量徽章（升级新增）
    QPushButton *m_clearButton;               ///< 擦除按钮
};

#endif // NUMBERPAD_H
