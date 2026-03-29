#include "ui/page/GroupChatPage.h"
#include "ui/ClientState.h"

GroupChatPage::GroupChatPage(QWidget* parent) : AbstractChatPage(parent) {
    QPushButton* btnBack = new QPushButton("Back to Groups", this);
    connect(btnBack, &QPushButton::clicked, this, &GroupChatPage::backToGroupsRequested);
    mainLayout->insertWidget(0, btnBack, 0, Qt::AlignLeft);
}
void GroupChatPage::loadChat(int groupId) {
    currentGroupId = groupId;
    this->clearChat();

    const StudyGroup* studyGroup = ClientState::getGroupById(groupId);
    if (!studyGroup) return;

    const User* currentUser = ClientState::getUser();
    int myId = currentUser ? currentUser->getId() : -1;

    for (const Message& msg : studyGroup->getMessages()) {
        bool isMe = (msg.getUserId() == myId);
        QString name = QString::fromStdString(ClientState::getUsername(msg.getUserId()));
        QString text = QString::fromStdString(msg.getText());

        addMessage(name, text, !isMe);
    }
}

void GroupChatPage::onSendMessageRequested(const QString& text) {
    ClientState::mockSendMessage(currentGroupId, text.toStdString());
    loadChat(currentGroupId);
}