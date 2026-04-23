#include "ui/widget/EditTaskDialog.h"
#include <QMessageBox>
#include "ui/ClientState.h"
#include "LanguageManager.h"

EditTaskDialog::EditTaskDialog(const Task& task, QWidget* parent)
    : CreateTaskDialog(parent), taskId(task.getId()) {

    setWindowTitle(LanguageManager::tr("task.edit"));

    titleInput->setText(QString::fromStdString(task.getTitle()));
    tagInput->setText(QString::fromStdString(task.getTag()));

    completedCheck->setChecked(task.getIsCompleted());

    int groupIdx = groupCombo->findData(task.getGroupId());
    if (groupIdx != -1) {
        groupCombo->setCurrentIndex(groupIdx);
    }

    int assigneeIdx = assigneeCombo->findData(task.getAssignedToId());
    if (assigneeIdx != -1) {
        assigneeCombo->setCurrentIndex(assigneeIdx);
    }

    btnCreate->setText(LanguageManager::tr("task.save"));

    btnDelete = new QPushButton(LanguageManager::tr("task.delete"), this);
    btnDelete->setCursor(Qt::PointingHandCursor);
    buttonLayout->insertWidget(0, btnDelete);

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
}

void EditTaskDialog::handleSubmit() {
    std::string newTitle = titleInput->text().toStdString();
    std::string newTag = tagInput->text().toStdString();
    int newAssignee = assigneeCombo->currentData().toInt();
    bool isCompleted = completedCheck->isChecked();

    ClientState::editTask(taskId, newTitle, newTag, newAssignee);
    ClientState::toggleTaskCompletion(taskId, isCompleted);
    emit taskUpdated();
    accept();
}