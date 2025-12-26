#ifndef DIAGRAMVIEW_HPP
#define DIAGRAMVIEW_HPP

#include <QWidget>

class QSvgWidget;
class QLabel;
class QStackedWidget;
class QScrollArea;
class DocumentModel;
class PlantUMLRenderer;

/**
 * DiagramView is the central widget that displays rendered PlantUML diagrams.
 * It shows the SVG preview and can trigger re-renders when PlantUML changes.
 */
class DiagramView : public QWidget {
    Q_OBJECT

public:
    explicit DiagramView(DocumentModel* model, QWidget* parent = nullptr);
    ~DiagramView() override = default;

    // Load SVG from file
    void loadSvg(const QString& filePath);
    
    // Render PlantUML and display result
    void renderPlantUML(const QString& source);
    
    // Clear the view
    void clear();

signals:
    void renderStarted();
    void renderComplete();
    void renderError(const QString& message);

public slots:
    void onCurrentPageChanged(const QString& pageId);
    void onDocumentLoaded();

private slots:
    void onRenderComplete(const QString& svgPath);
    void onRenderError(const QString& errorMessage);

private:
    void updateDisplay();
    void showPlaceholder(const QString& message);
    void showSvg();
    void showError(const QString& message);

    DocumentModel* m_model;
    PlantUMLRenderer* m_renderer;
    
    QStackedWidget* m_stack{nullptr};
    QScrollArea* m_scrollArea{nullptr};
    QSvgWidget* m_svgWidget{nullptr};
    QLabel* m_placeholder{nullptr};
    QLabel* m_errorLabel{nullptr};
    QLabel* m_statusLabel{nullptr};
    
    QString m_currentSvgPath;
    bool m_isRendering{false};
};

#endif // DIAGRAMVIEW_HPP

