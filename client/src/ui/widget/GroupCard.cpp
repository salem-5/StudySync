#include "ui/widget/GroupCard.h"
#include <QPushButton>
#include <QMessageBox>
#include <QHBoxLayout>
#include "ui/ClientState.h"
#include "LanguageManager.h"

GroupCard::GroupCard(const StudyGroup& group, bool isPinned, QWidget* parent) : QFrame(parent) {
    setAttribute(Qt::WA_StyledBackground, true);
    setProperty("cssClass", "card");

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(15, 15, 15, 15);

    QHBoxLayout* headerLayout = new QHBoxLayout();
    headerLayout->setContentsMargins(0,0,0,0);

    QLabel* title = new QLabel(QString::fromStdString(group.getName()));
    QFont titleFont = title->font();
    titleFont.setBold(true);
    titleFont.setPointSize(12);
    title->setFont(titleFont);


    QPushButton* btnPin = new QPushButton(isPinned ? LanguageManager::tr("group.pinned") : LanguageManager::tr("group.pin"));
    btnPin->setCursor(Qt::PointingHandCursor);
    connect(btnPin, &QPushButton::clicked, this, [this, btnPin, id = group.getId()]() {
        ClientState::togglePinGroup(id);
        bool currentlyPinned = ClientState::isGroupPinned(id);
        btnPin->setText(currentlyPinned ? LanguageManager::tr("group.pinned") : LanguageManager::tr("group.pin"));
        emit pinStateChanged();
    });

    headerLayout->addWidget(title);
    headerLayout->addStretch();
    headerLayout->addWidget(btnPin);

    size_t memberCount = group.getMemberIds().size();
    QLabel* members = new QLabel(LanguageManager::tr("group.members_count").arg(memberCount));
    members->setObjectName("members");

    QPushButton* btnManageMembers = new QPushButton(LanguageManager::tr("group.manage_members"));
    QPushButton* btnChat = new QPushButton(LanguageManager::tr("group.open_chat"));
    QPushButton* btnTasks = new QPushButton(LanguageManager::tr("group.open_tasks"));
    QPushButton* btnDelete = new QPushButton(LanguageManager::tr("group.delete"));
    headerLayout->addWidget(btnDelete);
    btnDelete->setCursor(Qt::PointingHandCursor);
    connect(btnDelete, &QPushButton::clicked, this, [this, id = group.getId()]() {
        QMessageBox::StandardButton reply = QMessageBox::question(
            this,
            LanguageManager::tr("group.delete_confirm_title"),
            LanguageManager::tr("group.delete_confirm_msg"),
            QMessageBox::Yes | QMessageBox::No
        );

        if (reply == QMessageBox::Yes) {
            emit deleteRequested(id);
        }
    });
    connect(btnManageMembers, &QPushButton::clicked, this, [this, id = group.getId()]() {
        emit manageMembersRequested(id);
    });
    connect(btnChat, &QPushButton::clicked, this, [this, id = group.getId()]() {
        emit openChatRequested(id);
    });
    connect(btnTasks, &QPushButton::clicked, this, [this, id = group.getId()]() {
        emit openTasksRequested(id);
    });

    QHBoxLayout* actionsLayout = new QHBoxLayout();
    actionsLayout->addWidget(btnManageMembers);
    actionsLayout->addWidget(btnChat);
    actionsLayout->addWidget(btnTasks);
    layout->addLayout(headerLayout);
    layout->addWidget(members);
    layout->addLayout(actionsLayout);
}