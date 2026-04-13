#pragma once
#include <QDialog>
#include <QTabWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QGroupBox>
#include <QCheckBox>
#include <QSettings>
#include <memory>
#include "ServerAPI.h"

class LoginWindow : public QDialog {
    Q_OBJECT

public:
    explicit LoginWindow(std::shared_ptr<ServerAPI> api, QWidget *parent = nullptr);

private slots:
    void handleLogin();
    void handleRegistration();

private:
    void setupUi();

    std::shared_ptr<ServerAPI> serverApi;

    QTabWidget* tabWidget;
    QLabel* statusLabel;

    QLineEdit* loginUsernameInput;
    QLineEdit* loginPasswordInput;
    QCheckBox* rememberCheck;
    QPushButton* loginBtn;

    QLineEdit* regUsernameInput;
    QLineEdit* regEmailInput;
    QLineEdit* regPasswordInput;
    QPushButton* regBtn;

    QGroupBox* advancedGroup;
    QLineEdit* ipInput;
    QLineEdit* portInput;
};