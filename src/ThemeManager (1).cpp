/**
 * @file ThemeManager.cpp
 * @brief 主题管理实现 - 提供三种完整 QSS 样式
 * @author 马广东
 * @date 2026-09-02
 */

#include "ThemeManager.h"
#include <QApplication>
#include <QStyle>
#include <QPalette>

/**
 * @brief 构造函数，初始化默认单元格颜色
 */
ThemeManager::ThemeManager(QObject *parent)
    : QObject(parent)
{
    m_cellColors["Normal"]   = QColor(255, 255, 255);
    m_cellColors["Selected"] = QColor(200, 230, 255);
    m_cellColors["Fixed"]    = QColor(220, 220, 220);
    m_cellColors["Error"]    = QColor(255, 200, 200);
    m_cellColors["Hint"]     = QColor(200, 255, 200);
    loadTheme(Theme::Classic);
}

/**
 * @brief 加载主题（字符串版）
 */
bool ThemeManager::loadTheme(const QString &themeName)
{
    QString lower = themeName.toLower();
    if (lower == "modern")      return loadTheme(Theme::Modern);
    if (lower == "dark")        return loadTheme(Theme::Dark);
    if (lower == "classic")     return loadTheme(Theme::Classic);
    return loadTheme(Theme::Classic); // 未知主题回退经典
}

/**
 * @brief 加载主题（枚举版）
 */
bool ThemeManager::loadTheme(Theme theme)
{
    switch (theme) {
    case Theme::Classic: m_currentStyle = buildClassicStyle(); break;
    case Theme::Modern:  m_currentStyle = buildModernStyle();  break;
    case Theme::Dark:    m_currentStyle = buildDarkStyle();    break;
    default:             m_currentStyle = buildClassicStyle(); break;
    }
    qApp->setStyleSheet(m_currentStyle);
    // 同时调整调色板以适配暗色
    if (theme == Theme::Dark) {
        QPalette pal = qApp->palette();
        pal.setColor(QPalette::Window, QColor(45, 45, 48));
        pal.setColor(QPalette::WindowText, Qt::white);
        pal.setColor(QPalette::Base, QColor(60, 60, 63));
        pal.setColor(QPalette::Text, Qt::white);
        qApp->setPalette(pal);
    } else {
        qApp->setPalette(qApp->style()->standardPalette());
    }
    return true;
}

/**
 * @brief 可用主题列表
 */
QStringList ThemeManager::getAvailableThemes() const
{
    return QStringList() << "Classic" << "Modern" << "Dark";
}

/**
 * @brief 设置单元格角色颜色
 */
void ThemeManager::setCellColor(const QString &role, const QColor &color)
{
    m_cellColors[role] = color;
}

/**
 * @brief 获取当前样式表
 */
QString ThemeManager::getCurrentStyleSheet() const
{
    return m_currentStyle;
}

/**
 * @brief 经典主题 QSS
 */
QString ThemeManager::buildClassicStyle() const
{
    return R"(
        QWidget { font-family: "Microsoft YaHei"; font-size: 14px; color: #E2E8F0; }
        QMainWindow { background-color: #1E293B; }
        QLabel { color: #E2E8F0; background: transparent; }
        QPushButton {
            background-color: #334155;
            color: #F1F5F9;
            border: 1px solid #475569;
            border-radius: 8px;
            padding: 8px 16px;
        }
        QPushButton:hover { background-color: #475569; border-color: #64748B; }
        QPushButton:pressed { background-color: #2D3B4F; }
        QMenuBar { background-color: #0F172A; color: #E2E8F0; }
        QMenuBar::item:selected { background: #334155; border-radius: 4px; }
        QMenu { background: #1E293B; color: #E2E8F0; border: 1px solid #334155; }
        QMenu::item:selected { background: #334155; }
        QStatusBar { background: #0F172A; color: #94A3B8; }
        QToolBar { background: #0F172A; border: none; }
        QGroupBox { color: #CBD5E1; }
    )";
}

/**
 * @brief 现代主题 QSS（蓝色调）
 */
QString ThemeManager::buildModernStyle() const
{
    return R"(
        QWidget { font-family: "Microsoft YaHei"; font-size: 14px; }
        QMainWindow { background-color: #eef4ff; }
        QLabel { color: #1a3c6e; }
        QPushButton {
            background-color: #4a90e2;
            color: white;
            border: none;
            border-radius: 6px;
            padding: 8px;
        }
        QPushButton:hover { background-color: #357abd; }
        QMenuBar { background-color: #d6e4ff; }
    )";
}

/**
 * @brief 暗色主题 QSS
 */
QString ThemeManager::buildDarkStyle() const
{
    return R"(
        QWidget { font-family: "Microsoft YaHei"; font-size: 14px; }
        QMainWindow { background-color: #2d2d30; }
        QLabel { color: #f0f0f0; }
        QPushButton {
            background-color: #3e3e42;
            color: #f0f0f0;
            border: 1px solid #555555;
            border-radius: 4px;
            padding: 6px;
        }
        QPushButton:hover { background-color: #505050; }
        QMenuBar { background-color: #1e1e1e; color: #f0f0f0; }
    )";
}
