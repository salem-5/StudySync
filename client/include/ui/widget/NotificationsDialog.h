#pragma once

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFrame>

class NotificationsDialog : public QDialog {
public:
    explicit NotificationsDialog(QWidget* parent = nullptr);

    void refresh();

private:
    QVBoxLayout* listLayout;
};