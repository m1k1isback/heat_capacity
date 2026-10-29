#include "ThemeManager.h"
#include <QFile>
#include <QApplication>
#include <QDebug>

ThemeManager::ThemeManager(QObject *parent)
    : QObject(parent)
    , m_currentTheme(Light)
    , m_settings("LabWork", "CalorimeterApp")
{
    // Загружаем сохранённую тему
    QString savedTheme = m_settings.value("theme", "light").toString();
    if (savedTheme == "dark") {
        m_currentTheme = Dark;
    }

    applyTheme();
}

void ThemeManager::setTheme(Theme theme)
{
    if (m_currentTheme != theme) {
        m_currentTheme = theme;
        m_settings.setValue("theme", theme == Dark ? "dark" : "light");
        applyTheme();
        emit themeChanged(theme);
    }
}

void ThemeManager::toggleTheme()
{
    setTheme(m_currentTheme == Light ? Dark : Light);
}

void ThemeManager::applyTheme()
{
    QString qss;
    if (m_currentTheme == Dark) {
        qss = loadQSSFile(":/themes/dark_theme.qss");
    } else {
        qss = loadQSSFile(":/themes/light_theme.qss");
    }

    if (!qss.isEmpty()) {
        qApp->setStyleSheet(qss);
    } else {
        qWarning() << "Не удалось загрузить файл темы";
    }
}

QString ThemeManager::getStyleSheet() const
{
    return qApp->styleSheet();
}

QString ThemeManager::loadQSSFile(const QString &fileName) const
{
    QFile file(fileName);
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        return file.readAll();
    }
    qWarning() << "Файл темы не найден:" << fileName;
    return QString();
}
