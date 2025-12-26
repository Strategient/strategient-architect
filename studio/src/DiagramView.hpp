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
 * Shows clear error overlays when rendering fails.
 */
class DiagramView : public QWidget {
    Q_OBJECT

public:
    explicit DiagramView(DocumentModel* model, QWidget* parent = nullptr);
    ~DiagramView() override = default;

    void loadSvg(const QString& filePath);
    void renderPlantUML(const QString& source);
    void clear();

signals:
    void renderStarted();
    void renderComplete();
    // Detailed error signal for logging
    void renderFailed(const QString& title, const QString& details);

public slots:
    void onCurrentPageChanged(const QString& pageId);
    void onDocumentLoaded();

private slots:
    void onRenderComplete(const QString& svgPath);
    void onRenderError(const QString& errorTitle, const QString& errorDetails);

private:
    void updateDisplay();
    void showPlaceholder(const QString& message);
    void showSvg();
    void showError(const QString& title, const QString& details);

    DocumentModel* m_model;
    PlantUMLRenderer* m_renderer;
    
    QStackedWidget* m_stack{nullptr};
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
