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
    setWindowTitle(LanguageManager::tr("nav.notifications.title"));
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
        listLayout->addWidget(new QLabel(LanguageManager::tr("notifications.empty")));
        return;
    }

    for (const StudyGroup* group : invites) {
        QFrame* box = new QFrame();

        box->setAttribute(Qt::WA_StyledBackground, true);
        box->setProperty("cssClass", "card");
        box->setFrameShape(QFrame::StyledPanel);

        QVBoxLayout* boxLayout = new QVBoxLayout(box);
        boxLayout->setContentsMargins(15, 12, 15, 12);
        boxLayout->setSpacing(15);

        boxLayout->addWidget(
            new QLabel(LanguageManager::tr("notifications.invite_msg").arg(QString::fromStdString(group->getName())))
        );

        QHBoxLayout* btnLayout = new QHBoxLayout();

        QPushButton* btnAccept = new QPushButton(LanguageManager::tr("action.accept"));
        QPushButton* btnDeny = new QPushButton(LanguageManager::tr("action.deny"));

        connect(btnAccept, &QPushButton::clicked, this,
            [this, gId = group->getId(), uId = user->getId()]() {
                ClientState::acceptInvite(gId, uId);
                refresh();
            }
        );

        connect(btnDeny, &QPushButton::clicked, this,
            [this, gId = group->getId(), uId = user->getId()]() {
                ClientState::denyInvite(gId, uId);
                refresh();
            }
        );

        btnLayout->addWidget(btnAccept);
        btnLayout->addWidget(btnDeny);

        boxLayout->addLayout(btnLayout);
        listLayout->addWidget(box);
    }
}