/**
 * @file ThemeManager.h
 * @brief 主题管理 - 提供经典/现代/暗色三种界面风格
 * @author 马广东
 * @date 2026-09-02
 */

#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H

#include <QObject>
#include <QStringList>
#include <QColor>
#include <QMap>

/**
 * @class ThemeManager
 * @brief 主题管理器，负责加载并应用 QSS 样式表
 * @author 马广东
 */
class ThemeManager : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief 内置主题枚举
     */
    enum class Theme { Classic, Modern, Dark };

    /**
     * @brief 构造函数
     * @param parent 父对象
     */
    explicit ThemeManager(QObject *parent = nullptr);

    /**
     * @brief 加载并应用指定主题
     * @param themeName 主题名（"Classic"/"Modern"/"Dark"，大小写不敏感）
     * @return 是否成功
     */
    bool loadTheme(const QString &themeName);

    /**
     * @brief 加载并应用指定主题（枚举版）
     */
    bool loadTheme(Theme theme);

    /**
     * @brief 获取可用主题列表
     * @return 主题名列表
     */
    QStringList getAvailableThemes() const;

    /**
     * @brief 设置某角色（单元格状态）的颜色
     * @param role 角色名（"Normal"/"Selected"/"Fixed"/"Error"/"Hint"）
     * @param color 颜色
     */
    void setCellColor(const QString &role, const QColor &color);

    /**
     * @brief 获取当前主题的 QSS 文本
     */
    QString getCurrentStyleSheet() const;

private:
    /**
     * @brief 构建经典主题 QSS
     */
    QString buildClassicStyle() const;

    /**
     * @brief 构建现代主题 QSS
     */
    QString buildModernStyle() const;

    /**
     * @brief 构建暗色主题 QSS
     */
    QString buildDarkStyle() const;

    QMap<QString, QColor> m_cellColors;  ///< 单元格角色 → 颜色
    QString m_currentStyle;              ///< 当前样式表
};

#endif // THEMEMANAGER_H
