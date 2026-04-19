#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QGroupBox>
#include "DataStructures.h"

class TasksPage : public QWidget {
    Q_OBJECT
public:
    TasksPage(QWidget* parent = nullptr);
    void loadTasks(int groupId);

signals:
    void backToGroupsRequested();
    void tasksChanged();
    void askAiRequested(int taskId);

private:
    QVBoxLayout* tasksLayout;
    QGroupBox* group;
    int currentGroupId = -1;
};