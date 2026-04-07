#pragma once
#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include "DataStructures.h"

class EditTaskDialog : public QDialog {
    Q_OBJECT
public:
    explicit EditTaskDialog(const Task& task, QWidget* parent = nullptr);

    signals:
        void taskUpdated();

private:
    int taskId;
    QLineEdit* titleInput;
    QLineEdit* tagInput;
    QComboBox* assigneeCombo;
};