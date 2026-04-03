#include <QApplication>
#include <QCoreApplication>
#include "core/fonts/fontmanager.h"
#include "settings/settingsmanager.h"
#include "ui/mainwindow/mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QCoreApplication::setApplicationName("QtGraph");
    QCoreApplication::setOrganizationName("QtGraph");
    FontManager::loadApplicationFonts();
    SettingsManager settingsManager;
    const AppSettings &currentSettings = settingsManager.settings();
    app.setFont(FontManager::uiFont(currentSettings.uiFontFamily, currentSettings.uiFontSize));

    MainWindow w(&settingsManager);
    w.show();

    return app.exec();
}
