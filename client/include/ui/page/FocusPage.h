#pragma once

#include <QMainWindow>
#include <QStackedWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QGroupBox>
#include <QFrame>
#include <QTimer>
#include "DataStructures.h"


class FocusPage : public QWidget {
    Q_OBJECT
public:
    FocusPage(QWidget* parent = nullptr);

private slots:
    void updateTimer();
    void startTimer();
    void pauseTimer();
    void resetTimer();

private:
    void refreshTimerLabel();

    QTimer* timer;
    QLabel* timerLabel;
    QPushButton* startButton;
    QPushButton* pauseButton;
    QPushButton* resetButton;
    int remainingSeconds;
};
