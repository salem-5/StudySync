#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QLineEdit>
#include <QPushButton>
#include <QMap>
#include <QFrame>
#include <QTimer>
#include <QLabel>

class AbstractChatPage : public QWidget {
    Q_OBJECT
public:
    AbstractChatPage(QWidget* parent = nullptr);
    virtual ~AbstractChatPage() = default;
    void clearChat();
    void addMessage(const QString& senderName, const QString& text, bool isReceived, const QMap<int, QString>& attachments = QMap<int, QString>());
    void addAttachmentChip(int taskId, const QString& title);
    void clearAttachments();

    void addLoadingMessage(const QString& senderName);
    void finishLoadingMessage(const QString& text, bool isError = false);
    void cancelLoading();

protected:
    virtual void onSendMessageRequested(const QString& text) = 0;
    virtual void onCancelGeneration() {}
    bool eventFilter(QObject* obj, QEvent* event) override;
    void setInputEnabled(bool enabled);

    QVBoxLayout* mainLayout;

    QFrame* inputCard;
    QVBoxLayout* inputCardLayout;
    QWidget* attachmentContainer;
    QHBoxLayout* attachmentLayout;
    QPushButton* btnAddAttachment;
    QMap<int, QString> attachedTasks;

    QLineEdit* chatInput;
    QPushButton* btnSend;

    virtual void onAddAttachmentClicked() {};

private:
    void setupUi();
    void onSendClicked();

    QScrollArea* scrollArea;
    QWidget* chatContainer;
    QVBoxLayout* chatLayout;

    bool lastSenderValid = false;
    bool lastSenderWasReceived = false;

    QTimer* spinnerTimer;
    int spinnerFrame;
    QLabel* currentLoadingLabel;
    QPushButton* currentCancelBtn;
    QWidget* currentLoadingBubble;
};