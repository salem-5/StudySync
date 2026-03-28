#pragma once
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() = default;
private:
    QWidget* centralWidget;
    QHBoxLayout* mainLayout;

    QWidget* sidebar;
    QPushButton* btnDashboard;
    QPushButton* btnFocus;
    QPushButton* btnGroups;
    QPushButton* btnAiTutor;
    QPushButton* btnLogout;

    void setupUi();
    void setupSidebar();
};