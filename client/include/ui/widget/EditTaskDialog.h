#pragma once
#include "ui/widget/CreateTaskDialog.h"
#include "DataStructures.h"

class EditTaskDialog : public CreateTaskDialog {
    Q_OBJECT

public:
    explicit EditTaskDialog(const Task& task, QWidget* parent = nullptr);

    signals:
        void taskUpdated();

protected slots:
    void handleSubmit() override;

private:
    int taskId;
    QPushButton* btnDelete;
};