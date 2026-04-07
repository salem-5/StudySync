#pragma once

#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include "DataStructures.h"

class QVBoxLayout;

class DashboardPage : public QWidget {
    Q_OBJECT
public:
    DashboardPage(QWidget* parent = nullptr);
    void refreshPinnedGroups();
    void refreshTaskCards();
private:
    QVBoxLayout* pinnedGroupsLayout;
    QVBoxLayout* tasksLayout;
private slots:
    void onStartFocusClicked();
signals:
    void startFocusRequested();
    void openGroupChatRequested(int groupId);
    void openGroupTasksRequested(int groupId);
};