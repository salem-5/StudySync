#pragma once
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>

class CreateGroupDialog : public QDialog {
    Q_OBJECT
public:
    explicit CreateGroupDialog(QWidget* parent = nullptr);

    signals:
        void groupCreated();

private:
    QLineEdit* nameInput;
    QPushButton* btnCreate;
    QPushButton* btnCancel;
};