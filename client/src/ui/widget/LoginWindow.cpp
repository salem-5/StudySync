#include "ui/widget/LoginWindow.h"
#include "LanguageManager.h"
#include <QMetaObject>

LoginWindow::LoginWindow(std::shared_ptr<ServerAPI> api, QWidget *parent)
    : QDialog(parent), serverApi(api) {
    setupUi();
}

void LoginWindow::setupUi() {
    this->setWindowTitle(LanguageManager::tr("app.auth_title"));
    this->setFixedSize(400, 520);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    mainLayout->setSpacing(15);


    QLabel* logoLabel = new QLabel(this);
    QPixmap logoPix(":/resources/studysync_logo.png");

    logoLabel->setPixmap(logoPix.scaled(180, 65, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    logoLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(logoLabel);

    tabWidget = new QTabWidget(this);

    QWidget* loginTab = new QWidget(this);
    QVBoxLayout* loginLayout = new QVBoxLayout(loginTab);
    loginLayout->setSpacing(15);

    loginUsernameInput = new QLineEdit(this);
    loginUsernameInput->setPlaceholderText(LanguageManager::tr("auth.placeholder.username"));

    loginPasswordInput = new QLineEdit(this);
    loginPasswordInput->setEchoMode(QLineEdit::Password);
    loginPasswordInput->setPlaceholderText(LanguageManager::tr("auth.placeholder.password"));

    rememberCheck = new QCheckBox(LanguageManager::tr("auth.remember_me"), this);
    loginBtn = new QPushButton(LanguageManager::tr("auth.login_btn"), this);
    loginBtn->setCursor(Qt::PointingHandCursor);

    loginLayout->addWidget(loginUsernameInput);
    loginLayout->addWidget(loginPasswordInput);
    loginLayout->addWidget(rememberCheck);
    loginLayout->addWidget(loginBtn);
    loginLayout->addStretch();

    QWidget* registerTab = new QWidget(this);
    QVBoxLayout* regLayout = new QVBoxLayout(registerTab);
    regLayout->setSpacing(15);

    regUsernameInput = new QLineEdit(this);
    regUsernameInput->setPlaceholderText(LanguageManager::tr("auth.placeholder.username"));
    regEmailInput = new QLineEdit(this);
    regEmailInput->setPlaceholderText(LanguageManager::tr("auth.placeholder.email"));
    regPasswordInput = new QLineEdit(this);
    regPasswordInput->setEchoMode(QLineEdit::Password);
    regPasswordInput->setPlaceholderText(LanguageManager::tr("auth.placeholder.password"));

    regBtn = new QPushButton(LanguageManager::tr("auth.register_btn"), this);
    regBtn->setCursor(Qt::PointingHandCursor);

    regLayout->addWidget(regUsernameInput);
    regLayout->addWidget(regEmailInput);
    regLayout->addWidget(regPasswordInput);
    regLayout->addWidget(regBtn);
    regLayout->addStretch();

    tabWidget->addTab(loginTab, LanguageManager::tr("auth.tab_login"));
    tabWidget->addTab(registerTab, LanguageManager::tr("auth.tab_register"));
    mainLayout->addWidget(tabWidget);

    advancedGroup = new QGroupBox(LanguageManager::tr("auth.advanced_options"), this);
    advancedGroup->setCheckable(true);
    advancedGroup->setChecked(false);
    QFormLayout* advLayout = new QFormLayout(advancedGroup);

    ipInput = new QLineEdit(this);
    portInput = new QLineEdit(this);

    advLayout->addRow(LanguageManager::tr("auth.ip_address"), ipInput);
    advLayout->addRow(LanguageManager::tr("auth.port"), portInput);
    mainLayout->addWidget(advancedGroup);

    statusLabel = new QLabel(this);
    statusLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(statusLabel);

    QSettings settings("StudySync", "ClientApp"); // save settings locally
    loginUsernameInput->setText(settings.value("username", "").toString());
    loginPasswordInput->setText(settings.value("password", "").toString());
    rememberCheck->setChecked(settings.value("remember", false).toBool());
    ipInput->setText(settings.value("ip", "127.0.0.1").toString());
    portInput->setText(settings.value("port", "8080").toString());

    connect(loginBtn, &QPushButton::clicked, this, &LoginWindow::handleLogin);
    connect(regBtn, &QPushButton::clicked, this, &LoginWindow::handleRegistration);
}

void LoginWindow::handleLogin() {
    serverApi->setServerAddress(ipInput->text().toStdString(), portInput->text().toStdString());

    QString username = loginUsernameInput->text();
    QString password = loginPasswordInput->text();

    loginBtn->setEnabled(false);
    statusLabel->setStyleSheet("color: #4d90fe;");
    statusLabel->setText(LanguageManager::tr("auth.status.connecting"));

    serverApi->login(username.toStdString(), password.toStdString(),
        [this, username, password](bool success, const std::string& msg) {
            QMetaObject::invokeMethod(this, [this, success, msg, username, password]() {
                loginBtn->setEnabled(true);
                if (success) {
                    QSettings settings("StudySync", "ClientApp");
                    if (rememberCheck->isChecked()) {
                        settings.setValue("username", username);
                        settings.setValue("password", password);
                        settings.setValue("remember", true);
                    } else {
                        settings.remove("username");
                        settings.remove("password");
                        settings.setValue("remember", false);
                    }
                    settings.setValue("ip", ipInput->text());
                    settings.setValue("port", portInput->text());

                    accept();
                } else {
                    statusLabel->setStyleSheet("color: red;");
                    QString errorMsg = msg.empty() ? LanguageManager::tr("auth.error.login_failed") : QString::fromStdString(msg);
                    statusLabel->setText(errorMsg);
                }
            });
        });
}

void LoginWindow::handleRegistration() {
    QString username = regUsernameInput->text();
    QString email = regEmailInput->text();
    QString password = regPasswordInput->text();

    if (username.isEmpty() || email.isEmpty() || password.isEmpty()) {
        statusLabel->setStyleSheet("color: red;");
        statusLabel->setText(LanguageManager::tr("auth.error.missing_fields"));
        return;
    }

    regBtn->setEnabled(false);
    statusLabel->setStyleSheet("color: black;");
    statusLabel->setText(LanguageManager::tr("auth.status.registering"));

    serverApi->createUser(username.toStdString(), email.toStdString(), password.toStdString(),
        [this](bool success) {

            QMetaObject::invokeMethod(this, [this, success]() {
                regBtn->setEnabled(true);
                if (success) {
                    statusLabel->setStyleSheet("color: green;");
                    statusLabel->setText(LanguageManager::tr("auth.status.reg_success"));

                    regPasswordInput->clear();
                    loginUsernameInput->setText(regUsernameInput->text());
                    tabWidget->setCurrentIndex(0);
                } else {
                    statusLabel->setStyleSheet("color: red;");
                    statusLabel->setText(LanguageManager::tr("auth.error.reg_failed"));
                }
            });
        });
}