#pragma once

#include <QString>
#include <QMap>
#include <QRegularExpression>
#include <QList>
#include <QTextDocument>

class SyntaxHighlighter {
public:
    struct MessageBlock {
        bool isCode;
        QString language;
        QString text;
    };

    static void init();
    static QList<MessageBlock> splitMessage(const QString& rawMessage);
    static void applyHighlighting(QTextDocument* doc, const QString& languageAlias, const QString& code);

private:
    struct Colors {
        QString comment = "#6A9955";
        QString string = "#CE9178";
        QString keyword = "#569CD6";
        QString number = "#B5CEA8";
        QString macro = "#C586C0";
        QString defaultColor = "#D4D4D4";
    };

    static Colors colors;
    static QMap<QString, QRegularExpression> rules;
};