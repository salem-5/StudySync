#include "ui/page/DashboardPage.h"
#include "ui/widget/TaskCard.h"
#include "ui/widget/GroupCard.h"
#include "ui/ClientState.h"
#include <iostream>
#include <QApplication>
#include <QFile>
#include <QGroupBox>
#include <QPushButton>
#include "LanguageManager.h"

DashboardPage::DashboardPage(QWidget* parent) : QWidget(parent) {
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(10, 10, 10, 10);

    QWidget* leftCol = new QWidget();
    QVBoxLayout* leftLayout = new QVBoxLayout(leftCol);
    leftLayout->setContentsMargins(0, 0, 0, 0);

    QGroupBox* welcomeGroup = new QGroupBox(LanguageManager::tr("dashboard.welcome"));
    QVBoxLayout* wLayout = new QVBoxLayout(welcomeGroup);

    const User* user = ClientState::getUser();
    QString username = user ? QString::fromStdString(user->getUsername()) : LanguageManager::tr("dashboard.student_placeholder");
    QLabel* wSub = new QLabel(LanguageManager::tr("dashboard.welcome_back_prefix") + username + "!\n" + LanguageManager::tr("dashboard.welcome_back_suffix"));

    QPushButton* wBtn = new QPushButton(LanguageManager::tr("dashboard.start_focus"));
    connect(wBtn, &QPushButton::clicked, this, &DashboardPage::onStartFocusClicked);
    wLayout->addWidget(wSub);
    wLayout->addWidget(wBtn);

    QGroupBox* tasksGroup = new QGroupBox(LanguageManager::tr("dashboard.tasks_due"));
    tasksLayout = new QVBoxLayout(tasksGroup);

    refreshTaskCards();
    leftLayout->addWidget(welcomeGroup);
    leftLayout->addWidget(tasksGroup);

    QGroupBox* rightCol = new QGroupBox(LanguageManager::tr("dashboard.pinned_groups"));
    pinnedGroupsLayout = new QVBoxLayout(rightCol);
    refreshPinnedGroups();

    layout->addWidget(leftCol);
    layout->addWidget(rightCol);

    layout->setStretch(0, 2);
    layout->setStretch(1, 1);
}

void DashboardPage::refreshTaskCards() {
    QLayoutItem* item;
    while ((item = tasksLayout->takeAt(0)) != nullptr) {
        if (item->widget())
            item->widget()->deleteLater(); //fix by chatgpt, to prevent deletion of object during qt event processing.
        delete item;
    }

    const std::vector<Task>& allTasks = ClientState::getTasks();
    const User* currentUser = ClientState::getUser();

    for (const Task& task : allTasks) {
        if (task.getIsCompleted() || (currentUser && task.getAssignedToId() != currentUser->getId())) {
            continue;
        }

        QString groupName = LanguageManager::tr("task.group.personal");
        const StudyGroup* group = ClientState::getGroupById(task.getGroupId());
        if (group) {
            groupName = QString::fromStdString(group->getName());
        }

        TaskCard* card = new TaskCard(task, groupName, nullptr, false);
        connect(card, &TaskCard::taskStateChanged, this, &DashboardPage::refreshTaskCards);

        tasksLayout->addWidget(card);
    }

    tasksLayout->addStretch();
}

void DashboardPage::refreshPinnedGroups() {
    QLayoutItem* item;
    while ((item = pinnedGroupsLayout->takeAt(0)) != nullptr) {
        if (item->widget()) delete item->widget();
        delete item;
    }
    const std::vector<StudyGroup>& allGroups = ClientState::getStudyGroups();
    for (const StudyGroup& group : allGroups) {
        if (!ClientState::isGroupPinned(group.getId()))
            continue;
        GroupCard* pinnedCard = new GroupCard(group, true);
        connect(pinnedCard, &GroupCard::openChatRequested, this, &DashboardPage::openGroupChatRequested);
        connect(pinnedCard, &GroupCard::openTasksRequested, this, &DashboardPage::openGroupTasksRequested);
        connect(pinnedCard, &GroupCard::pinStateChanged, this, &DashboardPage::refreshPinnedGroups);
        connect(pinnedCard, &GroupCard::deleteRequested, this, [this](int id) {
            ClientState::deleteGroup(id);
            refreshPinnedGroups();
            refreshTaskCards();
        });
        pinnedGroupsLayout->addWidget(pinnedCard);
    }
    pinnedGroupsLayout->addStretch();
}

void DashboardPage::onStartFocusClicked() {
    emit startFocusRequested();
}