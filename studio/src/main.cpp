#include <QApplication>
#include <QFile>
#include <QFont>
#include <QFontDatabase>
#include <QIcon>
#include "MainWindow.hpp"

int main(int argc, char* argv[]) {
    // Set WebEngine attributes before QApplication (required for some systems)
    // QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);

    QApplication app(argc, argv);
    app.setApplicationName("Strategient Architect");
    app.setApplicationVersion("0.1.0");
    app.setOrganizationName("Strategient");

    // Set application window icon with multiple sizes for crisp display
    QIcon appIcon;
    appIcon.addFile(":/icons/strategient-16.png", QSize(16, 16));
    appIcon.addFile(":/icons/strategient-24.png", QSize(24, 24));
    appIcon.addFile(":/icons/strategient-32.png", QSize(32, 32));
    appIcon.addFile(":/icons/strategient-48.png", QSize(48, 48));
    appIcon.addFile(":/icons/strategient-64.png", QSize(64, 64));
    appIcon.addFile(":/icons/strategient-128.png", QSize(128, 128));
    appIcon.addFile(":/icons/strategient-256.png", QSize(256, 256));
    app.setWindowIcon(appIcon);

    // Set application-wide font
    QFont appFont("Inter", 10);
    appFont.setStyleHint(QFont::SansSerif);
    appFont.setHintingPreference(QFont::PreferFullHinting);
    
    // Fallback fonts if Inter is not available
    QStringList fallbacks;
    fallbacks << "IBM Plex Sans" << "Source Sans 3" << "Segoe UI" << "SF Pro Display" << "Roboto" << "sans-serif";
    appFont.setFamilies(QStringList() << "Inter" << fallbacks);
    
    app.setFont(appFont);

    // Load and apply dark theme stylesheet
    QFile styleFile(":/theme/strategient-dark.qss");
    if (styleFile.open(QFile::ReadOnly | QFile::Text)) {
        QString styleSheet = QString::fromUtf8(styleFile.readAll());
        app.setStyleSheet(styleSheet);
        styleFile.close();
    }

    MainWindow window;
    window.showMaximized();

    return app.exec();
}
