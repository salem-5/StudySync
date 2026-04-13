#include "ui/widget/EditTaskDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QLabel>
#include "ui/ClientState.h"
#include "LanguageManager.h"

EditTaskDialog::EditTaskDialog(const Task& task, QWidget* parent) : QDialog(parent), taskId(task.getId()) {
    setWindowTitle(LanguageManager::tr("task.edit"));
    setMinimumWidth(300);

    QVBoxLayout* layout = new QVBoxLayout(this);

    layout->addWidget(new QLabel(LanguageManager::tr("task.form.title")));
    titleInput = new QLineEdit(QString::fromStdString(task.getTitle()));
    layout->addWidget(titleInput);

    layout->addWidget(new QLabel(LanguageManager::tr("task.form.tag")));
    tagInput = new QLineEdit(QString::fromStdString(task.getTag()));
    layout->addWidget(tagInput);

    layout->addWidget(new QLabel(LanguageManager::tr("task.form.assignee")));
    assigneeCombo = new QComboBox();
    
    const StudyGroup* group = ClientState::getGroupById(task.getGroupId());
    if (group) {
        for (int mId : group->getMemberIds()) {
            QString name = QString::fromStdString(ClientState::getUsername(mId));
            assigneeCombo->addItem(name, mId);
        }
        int index = assigneeCombo->findData(task.getAssignedToId());
        if (index != -1) {
            assigneeCombo->setCurrentIndex(index);
        }
    }
    layout->addWidget(assigneeCombo);

    QPushButton* btnDelete = new QPushButton(LanguageManager::tr("task.delete"));
    QHBoxLayout* buttonsLayout = new QHBoxLayout();
    QPushButton* btnSave = new QPushButton(LanguageManager::tr("task.save"));
    QPushButton* btnCancel = new QPushButton(LanguageManager::tr("dialog.cancel"));

    buttonsLayout->addWidget(btnDelete);
    buttonsLayout->addStretch();
    buttonsLayout->addWidget(btnCancel);
    buttonsLayout->addWidget(btnSave);
    layout->addLayout(buttonsLayout);

    connect(btnDelete, &QPushButton::clicked, this, [this]() {
        QMessageBox::StandardButton reply = QMessageBox::question(
            this,
            LanguageManager::tr("task.delete_confirm_title"),
            LanguageManager::tr("task.delete_confirm_msg"),
            QMessageBox::Yes | QMessageBox::No
        );

        if (reply == QMessageBox::Yes) {
            ClientState::deleteTask(taskId);
            emit taskUpdated();
            accept();
        }
    });
    connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);
    connect(btnSave, &QPushButton::clicked, this, [this]() {
        std::string newTitle = titleInput->text().toStdString();
        std::string newTag = tagInput->text().toStdString();
        int newAssignee = assigneeCombo->currentData().toInt();
        ClientState::editTask(taskId, newTitle, newTag, newAssignee);
        emit taskUpdated();
        accept();
    });
}