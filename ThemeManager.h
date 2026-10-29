#pragma once

#include <QObject>
#include <QString>
#include <QSettings>

class ThemeManager : public QObject
{
    Q_OBJECT

public:
    explicit ThemeManager(QObject *parent = nullptr);
    ~ThemeManager() override = default;

    enum Theme {
        Light,
        Dark
    };
    Q_ENUM(Theme)

    Theme currentTheme() const { return m_currentTheme; }
    void setTheme(Theme theme);
    void toggleTheme();

    // Применить тему ко всему приложению
    void applyTheme();

    // Получить стиль для конкретного виджета (если нужно)
    QString getStyleSheet() const;

signals:
    void themeChanged(Theme theme);

private:
    Theme m_currentTheme;
    QSettings m_settings;

    QString loadQSSFile(const QString &fileName) const;
};
