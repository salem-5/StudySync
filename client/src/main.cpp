#include <QApplication>
#include "ui/MainWindow.h"
#include "ClientNetworkManager.h"
#include "LanguageManager.h"
#include "ui/ClientState.h"
#include "ui/widget/LoginWindow.h"


int main(int argc, char* argv[]) {
    auto networkManager = std::make_shared<ClientNetworkManager>("127.0.0.1", "8080");
    auto api = std::make_shared<ServerAPI>(networkManager);
    ClientState::setApi(api);
    LanguageManager::loadFallback(":/resources/lang/en_us.json");
    LanguageManager::loadLanguage(":/resources/lang/en_us.json");
    QApplication app(argc, argv);
    MainWindow::loadStylesheet(app);

    LoginWindow loginDialog(api);
    if (loginDialog.exec() == QDialog::Accepted) {
        MainWindow mainWindow;
        MainWindow::loadStylesheet(app);
        mainWindow.show();
        return app.exec();
    }
}
