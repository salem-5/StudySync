#include "ui/MainWindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUi();
}

void MainWindow::setupUi() {
    this->setWindowTitle("StudySync");
    this->resize(1000, 700);

    centralWidget = new QWidget(this);
    this->setCentralWidget(centralWidget);
    mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(10);

    setupSidebar();

    mainLayout->addWidget(sidebar);
    mainLayout->addStretch(); // placeholder for right window side
}

void MainWindow::setupSidebar() {
    sidebar = new QWidget(this);
    sidebar->setFixedWidth(200);

    QVBoxLayout* layout = new QVBoxLayout(sidebar);
    layout->setContentsMargins(0, 0, 0, 0);

    QLabel* logoLabel = new QLabel("📚StudySync", this);
    QFont logoFont = logoLabel->font();
    logoFont.setBold(true);
    logoFont.setPointSize(23);
    logoLabel->setFont(logoFont);

    layout->addWidget(logoLabel);
    layout->addSpacing(5);

    btnDashboard = new QPushButton("Dashboard", this);
    btnFocus = new QPushButton("Focus Session", this);
    btnGroups = new QPushButton("Study Groups", this);
    btnAiTutor = new QPushButton("AI Tutor", this);

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

    btnLogout = new QPushButton("Logout", this);
    layout->addWidget(btnLogout);
}