#pragma once

#include <QMainWindow>
#include <QStackedWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QFrame>
#include "DataStructures.h"

class GroupCard : public QFrame {
    Q_OBJECT
public:
GroupCard(const StudyGroup& group, bool isPinned, bool showManageOptions = true, QWidget* parent = nullptr);
signals:
    void openChatRequested(int groupId);
    void pinStateChanged();
    void manageMembersRequested(int groupId);
    void openTasksRequested(int groupId);
    void deleteRequested(int groupId);
};