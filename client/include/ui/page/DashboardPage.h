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
private:
    QVBoxLayout* pinnedGroupsLayout;
private slots:
    void onStartFocusClicked();
signals:
    void startFocusRequested();
    void openGroupChatRequested(int groupId);
    void openGroupTasksRequested(int groupId);
};