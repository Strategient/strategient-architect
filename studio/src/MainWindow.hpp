#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>

class RunConsole;
class PlantUMLPreview;
class DocumentModel;
class PagesSidebar;
class DiagramPlaceholder;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override = default;

private slots:
    void onOpenProject();
    void onSaveProject();
    void onSaveProjectAs();
    void onDocumentLoaded();
    void onDirtyChanged(bool dirty);
    void onCurrentPageChanged(const QString& pageId);

private:
    void setupUI();
    void setupMenus();
    void logMessage(const QString& message);
    void updateWindowTitle();

    // Core components
    DocumentModel* m_documentModel{nullptr};
    
    // UI components
    RunConsole* m_runConsole{nullptr};
    PlantUMLPreview* m_plantUMLPreview{nullptr};
    PagesSidebar* m_pagesSidebar{nullptr};
    DiagramPlaceholder* m_diagramPlaceholder{nullptr};
};

#endif // MAINWINDOW_HPP
