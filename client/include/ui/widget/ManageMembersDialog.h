#pragma once
#include <QDialog>
#include <QVBoxLayout>
#include <QLineEdit>
#include "DataStructures.h"

class ManageMembersDialog : public QDialog {
    Q_OBJECT
public:
    explicit ManageMembersDialog(int groupId, QWidget* parent = nullptr);

    signals:
        void membersUpdated();

private:
    int groupId;
    QVBoxLayout* listLayout;
    void refreshList();
};