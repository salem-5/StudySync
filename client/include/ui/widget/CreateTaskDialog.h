#pragma once
#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>
#include <QFormLayout>
#include "DataStructures.h"

class CreateTaskDialog : public QDialog {
    Q_OBJECT

private:
    QLineEdit* titleInput;
    QLineEdit* tagInput;
    QComboBox* assigneeCombo;
    QComboBox* groupCombo;
    QCheckBox* completedCheck;

    QPushButton* btnCreate;
    QPushButton* btnCancel;

public:
    explicit CreateTaskDialog(QWidget* parent = nullptr);

signals:
    void taskCreated(Task task);

private slots:
    void handleCreate();
};