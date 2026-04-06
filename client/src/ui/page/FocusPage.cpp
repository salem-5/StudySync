#include "ui/page/FocusPage.h"
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

    QComboBox* taskSelect = new QComboBox();
    taskSelect->addItem(LanguageManager::tr("focus.task_placeholder"));

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

    groupLayout->addWidget(taskSelect);
    groupLayout->addWidget(timerLabel);
    groupLayout->addLayout(btnLayout);

    layout->addWidget(group);

    connect(timer, &QTimer::timeout, this, &FocusPage::updateTimer);
    connect(startButton, &QPushButton::clicked, this, &FocusPage::startTimer);
    connect(pauseButton, &QPushButton::clicked, this, &FocusPage::pauseTimer);
    connect(resetButton, &QPushButton::clicked, this, &FocusPage::resetTimer);
}

void FocusPage::refreshTimerLabel() {
    int minutes = remainingSeconds / 60;
    int seconds = remainingSeconds % 60;
    timerLabel->setText(QString("%1:%2").arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0')));
}

void FocusPage::updateTimer() {
    if (remainingSeconds <= 0) {
        timer->stop();
        startButton->setEnabled(true);
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

void FocusPage::startTimer() {
    if (!timer->isActive()) {
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
    startButton->setEnabled(true);
    pauseButton->setEnabled(false);
}
