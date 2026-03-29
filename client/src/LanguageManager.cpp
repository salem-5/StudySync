#include "LanguageManager.h"

#include <boost/json.hpp>
#include <QFile>
#include <QByteArray>

namespace json = boost::json;

static void loadIntoMap(const QString& path,QHash<QString, QString>& map) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QByteArray data = file.readAll();
    file.close();

    json::value jv = json::parse(data.constData());
    json::object obj = jv.as_object();

    map.clear();
    for (auto& [key, value] : obj) {
        if (value.is_string()) {
            map.insert(
            QString::fromUtf8(key.data(), key.size()),
            QString::fromUtf8(value.as_string().c_str())
            );
        }
    }
}

QHash<QString, QString> LanguageManager::translations;
QHash<QString, QString> LanguageManager::fallback;

bool LanguageManager::loadLanguage(const QString& path) {
    loadIntoMap(path, translations);
    return true;
}

void LanguageManager::loadFallback(const QString& path) {
    loadIntoMap(path, fallback);
}

QString LanguageManager::tr(const QString& key) {
    if (translations.contains(key))
        return translations.value(key);

    if (fallback.contains(key))
        return fallback.value(key);

    return key;
}