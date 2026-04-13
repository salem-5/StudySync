#include "ui/page/TasksPage.h"
#include "ui/widget/TaskCard.h"
#include "ui/ClientState.h"
#include <QVBoxLayout>
#include <QPushButton>
#include "LanguageManager.h"
#include "ui/MainWindow.h"
#include "ui/widget/EditTaskDialog.h"

TasksPage::TasksPage(QWidget* parent) : QWidget(parent) {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    QPushButton* btnBack = new QPushButton(LanguageManager::tr("nav.back_to_groups"));
    connect(btnBack, &QPushButton::clicked, this, &TasksPage::backToGroupsRequested);

    connect(btnBack, &QPushButton::clicked, this, [this]() {
        currentGroupId = -1;
    });

    mainLayout->addWidget(btnBack, 0, Qt::AlignLeft);

    group = new QGroupBox(LanguageManager::tr("task.title"));
    tasksLayout = new QVBoxLayout(group);

    mainLayout->addWidget(group);

    connect(ClientNotifier::instance(), &ClientNotifier::tasksChanged, this, [this]() {
        if (currentGroupId != -1) loadTasks(currentGroupId);
    });
}

void TasksPage::loadTasks(int groupId) {
    currentGroupId = groupId;
    QLayoutItem* item;
    while ((item = tasksLayout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }

    const StudyGroup* studyGroup = ClientState::getGroupById(groupId);
    if (!studyGroup) return;

    QString groupName = QString::fromStdString(studyGroup->getName());
    group->setTitle(groupName + " " + LanguageManager::tr("task.suffix"));

    const std::vector<Task>& allTasks = ClientState::getTasks();
    for (const Task& t : allTasks) {
        if (t.getGroupId() == groupId) {
            TaskCard* card = new TaskCard(t, groupName);


            connect(card, &TaskCard::editRequested, this, [this, t, groupId]() {
                EditTaskDialog* dialog = new EditTaskDialog(t, this);
                dialog->exec();
            });
            tasksLayout->addWidget(card);
        }
    }
    tasksLayout->addStretch();
}