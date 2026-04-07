#pragma once

#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QFrame>
#include "DataStructures.h"

class TaskCard : public QFrame {
    Q_OBJECT
public:
    TaskCard(const Task& task, const QString& groupName, QWidget* parent = nullptr, bool hasEditButton = true);
signals:
    void editRequested(int taskId);
    void taskStateChanged();
};