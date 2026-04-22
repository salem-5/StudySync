#include "ui/widget/CreateGroupDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFrame>
#include "LanguageManager.h"
#include "ui/ClientState.h"

CreateGroupDialog::CreateGroupDialog(QWidget* parent) : QDialog(parent) {
    setWindowTitle(LanguageManager::tr("group.create"));
    resize(400, 180);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    QFrame* cardFrame = new QFrame(this);
    cardFrame->setProperty("cssClass", "card");

    QVBoxLayout* cardLayout = new QVBoxLayout(cardFrame);
    cardLayout->setContentsMargins(15, 15, 15, 15);
    cardLayout->setSpacing(10);

    QHBoxLayout* headerLayout = new QHBoxLayout();
    headerLayout->setContentsMargins(0, 0, 0, 0);

    nameInput = new QLineEdit(this);
    nameInput->setObjectName("seamlessGroupName");
    nameInput->setPlaceholderText(LanguageManager::tr("group.form.name") + "...");


    QFont titleFont = nameInput->font();
    titleFont.setBold(true);
    titleFont.setPointSize(12);
    nameInput->setFont(titleFont);

    headerLayout->addWidget(nameInput);
    cardLayout->addLayout(headerLayout);

    QLabel* membersPlaceholder = new QLabel(LanguageManager::tr("group.members_count").arg(1), this);
    membersPlaceholder->setStyleSheet("color: darkGray;");
    cardLayout->addWidget(membersPlaceholder);

    mainLayout->addWidget(cardFrame);

    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnCreate = new QPushButton(LanguageManager::tr("action.create"), this);
    btnCancel = new QPushButton(LanguageManager::tr("action.cancel"), this);

    btnCreate->setDefault(true);
    btnCreate->setCursor(Qt::PointingHandCursor);
    btnCancel->setCursor(Qt::PointingHandCursor);

    btnLayout->addStretch();
    btnLayout->addWidget(btnCancel);
    btnLayout->addWidget(btnCreate);

    mainLayout->addLayout(btnLayout);

    connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);
    connect(btnCreate, &QPushButton::clicked, this, [this]() {
        if (!nameInput->text().trimmed().isEmpty()) {
            ClientState::createGroup(nameInput->text().toStdString());
            emit groupCreated();
            accept();
        }
    });
}