#include "ui/page/AbstractChatPage.h"
#include <QHBoxLayout>
#include <QScrollBar>
#include <QTimer>
#include <QLabel>
#include <QFrame>

AbstractChatPage::AbstractChatPage(QWidget* parent) : QWidget(parent) {
    setupUi();
}

void AbstractChatPage::setupUi() {
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    chatContainer = new QWidget();
    chatLayout = new QVBoxLayout(chatContainer);
    chatLayout->setContentsMargins(5, 5, 5, 5);
    chatLayout->setSpacing(0);
    chatLayout->addStretch();

    scrollArea->setWidget(chatContainer);
    mainLayout->addWidget(scrollArea);

    QHBoxLayout* inputLayout = new QHBoxLayout();
    chatInput = new QLineEdit(this);
    chatInput->setPlaceholderText("Type a message...");

    btnSend = new QPushButton("Send", this);

    inputLayout->addWidget(chatInput);
    inputLayout->addWidget(btnSend);
    mainLayout->addLayout(inputLayout);

    connect(btnSend, &QPushButton::clicked, this, &AbstractChatPage::onSendClicked);
    connect(chatInput, &QLineEdit::returnPressed, this, &AbstractChatPage::onSendClicked);
}

void AbstractChatPage::addMessage(const QString& senderName, const QString& text, bool isReceived) {
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
    innerLayout->setSpacing(2);

    if (!consecutive) {
        QLabel* nameLabel = new QLabel(senderName, innerBubble);
        nameLabel->setObjectName("senderNameLabel");
        innerLayout->addWidget(nameLabel);
    }

    QLabel* messageLabel = new QLabel(text, innerBubble);
    messageLabel->setWordWrap(true);
    messageLabel->setMaximumWidth(400);
    innerLayout->addWidget(messageLabel);

    if (isReceived) {
        bubbleLayout->addWidget(innerBubble);
        bubbleLayout->addStretch();
    } else {
        bubbleLayout->addStretch();
        bubbleLayout->addWidget(innerBubble);
    }
    chatLayout->addWidget(bubbleWidget);
}

void AbstractChatPage::onSendClicked() {
    QString text = chatInput->text().trimmed();
    if (!text.isEmpty()) {
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
    }
    lastSenderValid = false;
}