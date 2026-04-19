#include "ui/page/AbstractChatPage.h"
#include <QHBoxLayout>
#include <QScrollBar>
#include <QTimer>
#include <QLabel>
#include <QFrame>
#include <QEvent>
#include <QStyle>
#include <QTextEdit>
#include "LanguageManager.h"
#include "ui/SyntaxHighlighter.h"
#include <QAbstractTextDocumentLayout>

class QTextEdit;

AbstractChatPage::AbstractChatPage(QWidget* parent) : QWidget(parent),currentLoadingLabel(nullptr), currentCancelBtn(nullptr), currentLoadingBubble(nullptr), spinnerFrame(0) {
    spinnerTimer = new QTimer(this);
    connect(spinnerTimer, &QTimer::timeout, this, [this]() {
        if (currentLoadingLabel) {
            QStringList frames = {"⠋", "⠙", "⠹", "⠸", "⠼", "⠴", "⠦", "⠧", "⠇", "⠏"}; // taken from the nodejs spinner in the terminal
            spinnerFrame = (spinnerFrame + 1) % frames.size();
            currentLoadingLabel->setText(frames[spinnerFrame] + LanguageManager::tr("chat.generating"));
        }
    });
    setupUi();
}

void AbstractChatPage::setupUi() {
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    QHBoxLayout* topBarLayout = new QHBoxLayout();
    topBarLayout->addStretch();

    btnClearHistory = new QPushButton(LanguageManager::tr("chat.clear_history"));
    btnClearHistory->setObjectName("btnClearHistory");
    btnClearHistory->hide();
    topBarLayout->addWidget(btnClearHistory);

    mainLayout->addLayout(topBarLayout);
    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setObjectName("chatScrollArea");

    chatContainer = new QWidget();
    chatLayout = new QVBoxLayout(chatContainer);
    chatLayout->setContentsMargins(5, 5, 5, 5);
    chatLayout->setSpacing(0);
    chatLayout->addStretch();

    scrollArea->setWidget(chatContainer);
    mainLayout->addWidget(scrollArea);

    inputCard = new QFrame(this);
    inputCard->setProperty("cssClass", "card");
    inputCardLayout = new QVBoxLayout(inputCard);
    inputCardLayout->setContentsMargins(10, 10, 10, 10);
    inputCardLayout->setSpacing(8);

    attachmentContainer = new QWidget(inputCard);
    attachmentLayout = new QHBoxLayout(attachmentContainer);
    attachmentLayout->setContentsMargins(0, 0, 0, 0);
    attachmentLayout->setSpacing(8);

    btnAddAttachment = new QPushButton(LanguageManager::tr("chat.add_task"), attachmentContainer);
    btnAddAttachment->setCursor(Qt::PointingHandCursor);
    btnAddAttachment->setProperty("cssClass", "card");
    connect(btnAddAttachment, &QPushButton::clicked, this, &AbstractChatPage::onAddAttachmentClicked);

    attachmentLayout->addWidget(btnAddAttachment);
    attachmentLayout->addStretch();

    inputCardLayout->addWidget(attachmentContainer);

    QHBoxLayout* entryLayout = new QHBoxLayout();
    entryLayout->setContentsMargins(0, 0, 0, 0);

    chatInput = new QLineEdit(inputCard);
    chatInput->setPlaceholderText(LanguageManager::tr("chat.placeholder"));
    chatInput->setMinimumHeight(36);

    btnSend = new QPushButton(LanguageManager::tr("chat.send"), inputCard);
    btnSend->setMinimumHeight(36);
    btnSend->setCursor(Qt::PointingHandCursor);
    btnSend->setProperty("cssClass", "card");

    entryLayout->addWidget(chatInput);
    entryLayout->addWidget(btnSend);

    inputCardLayout->addLayout(entryLayout);
    mainLayout->addWidget(inputCard);

    connect(btnSend, &QPushButton::clicked, this, &AbstractChatPage::onSendClicked);
    connect(chatInput, &QLineEdit::returnPressed, this, &AbstractChatPage::onSendClicked);
}

void AbstractChatPage::setInputEnabled(bool enabled) {
    chatInput->setEnabled(enabled);
    btnSend->setEnabled(enabled);
    btnAddAttachment->setEnabled(enabled);
    if (!enabled) {
        chatInput->setPlaceholderText(LanguageManager::tr("chat.waiting"));
    } else {
        chatInput->setPlaceholderText(LanguageManager::tr("chat.placeholder"));
    }
}

void AbstractChatPage::addAttachmentChip(int taskId, const QString& title) {
    if (attachedTasks.contains(taskId)) return;
    attachedTasks.insert(taskId, title);

    QFrame* chip = new QFrame(attachmentContainer);
    chip->setProperty("cssClass", "card");
    chip->setProperty("isChip", true);

    QHBoxLayout* chipLayout = new QHBoxLayout(chip);
    chipLayout->setContentsMargins(10, 4, 6, 4);
    chipLayout->setSpacing(6);

    QLabel* lblTitle = new QLabel(title, chip);

    QPushButton* btnClose = new QPushButton("×", chip);
    btnClose->setObjectName("closeBtn");
    btnClose->setFixedSize(20, 20);
    btnClose->setCursor(Qt::PointingHandCursor);

    chipLayout->addWidget(lblTitle);
    chipLayout->addWidget(btnClose);

    attachmentLayout->insertWidget(attachmentLayout->count() - 1, chip);

    connect(btnClose, &QPushButton::clicked, this, [this, chip, taskId]() {
        attachedTasks.remove(taskId);
        chip->hide();
        chip->deleteLater();
    });
}

bool AbstractChatPage::eventFilter(QObject* obj, QEvent* event) {
    return QWidget::eventFilter(obj, event);
}

void AbstractChatPage::clearAttachments() {
    for (int i = 0; i < attachmentLayout->count(); ++i) {
        QWidget* w = attachmentLayout->itemAt(i)->widget();
        if (w && w->property("isChip").toBool()) {
            w->deleteLater();
        }
    }
    attachedTasks.clear();
}

void AbstractChatPage::addMessage(const QString& senderName, const QString& text, bool isReceived, const QMap<int, QString>& attachments) {
    bool consecutive = (lastSenderValid && lastSenderWasReceived == isReceived);
    lastSenderValid = true;
    lastSenderWasReceived = isReceived;

    QWidget* bubbleWidget = new QWidget(chatContainer);
    QHBoxLayout* bubbleLayout = new QHBoxLayout(bubbleWidget);
    bubbleLayout->setContentsMargins(0, consecutive ? 2 : 10, 0, 0);

    QFrame* innerBubble = new QFrame(bubbleWidget);
    innerBubble->setProperty("cssClass", "card");

    QVBoxLayout* innerLayout = new QVBoxLayout(innerBubble);
    innerLayout->setContentsMargins(12, 8, 12, 8);
    innerLayout->setSpacing(6);

    if (!consecutive) {
        QLabel* nameLabel = new QLabel(senderName, innerBubble);
        nameLabel->setObjectName("senderNameLabel");
        innerLayout->addWidget(nameLabel);
    }

    if (!attachments.isEmpty()) {
        QFrame* attachmentArea = new QFrame(innerBubble);
        QVBoxLayout* attLayout = new QVBoxLayout(attachmentArea);
        attLayout->setContentsMargins(0, 4, 0, 4);
        attLayout->setSpacing(6);

        for (auto it = attachments.begin(); it != attachments.end(); ++it) {
            QFrame* taskCard = new QFrame(attachmentArea);
            taskCard->setProperty("cssClass", "card");

            QHBoxLayout* tcLayout = new QHBoxLayout(taskCard);
            tcLayout->setContentsMargins(10, 6, 10, 6);
            tcLayout->setSpacing(8);

            QLabel* icon = new QLabel("+ ", taskCard);
            QLabel* title = new QLabel(it.value(), taskCard);
            QFont f = title->font();
            f.setPointSize(9);
            title->setFont(f);

            tcLayout->addWidget(icon);
            tcLayout->addWidget(title);
            tcLayout->addStretch();

            attLayout->addWidget(taskCard);
        }
        innerLayout->addWidget(attachmentArea);
    }

    if (!text.isEmpty()) {
        QList<SyntaxHighlighter::MessageBlock> blocks = SyntaxHighlighter::splitMessage(text);

        for (const auto& block : blocks) {
            if (!block.isCode) {
                if (block.text.trimmed().isEmpty()) continue;

                QLabel* messageLabel = new QLabel(innerBubble);
                messageLabel->setWordWrap(true);
                messageLabel->setMaximumWidth(400);
                messageLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
                messageLabel->setOpenExternalLinks(true);
                messageLabel->setTextFormat(Qt::RichText);

                QString textStr = block.text;
                textStr.replace("<", "&lt;").replace(">", "&gt;").replace("\n", "<br>");
                QRegularExpression inlineRe("`([^`]+)`");
                textStr.replace(inlineRe, "<span style=\"background-color: #2D2D30; font-family: monospace; padding: 2px 4px; border-radius: 4px;\">&nbsp;\\1&nbsp;</span>");

                messageLabel->setText(textStr);
                innerLayout->addWidget(messageLabel);
            } else {
                QFrame* codeFrame = new QFrame(innerBubble);
                codeFrame->setObjectName("codeFrame");
                codeFrame->setStyleSheet("#codeFrame { background-color: #1E1E1E; border-radius: 8px; }");

                QVBoxLayout* codeLayout = new QVBoxLayout(codeFrame);
                codeLayout->setContentsMargins(14, 14, 14, 14);
                codeLayout->setSpacing(0);

                QTextEdit* textEdit = new QTextEdit(codeFrame);
                textEdit->setReadOnly(true);
                textEdit->setFrameStyle(QFrame::NoFrame);
                textEdit->viewport()->setAutoFillBackground(false);
                textEdit->setStyleSheet("QTextEdit { background: transparent; border: none; }");

                textEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                textEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                textEdit->setLineWrapMode(QTextEdit::WidgetWidth);
                textEdit->document()->setDocumentMargin(0);

                SyntaxHighlighter::applyHighlighting(textEdit->document(), block.language, block.text);
                int fixedWidth = 360;
                textEdit->setFixedWidth(fixedWidth);

                textEdit->document()->setTextWidth(fixedWidth);
                int docHeight = qRound(textEdit->document()->size().height());
                QFontMetrics metrics(textEdit->font());
                int halfLineHeight = metrics.lineSpacing() / 2;

                textEdit->setFixedHeight(docHeight + halfLineHeight);
                codeFrame->setFixedHeight(docHeight + halfLineHeight + 28);

                codeLayout->addWidget(textEdit);
                innerLayout->addWidget(codeFrame);
            }
        }
    }

    if (isReceived) {
        bubbleLayout->addWidget(innerBubble);
        bubbleLayout->addStretch();
    } else {
        bubbleLayout->addStretch();
        bubbleLayout->addWidget(innerBubble);
    }
    chatLayout->addWidget(bubbleWidget);
}

void AbstractChatPage::addLoadingMessage(const QString& senderName) {
    if (currentLoadingBubble) return;
    bool consecutive = (lastSenderValid && lastSenderWasReceived == true);
    lastSenderValid = true;
    lastSenderWasReceived = true;

    currentLoadingBubble = new QWidget(chatContainer);
    QHBoxLayout* bubbleLayout = new QHBoxLayout(currentLoadingBubble);
    bubbleLayout->setContentsMargins(0, consecutive ? 2 : 10, 0, 0);

    QFrame* innerBubble = new QFrame(currentLoadingBubble);
    innerBubble->setProperty("cssClass", "card");

    QVBoxLayout* innerLayout = new QVBoxLayout(innerBubble);
    innerLayout->setContentsMargins(12, 8, 12, 8);
    innerLayout->setSpacing(4);

    if (!consecutive) {
        QLabel* nameLabel = new QLabel(senderName, innerBubble);
        nameLabel->setObjectName("senderNameLabel");
        innerLayout->addWidget(nameLabel);
    }

    QHBoxLayout* textRow = new QHBoxLayout();

    currentLoadingLabel = new QLabel("⠋ " + LanguageManager::tr("chat.generating"), innerBubble);
    currentLoadingLabel->setObjectName("loadingLabel");

    currentCancelBtn = new QPushButton(LanguageManager::tr("chat.stop"), innerBubble);
    currentCancelBtn->setCursor(Qt::PointingHandCursor);
    currentCancelBtn->setObjectName("cancelBtn");
    connect(currentCancelBtn, &QPushButton::clicked, this, &AbstractChatPage::cancelLoading);

    textRow->addWidget(currentLoadingLabel);
    textRow->addStretch();
    textRow->addWidget(currentCancelBtn);

    innerLayout->addLayout(textRow);

    bubbleLayout->addWidget(innerBubble);
    bubbleLayout->addStretch();

    chatLayout->addWidget(currentLoadingBubble);

    QTimer::singleShot(0, this, [this]() {
        scrollArea->widget()->adjustSize();
        QScrollBar* bar = scrollArea->verticalScrollBar();
        bar->setValue(bar->maximum());
    });

    setInputEnabled(false);
    spinnerFrame = 0;
    spinnerTimer->start(100);
}

void AbstractChatPage::finishLoadingMessage(const QString& text, bool isError) {
    if (!currentLoadingBubble) return;

    spinnerTimer->stop();

    if (isError) {
        if (currentLoadingLabel) {
            currentLoadingLabel->setText(text);
            currentLoadingLabel->setProperty("errorState", true);
            currentLoadingLabel->style()->unpolish(currentLoadingLabel);
            currentLoadingLabel->style()->polish(currentLoadingLabel);
            currentLoadingLabel->setWordWrap(true);
            currentLoadingLabel->setMaximumWidth(400);
        }

        if (currentCancelBtn) {
            currentCancelBtn->hide();
            currentCancelBtn->deleteLater();
            currentCancelBtn = nullptr;
        }

        currentLoadingLabel = nullptr;
        currentLoadingBubble = nullptr;
    } else {
        QString senderName = "AI";
        if (QLabel* nameLabel = currentLoadingBubble->findChild<QLabel*>("senderNameLabel")) {
            senderName = nameLabel->text();
        }

        currentLoadingBubble->hide();
        currentLoadingBubble->deleteLater();
        currentLoadingBubble = nullptr;
        currentLoadingLabel = nullptr;
        currentCancelBtn = nullptr;
        addMessage(senderName, text, true);
    }

    setInputEnabled(true);
    chatInput->setFocus();
    QTimer::singleShot(0, this, [this]() {
        scrollArea->widget()->adjustSize();
        scrollToBottom();
    });
}

void AbstractChatPage::cancelLoading() {
    onCancelGeneration();
    finishLoadingMessage(LanguageManager::tr("chat.generation_canceled"), true);
}

void AbstractChatPage::onSendClicked() {
    QString text = chatInput->text().trimmed();
    if (!text.isEmpty() || !attachedTasks.isEmpty()) {
        onSendMessageRequested(text);
        chatInput->clear();
    }
}

void AbstractChatPage::clearChat() {
    while (chatLayout->count() > 1) {
        QLayoutItem* item = chatLayout->takeAt(1);
        if (QWidget* widget = item->widget()) {
            widget->deleteLater();
        }
        delete item;

        currentLoadingLabel = nullptr;
        currentLoadingBubble = nullptr;
        if (currentCancelBtn) {
            currentCancelBtn = nullptr;
        }
        spinnerTimer->stop();
    }
}

void AbstractChatPage::refreshChatScrollPosition() {
    if (scrollArea && scrollArea->verticalScrollBar()) {
        QScrollBar* bar = scrollArea->verticalScrollBar();
        previousScrollPosition = bar->value();

        scrollArea->setProperty("prevScrollMax", bar->maximum());
        scrollArea->setProperty("prevItemCount", chatLayout->count());
    }
}

void AbstractChatPage::restoreChatScrollPosition() {
    if (scrollArea && scrollArea->verticalScrollBar()) {
        QTimer::singleShot(0, this, [this]() {
            scrollArea->widget()->adjustSize(); // Force layout recalculation first
            QScrollBar* bar = scrollArea->verticalScrollBar();

            int prevItemCount = scrollArea->property("prevItemCount").toInt();
            int newItemCount = chatLayout->count();
            int prevMax = scrollArea->property("prevScrollMax").toInt();

            bool wasAtBottom = (previousScrollPosition >= prevMax) && (prevMax > 0);

            if (newItemCount > prevItemCount || prevItemCount <= 1 || wasAtBottom) {
                bar->setValue(bar->maximum());
            } else {
                bar->setValue(previousScrollPosition);
            }
        });
    }
}

void AbstractChatPage::scrollToBottom() {
    if (scrollArea && scrollArea->verticalScrollBar()) {
        scrollArea->verticalScrollBar()->setValue(scrollArea->verticalScrollBar()->maximum());
    }
}