#include "ui/MainWindow.h"

#include <iostream>
#include <QApplication>
#include <QFile>
#include <QScreen>

#include "ui/ClientState.h"
#include "ui/widget/CreateTaskDialog.h"
#include "LanguageManager.h"
#include "ui/widget/NotificationsDialog.h"
#include "ui/widget/ManageMembersDialog.h"

void MainWindow::loadStylesheet(QApplication &app) {
    QFile styleFile(":/resources/themeStyle.css");
    if (styleFile.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(styleFile.readAll());
        app.setStyleSheet(styleSheet);
        styleFile.close();
    } else {
        std::cout << "couldnt load stylesheet" << std::endl;
    }
}
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUi();
    connectSignals();
}

void MainWindow::refreshAll() {
    emit tasksChanged();
    emit groupsChanged();

    for (auto* notifDialog : this->findChildren<NotificationsDialog*>()) {
        notifDialog->refresh();
    }
    for (auto* membersDialog : this->findChildren<ManageMembersDialog*>()) {
        membersDialog->refreshList();
    }
}

void MainWindow::setupUi() {
    this->setWindowTitle(LanguageManager::tr("app.name"));
    this->resize(1000, 700);

    centralWidget = new QWidget(this);
    this->setCentralWidget(centralWidget);
    mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(10);

    setupSidebar();

    QWidget* rightSideContainer = new QWidget(this);
    QVBoxLayout* rightSideLayout = new QVBoxLayout(rightSideContainer);
    rightSideLayout->setContentsMargins(0, 0, 0, 0);

    setupTopbar();

    stackedWidget = new QStackedWidget(this);

    pageDashboard = new DashboardPage(this);
    pageFocus = new FocusPage(this);
    pageGroups = new GroupsPage(this);
    pageAiTutor = new AiTutorPage(this);
    pageGroupChat = new GroupChatPage(this);
    pageTasks = new TasksPage(this);

    stackedWidget->addWidget(pageDashboard);
    stackedWidget->addWidget(pageFocus);
    stackedWidget->addWidget(pageGroups);
    stackedWidget->addWidget(pageAiTutor);
    stackedWidget->addWidget(pageGroupChat);
    stackedWidget->addWidget(pageTasks);

    rightSideLayout->addWidget(topbar);
    rightSideLayout->addWidget(stackedWidget);

    mainLayout->addWidget(sidebar);
    mainLayout->addWidget(rightSideContainer);
}

void MainWindow::setupSidebar() {
    sidebar = new QWidget(this);
    sidebar->setFixedWidth(200);

    QVBoxLayout* layout = new QVBoxLayout(sidebar);
    layout->setContentsMargins(0, 0, 0, 0);

    QLabel* logoLabel = new QLabel(this);
    QPixmap logoPix(":/resources/studysync_logo.png");

    logoLabel->setPixmap(logoPix.scaled(180, 65, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    logoLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    layout->addWidget(logoLabel);
    layout->addSpacing(5);

    btnDashboard = new QPushButton(LanguageManager::tr("nav.dashboard"), this);
    btnFocus = new QPushButton(LanguageManager::tr("nav.focus_session"), this);
    btnGroups = new QPushButton(LanguageManager::tr("nav.study_groups"), this);
    btnAiTutor = new QPushButton(LanguageManager::tr("nav.ai_tutor"), this);

    btnDashboard->setCheckable(true);
    btnFocus->setCheckable(true);
    btnGroups->setCheckable(true);
    btnAiTutor->setCheckable(true);
    btnDashboard->setChecked(true);

    layout->addWidget(btnDashboard);
    layout->addWidget(btnFocus);
    layout->addWidget(btnGroups);
    layout->addWidget(btnAiTutor);
    layout->addStretch();

    btnLogout = new QPushButton(LanguageManager::tr("nav.logout"), this);
    layout->addWidget(btnLogout);
}

void MainWindow::setupTopbar() {
    topbar = new QWidget(this);
    topbar->setFixedHeight(50);

    QHBoxLayout* layout = new QHBoxLayout(topbar);
    layout->setContentsMargins(0, 0, 0, 0);

    topbarTitle = new QLabel(LanguageManager::tr("nav.dashboard"), this);
    QFont titleFont = topbarTitle->font();
    titleFont.setBold(true);
    titleFont.setPointSize(12);
    topbarTitle->setFont(titleFont);

    layout->addWidget(topbarTitle);
    layout->addStretch();
    createTaskBtn = new QPushButton(LanguageManager::tr("task.create"), this);
    layout->addWidget(createTaskBtn);
    QPushButton* btnNotifications = new QPushButton(LanguageManager::tr("nav.notifications"), this);
    layout->addWidget(btnNotifications);
    connect(btnNotifications, &QPushButton::clicked, this, [this]() {
        NotificationsDialog* dialog = new NotificationsDialog(this);
        dialog->exec();
        pageGroups->loadGroups();
    });
    layout->addWidget(new QLabel(ClientState::getUser()->getUsername().c_str(), this));
}

void MainWindow::connectSignals() {
    connect(createTaskBtn, &QPushButton::clicked, this, &MainWindow::openCreateTaskDialog);
    connect(btnDashboard, &QPushButton::clicked, this, &MainWindow::switchPage);
    connect(btnFocus, &QPushButton::clicked, this, &MainWindow::switchPage);
    connect(btnGroups, &QPushButton::clicked, this, &MainWindow::switchPage);
    connect(btnAiTutor, &QPushButton::clicked, this, &MainWindow::switchPage);
    connect(pageDashboard, &DashboardPage::startFocusRequested, this, &MainWindow::startFocusFromDashboard);
    connect(pageDashboard, &DashboardPage::openGroupChatRequested, this, &MainWindow::openGroupChat);
    connect(pageDashboard, &DashboardPage::openGroupTasksRequested, this, &MainWindow::openGroupTasks);
    connect(pageGroups, &GroupsPage::openGroupTasksRequested, this, &MainWindow::openGroupTasks);
    connect(pageGroups, &GroupsPage::openGroupChatRequested, this, &MainWindow::openGroupChat);
    connect(pageGroupChat, &GroupChatPage::backToGroupsRequested, this, &MainWindow::navigateBackToGroups);
    connect(pageTasks, &TasksPage::backToGroupsRequested, this, &MainWindow::navigateBackToGroups);
    connect(this, &MainWindow::tasksChanged, pageDashboard, &DashboardPage::refreshTaskCards);
    connect(pageTasks, &TasksPage::tasksChanged, pageDashboard, &DashboardPage::refreshTaskCards);
    connect(pageGroups, &GroupsPage::groupsChanged, this, [this]() {
        pageDashboard->refreshTaskCards();
        pageDashboard->refreshPinnedGroups();
    });
    connect(this, &MainWindow::groupsChanged, pageGroups, &GroupsPage::loadGroups);
    connect(this, &MainWindow::groupsChanged, pageDashboard, &DashboardPage::refreshPinnedGroups);
    connect(this, &MainWindow::groupsChanged, pageFocus, &FocusPage::refreshGroupList);
    connect(pageGroups, &GroupsPage::groupsChanged, this, [this]() {
        emit groupsChanged();
        emit tasksChanged();
    });
}

void MainWindow::switchPage() {
    QPushButton* clickedBtn = qobject_cast<QPushButton*>(sender());
    if (!clickedBtn) return;

    btnDashboard->setChecked(false);
    btnFocus->setChecked(false);
    btnGroups->setChecked(false);
    btnAiTutor->setChecked(false);

    clickedBtn->setChecked(true);
    pageDashboard->refreshPinnedGroups();

    if (clickedBtn == btnDashboard) {
        stackedWidget->setCurrentIndex(0);
        topbarTitle->setText(LanguageManager::tr("nav.dashboard"));
    } else if (clickedBtn == btnFocus) {
        pageFocus->refreshGroupList();
        stackedWidget->setCurrentIndex(1);
        topbarTitle->setText(LanguageManager::tr("nav.focus_session"));
    } else if (clickedBtn == btnGroups) {
        stackedWidget->setCurrentIndex(2);
        topbarTitle->setText(LanguageManager::tr("nav.study_groups"));
    } else if (clickedBtn == btnAiTutor) {
        stackedWidget->setCurrentIndex(3);
        topbarTitle->setText(LanguageManager::tr("nav.ai_tutor"));
    }
}

void MainWindow::openGroupChat(int groupId) {
    stackedWidget->setCurrentIndex(4);
    topbarTitle->setText(LanguageManager::tr("nav.group_chat"));

    btnDashboard->setChecked(false);
    btnFocus->setChecked(false);
    btnAiTutor->setChecked(false);
    btnGroups->setChecked(true);

    pageGroupChat->loadChat(groupId);
}

void MainWindow::openGroupTasks(int groupId) {
    stackedWidget->setCurrentIndex(5);
    topbarTitle->setText(LanguageManager::tr("nav.group_tasks"));

    btnDashboard->setChecked(false);
    btnFocus->setChecked(false);
    btnGroups->setChecked(true);
    btnAiTutor->setChecked(false);

    pageTasks->loadTasks(groupId);
}

void MainWindow::startFocusFromDashboard() {
    btnDashboard->setChecked(false);
    btnFocus->setChecked(true);
    pageFocus->refreshGroupList();
    stackedWidget->setCurrentIndex(1);
    topbarTitle->setText(LanguageManager::tr("nav.focus_session"));
    pageFocus->resetFocusTimer();
    pageFocus->startFocusTimer();
}

void MainWindow::navigateBackToGroups() {
    btnDashboard->setChecked(false);
    btnFocus->setChecked(false);
    btnGroups->setChecked(true);
    btnAiTutor->setChecked(false);
    stackedWidget->setCurrentIndex(2);
    topbarTitle->setText(LanguageManager::tr("nav.study_groups"));
    pageDashboard->refreshPinnedGroups();
}


void MainWindow::openCreateTaskDialog() {
    CreateTaskDialog* dialog = new CreateTaskDialog(this);

    connect(dialog, &CreateTaskDialog::taskCreated,this, [this](const Task& task) {
        handleNewTask(task);
        emit tasksChanged();
        pageTasks->loadTasks(task.getGroupId());
    });

    dialog->exec();
}

void MainWindow::handleNewTask(Task task) {
    ClientState::createTask(task.getGroupId(), task.getTitle(), task.getTag(), task.getAssignedToId());
}