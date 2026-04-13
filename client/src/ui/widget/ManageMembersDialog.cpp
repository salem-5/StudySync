#include "ui/widget/ManageMembersDialog.h"
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QScrollArea>
#include "ui/ClientState.h"
#include "LanguageManager.h"

ManageMembersDialog::ManageMembersDialog(int groupId, QWidget* parent) : QDialog(parent), groupId(groupId) {
    setWindowTitle(LanguageManager::tr("group.members_title"));
    setMinimumSize(350, 400);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    QWidget* listContainer = new QWidget();
    listLayout = new QVBoxLayout(listContainer);
    listLayout->setAlignment(Qt::AlignTop);
    scrollArea->setWidget(listContainer);

    mainLayout->addWidget(scrollArea);

    QHBoxLayout* addLayout = new QHBoxLayout();
    QLineEdit* usernameInput = new QLineEdit();
    usernameInput->setPlaceholderText(LanguageManager::tr("task.add_member_placeholder"));
    QPushButton* btnAdd = new QPushButton(LanguageManager::tr("group.add_member"));

    connect(btnAdd, &QPushButton::clicked, this, [this, usernameInput]() {
        QString uname = usernameInput->text().trimmed();
        if (!uname.isEmpty()) {
            ClientState::inviteMemberToGroup(this->groupId, uname.toStdString());
            usernameInput->clear();
            refreshList();
            emit membersUpdated();
        }
    });

    addLayout->addWidget(usernameInput);
    addLayout->addWidget(btnAdd);
    mainLayout->addLayout(addLayout);

    refreshList();
    connect(ClientNotifier::instance(), &ClientNotifier::groupsChanged, this, &ManageMembersDialog::refreshList);
    connect(ClientNotifier::instance(), &ClientNotifier::userChanged, this, &ManageMembersDialog::refreshList);
}

void ManageMembersDialog::refreshList() {
    QLayoutItem* item;
    while ((item = listLayout->takeAt(0)) != nullptr) {
        if (item->widget()) delete item->widget();
        delete item;
    }

    const StudyGroup* group = ClientState::getGroupById(groupId);
    if (!group) return;

    const User* currentUser = ClientState::getUser();
    auto members = group->getMemberIds();
    auto invited = group->getInvitedMemberIds();

    if (members.empty() && invited.empty()) {
        listLayout->addWidget(new QLabel(LanguageManager::tr("group.no_members")));
        return;
    }

    auto createMemberRow = [this](const QString& name, const QString& buttonText, std::function<void()> onClick) {
        QFrame* rowWidget = new QFrame();

        rowWidget->setAttribute(Qt::WA_StyledBackground, true);
        rowWidget->setProperty("cssClass", "card");
        rowWidget->setFrameShape(QFrame::StyledPanel);

        QHBoxLayout* rowLayout = new QHBoxLayout(rowWidget);
        rowLayout->setContentsMargins(15, 12, 15, 12);
        rowLayout->setSpacing(15);

        QLabel* nameLabel = new QLabel(name);
        QFont nameFont = nameLabel->font();
        nameFont.setPointSize(11);
        nameFont.setBold(true);
        nameLabel->setFont(nameFont);

        rowLayout->addWidget(nameLabel);
        rowLayout->addStretch();

        if (!buttonText.isEmpty()) {
            QPushButton* actionBtn = new QPushButton(buttonText);
            actionBtn->setCursor(Qt::PointingHandCursor);
            connect(actionBtn, &QPushButton::clicked, this, onClick);
            rowLayout->addWidget(actionBtn);
        }

        return rowWidget;
    };

    for (int mId : members) {
        QString name = QString::fromStdString(ClientState::getUsername(mId));
        bool isSelf = (currentUser && currentUser->getId() == mId);

        QString btnText = isSelf ? "" : LanguageManager::tr("group.remove_member");

        QWidget* row = createMemberRow(name, btnText, [this, mId]() {
            ClientState::removeMemberFromGroup(groupId, mId);
            refreshList();
            emit membersUpdated();
        });
        listLayout->addWidget(row);
    }

    for (int mId : invited) {
        QString name = QString::fromStdString(ClientState::getUsername(mId)) + " (" + LanguageManager::tr("group.invited_status") + ")";

        QWidget* row = createMemberRow(name, LanguageManager::tr("group.cancel_invite"), [this, mId]() {
            ClientState::cancelInvite(groupId, mId);
            refreshList();
            emit membersUpdated();
        });
        listLayout->addWidget(row);
    }
}