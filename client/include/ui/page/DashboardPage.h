#pragma once

#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QPushButton>
#include "DataStructures.h"

class QVBoxLayout;

class DashboardPage : public QWidget {
    Q_OBJECT
public:
    DashboardPage(QWidget* parent = nullptr);
    void refreshPinnedGroups();
    void refreshTaskCards();
    void refreshWelcomeCard();
private:
    QVBoxLayout* pinnedGroupsLayout;
    QVBoxLayout* tasksLayout;
    QLabel* welcomeSubLabel;
    QPushButton* welcomeBtn;
    bool isFocusActive = false;

public slots:
    void onStartFocusClicked();
    void setFocusActive(bool active);

signals:
    void startFocusRequested();
    void openGroupChatRequested(int groupId);
    void openGroupTasksRequested(int groupId);
    void gotoFocusRequested();
    void createTaskRequested();

};