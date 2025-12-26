#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>

class RunConsole;
class PlantUMLPreview;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override = default;

private:
    void setupUI();
    void setupMenus();
    void logMessage(const QString& message);

    RunConsole* m_runConsole{nullptr};
    PlantUMLPreview* m_plantUMLPreview{nullptr};
};

#endif // MAINWINDOW_HPP

