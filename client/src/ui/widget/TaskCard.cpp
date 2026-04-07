#include "ui/widget/TaskCard.h"

#include <QApplication>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include "ui/ClientState.h"
#include "LanguageManager.h"

TaskCard::TaskCard(const Task& task, const QString& groupName, QWidget* parent, bool hasEditButton) : QFrame(parent) {
    setAttribute(Qt::WA_StyledBackground, true);
    setProperty("cssClass", "card");

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(15, 12, 15, 12);
    layout->setSpacing(15);

    QCheckBox* checkbox = new QCheckBox();
    checkbox->setChecked(task.getIsCompleted());

    QVBoxLayout* textLayout = new QVBoxLayout();
    textLayout->setContentsMargins(0, 0, 0, 0);
    textLayout->setSpacing(2);

    QLabel* title = new QLabel(QString::fromStdString(task.getTitle()));
    QFont titleFont = title->font();
    titleFont.setPointSize(11);

    auto updateTitleStyle = [title, titleFont](bool isCompleted) {
        QFont f = titleFont;
        f.setStrikeOut(isCompleted);
        f.setBold(!isCompleted);
        title->setFont(f);
        QPalette p = title->palette();
        p.setColor(QPalette::WindowText, isCompleted ? Qt::gray : QApplication::palette().color(QPalette::WindowText));
        title->setPalette(p);
    };

    updateTitleStyle(task.getIsCompleted());
    connect(checkbox, &QCheckBox::toggled, this, [this, id = task.getId(), updateTitleStyle](bool checked) {
        ClientState::mockToggleTaskCompletion(id, checked);
        updateTitleStyle(checked);
        emit taskStateChanged();
    });

    int assigneeId = task.getAssignedToId();
    QString assigneeName = QString::fromStdString(ClientState::getUsername(assigneeId));
    QLabel* subtitle = new QLabel(QString::fromStdString(task.getTag()) + "  " + LanguageManager::tr("task.group_separator") + " " + groupName);
    QLabel* assigneeLabel = new QLabel(LanguageManager::tr("task.assigned_to_prefix") + " " + assigneeName);

    QFont subFont = subtitle->font();
    subFont.setPointSize(9);
    subtitle->setFont(subFont);
    assigneeLabel->setFont(subFont);

    QPalette subPalette = subtitle->palette();
    subPalette.setColor(QPalette::WindowText, Qt::darkGray);
    subtitle->setPalette(subPalette);
    assigneeLabel->setPalette(subPalette);

    textLayout->addWidget(title);
    textLayout->addWidget(subtitle);
    textLayout->addWidget(assigneeLabel);

    layout->addWidget(checkbox);
    layout->addLayout(textLayout);
    layout->addStretch();
    if (hasEditButton) {
        QPushButton* btnEdit = new QPushButton(LanguageManager::tr("task.edit"));
        btnEdit->setCursor(Qt::PointingHandCursor);
        connect(btnEdit, &QPushButton::clicked, this, [this, id = task.getId()]() {
            emit editRequested(id);
        });
        layout->addWidget(btnEdit);
    }
}
