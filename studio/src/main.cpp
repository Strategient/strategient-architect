#include <QApplication>
#include "MainWindow.hpp"

int main(int argc, char* argv[]) {
    // Set WebEngine attributes before QApplication (required for some systems)
    // QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);

    QApplication app(argc, argv);
    app.setApplicationName("Strategient Architect");
    app.setApplicationVersion("0.1.0");
    app.setOrganizationName("Strategient");

    MainWindow window;
    window.show();

    return app.exec();
}
