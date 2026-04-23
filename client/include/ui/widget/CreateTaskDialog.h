#pragma once
#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>
#include <QHBoxLayout>
#include "DataStructures.h"

class CreateTaskDialog : public QDialog {
    Q_OBJECT

protected:
    QLineEdit* titleInput;
    QLineEdit* tagInput;
    QComboBox* assigneeCombo;
    QComboBox* groupCombo;
    QCheckBox* completedCheck;

    QPushButton* btnCreate;
    QPushButton* btnCancel;
    QHBoxLayout* buttonLayout;

public:
    explicit CreateTaskDialog(QWidget* parent = nullptr);

    signals:
        void taskCreated(Task task);

protected slots:
    virtual void handleSubmit();
};