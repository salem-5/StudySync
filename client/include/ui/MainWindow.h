#pragma once
#include "DataStructures.h"
#include "page/GroupsPage.h"

#include "ui/page/DashboardPage.h"
#include "ui/page/TasksPage.h"
#include "ui/page/FocusPage.h"
#include "ui/page/GroupsPage.h"
#include "ui/page/GroupChatPage.h"
#include "ui/page/AiTutorPage.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    static void loadStylesheet(QApplication &app);
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() = default;
private:
    QWidget* centralWidget;
    QHBoxLayout* mainLayout;

    QWidget* sidebar;
    QPushButton* btnDashboard;
    QPushButton* createTaskBtn;
    QPushButton* btnFocus;
    QPushButton* btnGroups;
    QPushButton* btnAiTutor;
    QPushButton* btnLogout;

    QWidget* topbar;
    QLabel* topbarTitle;

    QStackedWidget* stackedWidget;
    DashboardPage* pageDashboard;
    TasksPage* pageTasks;
    FocusPage* pageFocus;
    GroupsPage* pageGroups;
    GroupChatPage* pageGroupChat;
    AiTutorPage* pageAiTutor;

    void setupUi();
    void setupSidebar();
    void setupTopbar();
    void connectSignals();

private slots:
    void switchPage();
    void openGroupChat(int groupId);
    void openGroupTasks(int groupId);
    void startFocusFromDashboard();
    void navigateBackToGroups();
    void openCreateTaskDialog();
    void handleNewTask(Task task);

signals:
    void tasksChanged();
};