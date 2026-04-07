#include "ui/widget/GroupCard.h"
#include <QPushButton>
#include <QMessageBox>
#include <QHBoxLayout>
#include "ui/ClientState.h"
#include "LanguageManager.h"
#include "ui/widget/NotificationsDialog.h"

#include <QScrollArea>

class QScrollArea;

NotificationsDialog::NotificationsDialog(QWidget* parent) : QDialog(parent) {
    setWindowTitle("Notifications");
    setMinimumSize(350, 400);

    QVBoxLayout* layout = new QVBoxLayout(this);
    listLayout = new QVBoxLayout();
    listLayout->setAlignment(Qt::AlignTop);

    QScrollArea* scroll = new QScrollArea();
    QWidget* container = new QWidget();
    container->setLayout(listLayout);

    scroll->setWidget(container);
    scroll->setWidgetResizable(true);
    layout->addWidget(scroll);

    refresh();
}

void NotificationsDialog::refresh()
{
    QLayoutItem* item;
    while ((item = listLayout->takeAt(0)) != nullptr) {
        if (item->widget()) delete item->widget();
        delete item;
    }

    const User* user = ClientState::getUser();
    if (!user) return;

    auto invites = ClientState::getPendingInvites(user->getId());

    if (invites.empty()) {
        listLayout->addWidget(new QLabel("No new notifications."));
        return;
    }

    for (const StudyGroup* group : invites) {
        QFrame* box = new QFrame();


        QVBoxLayout* boxLayout = new QVBoxLayout(box);

        boxLayout->addWidget(
            new QLabel(QString("You have been invited to join <b>%1</b>")
            .arg(QString::fromStdString(group->getName())))
        );

        QHBoxLayout* btnLayout = new QHBoxLayout();

        QPushButton* btnAccept = new QPushButton("Accept");

        QPushButton* btnDeny = new QPushButton("Deny");

        connect(btnAccept, &QPushButton::clicked, this,
            [this, gId = group->getId(), uId = user->getId()]() {
                ClientState::mockAcceptInvite(gId, uId);
                refresh();
            }
        );

        connect(btnDeny, &QPushButton::clicked, this,
            [this, gId = group->getId(), uId = user->getId()]() {
                ClientState::mockDenyInvite(gId, uId);
                refresh();
            }
        );

        btnLayout->addWidget(btnAccept);
        btnLayout->addWidget(btnDeny);

        boxLayout->addLayout(btnLayout);
        listLayout->addWidget(box);
    }
}