#include "ui/page/GroupsPage.h"
#include "ui/widget/GroupCard.h"
#include "ui/ClientState.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <algorithm>
#include "LanguageManager.h"
#include "ui/widget/CreateGroupDialog.h"
#include "ui/widget/ManageMembersDialog.h"

GroupsPage::GroupsPage(QWidget* parent) : QWidget(parent) {
    QVBoxLayout* layout = new QVBoxLayout(this);

    QHBoxLayout* headerLayout = new QHBoxLayout();
    headerLayout->addStretch();
    QPushButton* createGroupButton = new QPushButton(LanguageManager::tr("group.create"));
    headerLayout->addWidget(createGroupButton);

    gridLayout = new QGridLayout();
    connect(createGroupButton, &QPushButton::clicked, this, [this]() {
        CreateGroupDialog* dialog = new CreateGroupDialog(this);
        connect(dialog, &CreateGroupDialog::groupCreated, this, &GroupsPage::loadGroups);
        dialog->exec();
    });

    layout->addLayout(headerLayout);
    layout->addLayout(gridLayout);

    loadGroups();
}

void GroupsPage::loadGroups() {
    QLayoutItem* item;
    while ((item = gridLayout->takeAt(0)) != nullptr) {
        if (item->widget()) delete item->widget();
        delete item;
    }

    const std::vector<StudyGroup>& allGroups = ClientState::getStudyGroups();
    const User* user = ClientState::getUser();

    int row = 0;
    int col = 0;

    for (const StudyGroup& group : allGroups) {
        bool isPinned = false;
        if (user) {
            const auto& pinnedIds = user->getPinnedGroupIds();
            isPinned = std::find(pinnedIds.begin(), pinnedIds.end(), group.getId()) != pinnedIds.end();
        }

        GroupCard* card = new GroupCard(group, isPinned);

        connect(card, &GroupCard::openChatRequested, this, &GroupsPage::openGroupChatRequested);
        connect(card, &GroupCard::openTasksRequested, this, &GroupsPage::openGroupTasksRequested);
        connect(card, &GroupCard::manageMembersRequested, this, [this, id = group.getId()]() {
            ManageMembersDialog* dialog = new ManageMembersDialog(id, this);
            connect(dialog, &ManageMembersDialog::membersUpdated, this, &GroupsPage::loadGroups);
            dialog->exec();
        });
        connect(card, &GroupCard::deleteRequested, this, [this](int id) {
            ClientState::deleteGroup(id);
            loadGroups();
            emit groupsChanged();
        });

        gridLayout->addWidget(card, row, col);
        col++;
        if (col > 1) {
            col = 0;
            row++;
        }
    }

    gridLayout->setRowStretch(row + 1, 1);
}