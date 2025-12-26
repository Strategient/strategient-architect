#ifndef DIAGRAMVIEW_HPP
#define DIAGRAMVIEW_HPP

#include <QWidget>
#include <QStackedWidget>

class QGraphicsView;
class QLabel;
class QScrollArea;
class QSvgWidget;
class DocumentModel;
class GraphvizRenderer;
class DiagramScene;

/**
 * DiagramView - Central widget for displaying diagrams.
 * 
 * Supports two modes:
 * 1. Interactive mode: QGraphicsView with draggable nodes
 * 2. Static mode: SVG render of Graphviz DOT
 * 
 * Features:
 * - Pan with middle mouse or Space+drag
 * - Zoom with Ctrl+wheel
 * - Node selection and dragging
 * - Layout persistence
 * - Multiple Graphviz layout engines (dot, neato, fdp, etc.)
 */
class DiagramView : public QWidget {
    Q_OBJECT

public:
    explicit DiagramView(DocumentModel* model, QWidget* parent = nullptr);
    ~DiagramView() override = default;

    // Mode control
    enum ViewMode { Interactive, Static };
    void setViewMode(ViewMode mode);
    ViewMode viewMode() const { return m_viewMode; }
    
    // Static mode: load SVG directly
    void loadSvg(const QString& filePath);
    
    // Re-render current page's Graphviz DOT source
    void renderGraphviz(const QString& dotSource);
    
    
    // Clear display
    void clear();
    
    // Access to scene
    DiagramScene* scene() const { return m_scene; }

signals:
    void renderStarted();
    void renderComplete();
    void renderFailed(const QString& title, const QString& details);
    void nodeSelected(const QString& nodeId);
    void nodeDoubleClicked(const QString& nodeId);
    void layoutChanged();

public slots:
    void onCurrentPageChanged(const QString& pageId);
    void onDocumentLoaded();

private slots:
    void onRenderComplete(const QString& svgPath);
    void onRenderError(const QString& errorTitle, const QString& errorDetails);
    void onSceneLayoutChanged();

private:
    void setupUI();
    void updateDisplay();
    void showPlaceholder(const QString& message);
    void showInteractiveView();
    void showStaticView();
    void showError(const QString& title, const QString& details);
    void saveLayoutToModel();

    DocumentModel* m_model;
    GraphvizRenderer* m_renderer;
    DiagramScene* m_scene;
    
    ViewMode m_viewMode = Interactive;
    
    // UI components
    QStackedWidget* m_stack{nullptr};
    QGraphicsView* m_graphicsView{nullptr};
    QWidget* m_svgContainer{nullptr};
    QScrollArea* m_scrollArea{nullptr};
    QSvgWidget* m_svgWidget{nullptr};
    QLabel* m_placeholder{nullptr};
    QWidget* m_errorWidget{nullptr};
    QLabel* m_errorTitle{nullptr};
    QLabel* m_errorDetails{nullptr};
    QLabel* m_statusLabel{nullptr};
    
    QString m_currentSvgPath;
    bool m_isRendering{false};
};

#endif // DIAGRAMVIEW_HPP
