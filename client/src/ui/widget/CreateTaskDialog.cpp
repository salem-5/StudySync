#include "ui/widget/CreateTaskDialog.h"
#include "LanguageManager.h"
#include "ui/ClientState.h"
#include "ui/page/DashboardPage.h"

CreateTaskDialog::CreateTaskDialog(QWidget* parent)
    : QDialog(parent) {

    setWindowTitle(LanguageManager::tr("task.create"));
    resize(350, 250);

    QFormLayout* layout = new QFormLayout(this);

    titleInput = new QLineEdit(this);
    tagInput = new QLineEdit(this);

    groupCombo = new QComboBox(this);
    assigneeCombo = new QComboBox(this);

    completedCheck = new QCheckBox(LanguageManager::tr("task.completed"), this);

    btnCreate = new QPushButton(LanguageManager::tr("action.create"), this);
    btnCancel = new QPushButton(LanguageManager::tr("action.cancel"), this);

    auto updateAssignees = [this]() {
        assigneeCombo->clear();
        int currentGroupId = groupCombo->currentData().toInt();
        const StudyGroup* group = ClientState::getGroupById(currentGroupId);
        if (group) {
            for (int mId : group->getMemberIds()) {
                QString name = QString::fromStdString(ClientState::getUsername(mId));
                assigneeCombo->addItem(name, mId);
            }
        }
    };
    auto refreshGroups = [this, updateAssignees]() {
        QVariant currentData = groupCombo->currentData();
        groupCombo->blockSignals(true);
        groupCombo->clear();

        for (const auto& group : ClientState::getStudyGroups()) {
            groupCombo->addItem(QString::fromStdString(group.getName()), group.getId());
        }

        int idx = groupCombo->findData(currentData);
        if (idx != -1) {
            groupCombo->setCurrentIndex(idx);
        } else if (groupCombo->count() > 0) {
            groupCombo->setCurrentIndex(0);
        }
        groupCombo->blockSignals(false);
        updateAssignees();
    };
    connect(groupCombo, &QComboBox::currentIndexChanged, this, updateAssignees);

    refreshGroups();
    connect(ClientNotifier::instance(), &ClientNotifier::groupsChanged, this, refreshGroups);
    connect(ClientNotifier::instance(), &ClientNotifier::userChanged, this, updateAssignees);

    layout->addRow(LanguageManager::tr("task.form.title"), titleInput);
    layout->addRow(LanguageManager::tr("task.form.tag"), tagInput);
    layout->addRow(LanguageManager::tr("task.form.group"), groupCombo);
    layout->addRow(LanguageManager::tr("task.form.assignee"), assigneeCombo);
    layout->addRow(completedCheck);
    layout->addRow(btnCreate, btnCancel);

    connect(btnCreate, &QPushButton::clicked, this, &CreateTaskDialog::handleCreate);
    connect(btnCancel, &QPushButton::clicked, this, &CreateTaskDialog::reject);
}

void CreateTaskDialog::handleCreate() {
    int id = rand();
    Task task(
        id,
        titleInput->text().toStdString(),
        tagInput->text().toStdString(),
        completedCheck->isChecked(),
        1,
        assigneeCombo->currentData().toInt(),
        groupCombo->currentData().toInt()
    );

    emit taskCreated(task);
    accept();
}