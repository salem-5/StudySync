#include "ui/widget/CreateGroupDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include "LanguageManager.h"
#include "ui/ClientState.h"

CreateGroupDialog::CreateGroupDialog(QWidget* parent) : QDialog(parent) {
    setWindowTitle(LanguageManager::tr("group.create"));
    setMinimumWidth(300);

    QVBoxLayout* layout = new QVBoxLayout(this);

    layout->addWidget(new QLabel("Group Name:"));
    nameInput = new QLineEdit(this);
    layout->addWidget(nameInput);

    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnCreate = new QPushButton(LanguageManager::tr("action.create"), this);
    btnCancel = new QPushButton(LanguageManager::tr("action.cancel"), this);

    btnLayout->addStretch();
    btnLayout->addWidget(btnCancel);
    btnLayout->addWidget(btnCreate);

    layout->addLayout(btnLayout);

    connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);
    connect(btnCreate, &QPushButton::clicked, this, [this]() {
        if (!nameInput->text().trimmed().isEmpty()) {
            ClientState::mockCreateGroup(nameInput->text().toStdString());
            emit groupCreated();
            accept();
        }
    });
}