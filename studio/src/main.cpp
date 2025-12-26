#include <QApplication>
#include "MainWindow.hpp"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("Strategient Architect");
    app.setApplicationVersion("0.1.0");
    app.setOrganizationName("Strategient");

    MainWindow window;
    window.show();

    return app.exec();
}

