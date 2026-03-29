#pragma once
#include <QString>
#include <QHash>

class LanguageManager {
public:
    static bool loadLanguage(const QString& path);
    static void loadFallback(const QString& path);
    static QString tr(const QString& key);

private:
    static QHash<QString, QString> translations;
    static QHash<QString, QString> fallback;
};