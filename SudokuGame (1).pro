######################################################################
# @file SudokuGame.pro
# @brief 数独游戏 Qt 项目配置文件
# @author 周浩东（项目整合）
# @date 2026-08-26
#
# 项目说明：
# - 基于 Qt 6 的 C++ 数独游戏
# - 包含 core / gui / widgets / multimedia 模块
# - 使用 C++11 标准
#
# 构建方法：
#   qmake SudokuGame.pro
#   make            (Linux/Mac)
#   nmake           (Windows MSVC)
#   mingw32-make    (Windows MinGW)
# 或使用 Qt Creator 直接打开本 .pro 文件编译运行
######################################################################

# Qt 所需模块
QT += core gui widgets multimedia

# C++ 标准
CONFIG += c++17
CONFIG += console

# 应用配置
TARGET = SudokuGame
TEMPLATE = app

# ========== 头文件（.h）==========
HEADERS += \
    include/MainWindow.h \
    include/SudokuBoard.h \
    include/NumberPad.h \
    include/SudokuGenerator.h \
    include/SudokuSolver.h \
    include/SudokuValidator.h \
    include/GameState.h \
    include/HighScore.h \
    include/ThemeManager.h \
    include/SettingsDialog.h \
    include/AboutDialog.h \
    include/SoundManager.h

# ========== 源文件（.cpp）==========
SOURCES += \
    src/main.cpp \
    src/MainWindow.cpp \
    src/SudokuBoard.cpp \
    src/NumberPad.cpp \
    src/SudokuGenerator.cpp \
    src/SudokuSolver.cpp \
    src/SudokuValidator.cpp \
    src/GameState.cpp \
    src/HighScore.cpp \
    src/ThemeManager.cpp \
    src/SettingsDialog.cpp \
    src/AboutDialog.cpp \
    src/SoundManager.cpp

# ========== 资源文件 ==========
RESOURCES += SudokuGame.qrc

# Windows 下生成独立窗口应用（去掉控制台）
win32:CONFIG -= console

# 头文件搜索路径
INCLUDEPATH += include
