#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>

class RunConsole;
class PlantUMLPreview;
class DocumentModel;
class PagesSidebar;
class DiagramView;
class MonacoEditorWidget;
class QTabWidget;
class QDockWidget;

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
    void onEditorTextChanged(const QString& newText);
    void onEditorReady();

private:
    void setupUI();
    void setupMenus();
    void logMessage(const QString& message);
    void updateWindowTitle();
    void updateEditorForCurrentPage();

    // Core components
    DocumentModel* m_documentModel{nullptr};
    
    // UI components
    RunConsole* m_runConsole{nullptr};
    PlantUMLPreview* m_plantUMLPreview{nullptr};
    PagesSidebar* m_pagesSidebar{nullptr};
    DiagramView* m_diagramView{nullptr};
    MonacoEditorWidget* m_monacoEditor{nullptr};
    
    // Bottom pane tabs
    QTabWidget* m_bottomTabs{nullptr};
    QDockWidget* m_bottomDock{nullptr};
    
    // Inspector placeholder
    QWidget* m_inspectorWidget{nullptr};
    
    // Change guard to prevent circular updates
    bool m_updatingFromModel{false};
};

#endif // MAINWINDOW_HPP
