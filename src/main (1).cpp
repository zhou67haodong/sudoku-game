/**
 * @file main.cpp
 * @brief 数独游戏主程序入口
 * @author 周浩东
 * @date 2026-08-31
 *
 * 程序说明：
 * - 创建 QApplication 应用实例
 * - 实例化主窗口 MainWindow
 * - 显示主窗口并进入事件循环
 */

#include <QApplication>
#include "MainWindow.h"

/**
 * @brief 应用主入口
 * @param argc 命令行参数个数
 * @param argv 命令行参数数组
 * @return 应用退出码
 */
int main(int argc, char *argv[])
{
    // 创建 Qt 应用实例（所有 Qt GUI 程序必须创建一个 QApplication 对象）
    QApplication app(argc, argv);

    // 设置应用程序基本信息（用于 About 对话框等）
    app.setApplicationName("Sudoku Game");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("SudokuStudio");

    // 创建并显示主窗口
    MainWindow mainWindow;
    mainWindow.show();

    // 启动事件循环，程序进入等待用户交互状态
    return app.exec();
}
