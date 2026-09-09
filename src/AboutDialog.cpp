/**
 * @file AboutDialog.cpp
 * @brief 关于对话框实现
 * @author 靳凯
 * @date 2026-09-03
 */

#include "AboutDialog.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <QPixmap>
#include <QApplication>

/**
 * @brief 构造函数，构建关于界面
 */
AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("关于 - 数独游戏");
    setFixedSize(380, 360);
    setupUi();
}

/**
 * @brief 构建 UI
 */
void AboutDialog::setupUi()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(8);
    layout->setContentsMargins(16, 16, 16, 16);

    // 应用名与版本
    QLabel *title = new QLabel("数独游戏 Sudoku Game v1.0.0", this);
    QFont f = title->font();
    f.setPointSize(16);
    f.setBold(true);
    title->setFont(f);
    title->setAlignment(Qt::AlignCenter);
    layout->addWidget(title);

    // 课程信息
    QLabel *course = new QLabel("面向对象程序设计课程设计", this);
    course->setAlignment(Qt::AlignCenter);
    layout->addWidget(course);

    // 团队照片占位
    QLabel *photo = new QLabel("【团队照片】", this);
    photo->setFixedSize(120, 120);
    photo->setStyleSheet("border:1px dashed #888; background:#f5f5f5;");
    photo->setAlignment(Qt::AlignCenter);
    layout->addWidget(photo, 0, Qt::AlignHCenter);

    // 团队成员
    QLabel *members = new QLabel(
        "开发团队：\n"
        "  周浩东  - 主窗口 / 棋盘 / 项目整合\n"
        "  张啸林  - 数独生成 / 校验算法 / 汇报\n"
        "  马广东  - 游戏状态 / 计时 / 存档 / PPT\n"
        "  靳凯  - 设置 / 对话框 / 音效 / 项目整合", this);
    members->setAlignment(Qt::AlignLeft);
    layout->addWidget(members);

    // 许可证
    QLabel *license = new QLabel("许可证：MIT License\n"
        "基于 Qt " + QString(QT_VERSION_STR) + " 开发", this);
    license->setWordWrap(true);
    layout->addWidget(license);

    // 确定按钮
    QPushButton *ok = new QPushButton("确定", this);
    ok->setFixedWidth(100);
    layout->addWidget(ok, 0, Qt::AlignHCenter);
    connect(ok, &QPushButton::clicked, this, &QDialog::accept);
}
