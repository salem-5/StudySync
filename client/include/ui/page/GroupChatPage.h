#pragma once

#include "AbstractChatPage.h"
#include "DataStructures.h"
#include <QLabel>

class GroupChatPage : public AbstractChatPage {
    Q_OBJECT
public:
    GroupChatPage(QWidget* parent = nullptr);
    void loadChat(int groupId);

    signals:
        void backToGroupsRequested();

protected:
    void onSendMessageRequested(const QString& text) override;

private:
    QLabel* groupTitleLabel;
    int currentGroupId;
};