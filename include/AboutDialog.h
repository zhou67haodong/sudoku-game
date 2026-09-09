/**
 * @file AboutDialog.h
 * @brief 关于对话框 - 展示项目信息、团队成员与许可证
 * @author 靳凯
 * @date 2026-09-03
 */

#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>

/**
 * @class AboutDialog
 * @brief 关于对话框
 * @author 靳凯
 */
class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父窗口
     */
    explicit AboutDialog(QWidget *parent = nullptr);

private:
    void setupUi();  ///< 构建界面
};

#endif // ABOUTDIALOG_H
