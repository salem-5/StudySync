#include <QApplication>
#include <QSettings>
#include <QString>
#include "ui/MainWindow.h"
#include "ClientNetworkManager.h"
#include "LanguageManager.h"
#include "ui/ClientState.h"
#include "ui/widget/LoginWindow.h"
#include <QStyleHints>
#include <QGuiApplication>

int main(int argc, char* argv[]) {
    QSettings settings("StudySync", "ClientApp");
    QString currentLang = settings.value("language", "en_us").toString();
    QString langPath = QString(":/resources/lang/%1.json").arg(currentLang);
    LanguageManager::loadFallback(":/resources/lang/en_us.json");
    LanguageManager::loadLanguage(langPath);

    QApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/icon.png"));
    MainWindow::loadStylesheet(app);
    QGuiApplication::styleHints()->setColorScheme(Qt::ColorScheme::Dark);
    int exitCode = 0;

    do {
        auto networkManager = std::make_shared<ClientNetworkManager>("127.0.0.1", "8080");
        auto api = std::make_shared<ServerAPI>(networkManager);
        ClientState::setApi(api);

        LoginWindow loginDialog(api);

        if (loginDialog.exec() == QDialog::Accepted) {
            MainWindow mainWindow;
            mainWindow.show();
            exitCode = app.exec();
        } else {
            exitCode = 0;
        }
    } while (exitCode == 42); // do while is required, since we need to check the condition at the the end

    return exitCode;
}