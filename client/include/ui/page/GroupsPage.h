#pragma once

#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QWidget> // Fixed missing QWidget include
#include "DataStructures.h"

class QGridLayout;

class GroupsPage : public QWidget {
    Q_OBJECT
public slots:
void loadGroups();
private:
    QGridLayout* gridLayout;
public:
    GroupsPage(QWidget* parent = nullptr);
signals:
void openGroupChatRequested(int groupId);
void groupsChanged();
void openGroupTasksRequested(int groupId);
};