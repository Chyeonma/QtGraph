#include <QApplication>
#include <QCoreApplication>
#include "app/appcontext.h"
#include "services/projectservice.h"
#include "services/fileservice.h"
#include "services/editorservice.h"
#include "services/settingsservice.h"
#include "models/appsettings.h"
#include "interfaces/ifontprovider.h"
#include "ui/mainwindow/mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QCoreApplication::setApplicationName("QtGraph");
    QCoreApplication::setOrganizationName("QtGraph");

    AppContext context;

    const AppSettings &currentSettings = context.settingsService()->settings();
    app.setFont(context.fontProvider()->uiFont(currentSettings.uiFontFamily, currentSettings.uiFontSize));

    MainWindow w(&context);
    w.show();

    return app.exec();
}
