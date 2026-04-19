#include "ui/SyntaxHighlighter.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QTextCursor>
#include <QTextCharFormat>
#include <QDebug>

SyntaxHighlighter::Colors SyntaxHighlighter::colors;
QMap<QString, QRegularExpression> SyntaxHighlighter::rules;

/*
 * This is taken from my other personal project, and the syntax rules json is derived from the prism javascript library using a simple javascript script
 * -Omar Salem
 */
void SyntaxHighlighter::init() {
    QFile file(":/resources/syntax_rules.json");
    if (!file.open(QIODevice::ReadOnly)) return;

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QJsonObject config = doc.object();

    auto parseColor = [](const QJsonValue& val, const QString& def) -> QString {
        if (val.isString()) return val.toString();
        if (val.isDouble()) return QString("#%1").arg(val.toInt(), 6, 16, QLatin1Char('0'));
        return def;
    };

    if (config.contains("colors")) {
        // Rename the local variable to avoid shadowing the static 'colors' struct
        QJsonObject colorsObj = config["colors"].toObject();
        colors.comment = parseColor(colorsObj["comment"], "#6A9955");
        colors.string = parseColor(colorsObj["string"], "#CE9178");
        colors.keyword = parseColor(colorsObj["keyword"], "#569CD6");
        colors.number = parseColor(colorsObj["number"], "#B5CEA8");
        colors.macro = parseColor(colorsObj["macro"], "#C586C0");
        colors.defaultColor = parseColor(colorsObj["defaultColor"], "#D4D4D4");
    }

    QJsonArray languages = config["languages"].toArray();
    for (const QJsonValue& langVal : languages) {
        QJsonObject lang = langVal.toObject();
        QString pattern = lang["pattern"].toString();
        QRegularExpression re(pattern);

        QJsonArray aliases = lang["aliases"].toArray();
        for (const QJsonValue& aliasVal : aliases) {
            rules.insert(aliasVal.toString().toLower(), re);
        }
    }
}

QList<SyntaxHighlighter::MessageBlock> SyntaxHighlighter::splitMessage(const QString& rawMessage) {
    QList<MessageBlock> blocks;
    int currentIndex = 0;

    while (currentIndex < rawMessage.length()) {
        int codeStart = rawMessage.indexOf("```", currentIndex);
        if (codeStart == -1) {
            blocks.append({false, "", rawMessage.mid(currentIndex)});
            break;
        }
        if (codeStart > currentIndex) {
            blocks.append({false, "", rawMessage.mid(currentIndex, codeStart - currentIndex)});
        }

        int codeEnd = rawMessage.indexOf("```", codeStart + 3);
        if (codeEnd == -1) {
            blocks.append({true, "", rawMessage.mid(codeStart + 3)});
            break;
        }

        QString codeContent = rawMessage.mid(codeStart + 3, codeEnd - (codeStart + 3));
        int firstNewline = codeContent.indexOf('\n');
        QString lang = "";
        QString code = codeContent;

        if (firstNewline != -1) {
            lang = codeContent.left(firstNewline).trimmed();
            code = codeContent.mid(firstNewline + 1);
        }

        if (code.endsWith('\n')) code.chop(1);
        if (code.endsWith('\r')) code.chop(1);

        blocks.append({true, lang, code});
        currentIndex = codeEnd + 3;
    }
    return blocks;
}

void SyntaxHighlighter::applyHighlighting(QTextDocument* doc, const QString& languageAlias, const QString& code) {
    QTextCursor cursor(doc);
    cursor.beginEditBlock();

    QTextCharFormat defaultFormat;
    defaultFormat.setForeground(QColor(colors.defaultColor));

    QFont monoFont("Courier");
    monoFont.setStyleHint(QFont::Monospace);
    defaultFormat.setFont(monoFont);

    if (!rules.contains(languageAlias.toLower())) {
        cursor.insertText(code, defaultFormat);
        cursor.endEditBlock();
        return;
    }

    QRegularExpression re = rules[languageAlias.toLower()];
    QRegularExpressionMatchIterator i = re.globalMatch(code);
    int lastEnd = 0;

    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();

        if (match.capturedStart() > lastEnd) {
            QString text = code.mid(lastEnd, match.capturedStart() - lastEnd);
            cursor.insertText(text, defaultFormat);
        }

        QString colorHex = colors.defaultColor;
        if (match.capturedStart("comment") != -1) colorHex = colors.comment;
        else if (match.capturedStart("string") != -1) colorHex = colors.string;
        else if (match.capturedStart("keyword") != -1) colorHex = colors.keyword;
        else if (match.capturedStart("number") != -1) colorHex = colors.number;
        else if (match.capturedStart("macro") != -1) colorHex = colors.macro;

        QTextCharFormat format;
        format.setForeground(QColor(colorHex));
        format.setFont(monoFont);
        cursor.insertText(match.captured(), format);

        lastEnd = match.capturedEnd();
    }

    if (lastEnd < code.length()) {
        cursor.insertText(code.mid(lastEnd), defaultFormat);
    }

    cursor.endEditBlock();
}