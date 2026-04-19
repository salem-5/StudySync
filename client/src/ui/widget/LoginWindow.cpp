#include "ui/widget/LoginWindow.h"
#include "LanguageManager.h"
#include <QMetaObject>
#include <QSettings>
#include <QComboBox>
#include <QFrame>
#include <QStyle>
#include <regex>

LoginWindow::LoginWindow(std::shared_ptr<ServerAPI> api, QWidget *parent)
    : QDialog(parent), serverApi(api) {
    setupUi();
}

auto updateStatusStyle = [](QLabel* label, const char* state) {
    label->setProperty("statusState", state);
    label->style()->unpolish(label);
    label->style()->polish(label);
};

void LoginWindow::setupUi() {
    this->setWindowTitle(LanguageManager::tr("app.auth_title"));
    this->setFixedSize(400, 560);

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
    rememberCheck->setObjectName("rememberCheck");

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

    QFrame* serverCard = new QFrame(this);
    serverCard->setObjectName("serverCard");

    QVBoxLayout* cardLayout = new QVBoxLayout(serverCard);
    cardLayout->setContentsMargins(15, 15, 15, 15);
    cardLayout->setSpacing(10);

    serverTypeCombo = new QComboBox(serverCard);
    serverTypeCombo->addItem(LanguageManager::tr("auth.server_public"));
    serverTypeCombo->addItem(LanguageManager::tr("auth.server_custom"));
    cardLayout->addWidget(serverTypeCombo);
    customServerContainer = new QWidget(serverCard);
    QFormLayout* customLayout = new QFormLayout(customServerContainer);
    customLayout->setContentsMargins(0, 0, 0, 0);

    ipInput = new QLineEdit(serverCard);
    portInput = new QLineEdit(serverCard);

    customLayout->addRow(LanguageManager::tr("auth.ip_address"), ipInput);
    customLayout->addRow(LanguageManager::tr("auth.port"), portInput);
    cardLayout->addWidget(customServerContainer);

    mainLayout->addWidget(serverCard);

    statusLabel = new QLabel(this);
    statusLabel->setObjectName("loginStatusLabel");
    statusLabel->setAlignment(Qt::AlignCenter);
    updateStatusStyle(statusLabel, "normal");
    mainLayout->addWidget(statusLabel);

    QSettings settings("StudySync", "ClientApp");
    loginUsernameInput->setText(settings.value("username", "").toString());
    loginPasswordInput->setText(settings.value("password", "").toString());
    rememberCheck->setChecked(settings.value("remember", false).toBool());

    bool useCustomServer = settings.value("use_custom_server", false).toBool();
    serverTypeCombo->setCurrentIndex(useCustomServer ? 1 : 0);
    customServerContainer->setVisible(useCustomServer);

    ipInput->setText(settings.value("ip", "127.0.0.1").toString());
    portInput->setText(settings.value("port", "8080").toString());

    if (!useCustomServer) {
        statusLabel->setText(LanguageManager::tr("auth.status.using_public"));
    } else {
        statusLabel->setText(LanguageManager::tr("auth.status.custom_server"));
    }

    connect(serverTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index) {
        bool isCustom = (index == 1);
        customServerContainer->setVisible(isCustom);

        updateStatusStyle(statusLabel, "normal");

        if (!isCustom) statusLabel->setText(LanguageManager::tr("auth.status.using_public"));
        else statusLabel->setText(LanguageManager::tr("auth.status.custom_server"));
    });


    connect(loginBtn, &QPushButton::clicked, this, &LoginWindow::handleLogin);
    connect(regBtn, &QPushButton::clicked, this, &LoginWindow::handleRegistration);
}

void LoginWindow::handleLogin() {
    bool isCustomServer = (serverTypeCombo->currentIndex() == 1);

    if (!isCustomServer) serverApi->setServerAddress("server.studysync.site", "2452");
    else serverApi->setServerAddress(ipInput->text().toStdString(), portInput->text().toStdString());

    QString username = loginUsernameInput->text();
    QString password = loginPasswordInput->text();

    loginBtn->setEnabled(false);
    updateStatusStyle(statusLabel, "normal");
    statusLabel->setText(LanguageManager::tr("auth.status.connecting"));

    serverApi->login(username.toStdString(), password.toStdString(),
        [this, username, password, isCustomServer](bool success, const std::string& msg) {
            QMetaObject::invokeMethod(this, [this, success, msg, username, password, isCustomServer]() {
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

                    settings.setValue("use_custom_server", isCustomServer);
                    if (isCustomServer) {
                        settings.setValue("ip", ipInput->text());
                        settings.setValue("port", portInput->text());
                    }

                    accept();
                } else {
                    updateStatusStyle(statusLabel, "error");
                    QString errorMsg = msg.empty() ? LanguageManager::tr("auth.error.login_failed") : QString::fromStdString(msg);
                    statusLabel->setText(errorMsg);
                }
            });
        });
}

void LoginWindow::handleRegistration() {
    bool isCustomServer = (serverTypeCombo->currentIndex() == 1);
    if (!isCustomServer) {
        serverApi->setServerAddress("server.studysync.site", "2452");
    } else {
        serverApi->setServerAddress(ipInput->text().toStdString(), portInput->text().toStdString());
    }

    QString username = regUsernameInput->text();
    QString email = regEmailInput->text();
    QString password = regPasswordInput->text();

    if (username.isEmpty() || email.isEmpty() || password.isEmpty()) {
        updateStatusStyle(statusLabel, "error");
        statusLabel->setText(LanguageManager::tr("auth.error.missing_fields"));
        return;
    }

    std::regex emailRegex(R"(.+@.+\..+)");
    if (!std::regex_match(email.toStdString(), emailRegex)) {
        updateStatusStyle(statusLabel, "error");
        statusLabel->setText(LanguageManager::tr("auth.error.bad_email"));
        return;
    }

    regBtn->setEnabled(false);
    updateStatusStyle(statusLabel, "normal");
    statusLabel->setText(LanguageManager::tr("auth.status.registering"));

    serverApi->createUser(username.toStdString(), email.toStdString(), password.toStdString(),
        [this](bool success) {

            QMetaObject::invokeMethod(this, [this, success]() {
                regBtn->setEnabled(true);
                if (success) {
                    updateStatusStyle(statusLabel, "success");
                    statusLabel->setText(LanguageManager::tr("auth.status.reg_success"));

                    regPasswordInput->clear();
                    loginUsernameInput->setText(regUsernameInput->text());
                    tabWidget->setCurrentIndex(0);
                } else {
                    updateStatusStyle(statusLabel, "error");
                    statusLabel->setText(LanguageManager::tr("auth.error.reg_failed"));
                }
            });
        });
}