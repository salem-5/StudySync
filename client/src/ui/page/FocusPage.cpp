#include "ui/page/FocusPage.h"
#include "ui/ClientState.h"
#include "ui/widget/TaskCard.h"
#include "ui/widget/GroupCard.h"
#include <iostream>
#include <QApplication>
#include <QFile>
#include "LanguageManager.h"

static constexpr int DefaultFocusDurationSeconds = 25 * 60;

FocusPage::FocusPage(QWidget* parent) : QWidget(parent), timer(new QTimer(this)), remainingSeconds(DefaultFocusDurationSeconds) {
    QVBoxLayout* layout = new QVBoxLayout(this);

    QGroupBox* group = new QGroupBox(LanguageManager::tr("focus.timer_title"));
    QVBoxLayout* groupLayout = new QVBoxLayout(group);
    groupLayout->setAlignment(Qt::AlignCenter);

    groupSelect = new QComboBox();
    groupSelect->addItem(LanguageManager::tr("focus.group_placeholder"));
    refreshGroupList();

    timerLabel = new QLabel();
    QFont timerFont = timerLabel->font();
    timerFont.setPointSize(48);
    timerLabel->setFont(timerFont);
    timerLabel->setAlignment(Qt::AlignCenter);
    refreshTimerLabel();

    QHBoxLayout* btnLayout = new QHBoxLayout();
    startButton = new QPushButton(LanguageManager::tr("focus.start"));
    pauseButton = new QPushButton(LanguageManager::tr("focus.pause"));
    resetButton = new QPushButton(LanguageManager::tr("focus.reset"));

    pauseButton->setEnabled(false);

    btnLayout->addWidget(startButton);
    btnLayout->addWidget(pauseButton);
    btnLayout->addWidget(resetButton);

    groupLayout->addWidget(groupSelect);
    groupLayout->addWidget(timerLabel);
    groupLayout->addLayout(btnLayout);
    layout->addWidget(group);

    connect(timer, &QTimer::timeout, this, &FocusPage::updateTimer);
    connect(startButton, &QPushButton::clicked, this, &FocusPage::startTimer);
    connect(pauseButton, &QPushButton::clicked, this, &FocusPage::pauseTimer);
    connect(resetButton, &QPushButton::clicked, this, &FocusPage::resetTimer);
    connect(groupSelect, &QComboBox::currentIndexChanged, this, &FocusPage::updateButtons);
}

void FocusPage::refreshGroupList() {
    groupSelect->clear();
    groupSelect->addItem(LanguageManager::tr("focus.group_placeholder"));
    const auto& studyGroups = ClientState::getStudyGroups();
    for (const StudyGroup& group : studyGroups) {
        groupSelect->addItem(QString::fromStdString(group.getName()), QVariant(group.getId()));
    }
}


void FocusPage::refreshTimerLabel() {
    int minutes = remainingSeconds / 60;
    int seconds = remainingSeconds % 60;
    timerLabel->setText(QString("%1:%2").arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0')));
}

void FocusPage::updateTimer() {
    if (remainingSeconds <= 0) {
        timer->stop();
        updateButtons();
        pauseButton->setEnabled(false);
        return;
    }

    remainingSeconds -= 1;
    refreshTimerLabel();

    if (remainingSeconds <= 0) {
        timer->stop();
        startButton->setEnabled(true);
        pauseButton->setEnabled(false);
    }
}

void FocusPage::updateButtons() {
    bool isGroupSelected = !groupSelect->currentIndex();
    startButton->setDisabled(isGroupSelected);
    resetButton->setDisabled(isGroupSelected);
}

void FocusPage::startTimer() {
    if (groupSelect->currentIndex() == 0) return;
    if (!timer->isActive()) {
        if (remainingSeconds == DefaultFocusDurationSeconds) {
            int groupId = groupSelect->currentData().toInt();
            int minutes = DefaultFocusDurationSeconds / 60;

            QString msg = LanguageManager::tr("focus.announce_msg").arg(minutes);
            ClientState::sendMessage(groupId, msg.toStdString());
        }

        timer->start(1000);
        startButton->setEnabled(false);
        pauseButton->setEnabled(true);
    }
}

void FocusPage::pauseTimer() {
    if (timer->isActive()) {
        timer->stop();
        startButton->setEnabled(true);
        pauseButton->setEnabled(false);
    }
}

void FocusPage::resetTimer() {
    timer->stop();
    remainingSeconds = DefaultFocusDurationSeconds;
    refreshTimerLabel();
    updateButtons();
    pauseButton->setEnabled(false);
}

void FocusPage::startFocusTimer() {
    startTimer();
}

void FocusPage::resetFocusTimer() {
    resetTimer();
}
