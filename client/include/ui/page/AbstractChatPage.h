#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QLineEdit>
#include <QPushButton>

class AbstractChatPage : public QWidget {
    Q_OBJECT
public:
    AbstractChatPage(QWidget* parent = nullptr);
    virtual ~AbstractChatPage() = default;
    void clearChat();
    void addMessage(const QString& senderName, const QString& text, bool isReceived);

protected:
    virtual void onSendMessageRequested(const QString& text) = 0;

    QVBoxLayout* mainLayout;
    QLineEdit* chatInput;
    QPushButton* btnSend;

private:
    void setupUi();
    void onSendClicked();

    QScrollArea* scrollArea;
    QWidget* chatContainer;
    QVBoxLayout* chatLayout;

    bool lastSenderValid = false;
    bool lastSenderWasReceived = false;
};