#include "ui/widget/CreateTaskDialog.h"
#include "LanguageManager.h"
#include "ui/ClientState.h"
#include "ui/page/DashboardPage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFrame>

CreateTaskDialog::CreateTaskDialog(QWidget* parent)
    : QDialog(parent) {

    setWindowTitle(LanguageManager::tr("task.create"));
    resize(450, 220);
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    QFrame* cardFrame = new QFrame(this);
    cardFrame->setProperty("cssClass", "card");

    QHBoxLayout* cardLayout = new QHBoxLayout(cardFrame);
    cardLayout->setContentsMargins(15, 12, 15, 12);
    cardLayout->setSpacing(15);

    completedCheck = new QCheckBox(this);
    cardLayout->addWidget(completedCheck, 0, Qt::AlignTop);

    QVBoxLayout* textLayout = new QVBoxLayout();
    textLayout->setContentsMargins(0, 0, 0, 0);
    textLayout->setSpacing(8);

    titleInput = new QLineEdit(this);
    titleInput->setObjectName("seamlessTitle");
    titleInput->setPlaceholderText(LanguageManager::tr("task.form.title") + "...");

    QFont titleFont = titleInput->font();
    titleFont.setPointSize(14);
    titleFont.setBold(true);
    titleInput->setFont(titleFont);
    textLayout->addWidget(titleInput);

    QHBoxLayout* subtitleLayout = new QHBoxLayout();
    subtitleLayout->setSpacing(5);

    tagInput = new QLineEdit(this);
    tagInput->setObjectName("seamlessTag");
    tagInput->setPlaceholderText(LanguageManager::tr("task.form.tag") + "...");
    tagInput->setMaximumWidth(150);

    QLabel* separatorLabel = new QLabel(LanguageManager::tr("task.group_separator") + " ", this);
    separatorLabel->setStyleSheet("color: darkGray;");

    groupCombo = new QComboBox(this);
    groupCombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    subtitleLayout->addWidget(tagInput);
    subtitleLayout->addWidget(separatorLabel);
    subtitleLayout->addWidget(groupCombo);
    textLayout->addLayout(subtitleLayout);

    QHBoxLayout* assigneeLayout = new QHBoxLayout();
    assigneeLayout->setSpacing(5);

    QLabel* assigneeLabel = new QLabel(LanguageManager::tr("task.assigned_to_prefix") + " ", this);
    assigneeLabel->setStyleSheet("color: darkGray;");

    assigneeCombo = new QComboBox(this);
    assigneeCombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    assigneeLayout->addWidget(assigneeLabel);
    assigneeLayout->addWidget(assigneeCombo);
    textLayout->addLayout(assigneeLayout);

    cardLayout->addLayout(textLayout);
    mainLayout->addWidget(cardFrame);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    btnCancel = new QPushButton(LanguageManager::tr("action.cancel"), this);
    btnCreate = new QPushButton(LanguageManager::tr("action.create"), this);

    btnCreate->setDefault(true);
    btnCreate->setCursor(Qt::PointingHandCursor);
    btnCancel->setCursor(Qt::PointingHandCursor);

    buttonLayout->addWidget(btnCancel);
    buttonLayout->addWidget(btnCreate);
    mainLayout->addLayout(buttonLayout);

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