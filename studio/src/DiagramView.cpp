#include "DiagramView.hpp"
#include "DocumentModel.hpp"
#include "PlantUMLRenderer.hpp"
#include "DiagramScene.hpp"
#include "NodeItem.hpp"

#include <QGraphicsView>
#include <QSvgWidget>
#include <QSvgRenderer>
#include <QLabel>
#include <QStackedWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QFile>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QScrollBar>

// Custom QGraphicsView with pan/zoom support
class InteractiveGraphicsView : public QGraphicsView {
public:
    explicit InteractiveGraphicsView(QWidget* parent = nullptr)
        : QGraphicsView(parent)
    {
        setRenderHint(QPainter::Antialiasing);
        setRenderHint(QPainter::SmoothPixmapTransform);
        // Use FullViewportUpdate to prevent ghosting/trails during drag
        setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
        setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
        setResizeAnchor(QGraphicsView::AnchorUnderMouse);
        setDragMode(QGraphicsView::NoDrag);
        setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        
        // Optimize for smooth updates
        setOptimizationFlag(QGraphicsView::DontAdjustForAntialiasing, true);
        
        // Set background (slate blue to match theme)
        setBackgroundBrush(QColor("#2D3A47"));
        setFrameShape(QFrame::NoFrame);
    }

protected:
    void wheelEvent(QWheelEvent* event) override {
        // Ctrl + wheel = zoom
        if (event->modifiers() & Qt::ControlModifier) {
            double scaleFactor = 1.15;
            if (event->angleDelta().y() < 0) {
                scaleFactor = 1.0 / scaleFactor;
            }
            
            // Limit zoom range
            qreal currentScale = transform().m11();
            qreal newScale = currentScale * scaleFactor;
            if (newScale >= 0.1 && newScale <= 5.0) {
                scale(scaleFactor, scaleFactor);
            }
            event->accept();
        } else {
            QGraphicsView::wheelEvent(event);
        }
    }
    
    void mousePressEvent(QMouseEvent* event) override {
        // Middle mouse or Space + left = pan
        if (event->button() == Qt::MiddleButton || 
            (event->button() == Qt::LeftButton && m_spacePressed)) {
            m_isPanning = true;
            m_lastPanPoint = event->pos();
            setCursor(Qt::ClosedHandCursor);
            event->accept();
            return;
        }
        QGraphicsView::mousePressEvent(event);
    }
    
    void mouseMoveEvent(QMouseEvent* event) override {
        if (m_isPanning) {
            QPointF delta = event->pos() - m_lastPanPoint;
            m_lastPanPoint = event->pos();
            
            horizontalScrollBar()->setValue(horizontalScrollBar()->value() - delta.x());
            verticalScrollBar()->setValue(verticalScrollBar()->value() - delta.y());
            event->accept();
            return;
        }
        QGraphicsView::mouseMoveEvent(event);
    }
    
    void mouseReleaseEvent(QMouseEvent* event) override {
        if (event->button() == Qt::MiddleButton || 
            (event->button() == Qt::LeftButton && m_isPanning)) {
            m_isPanning = false;
            setCursor(Qt::ArrowCursor);
            event->accept();
            return;
        }
        QGraphicsView::mouseReleaseEvent(event);
    }
    
    void keyPressEvent(QKeyEvent* event) override {
        if (event->key() == Qt::Key_Space && !event->isAutoRepeat()) {
            m_spacePressed = true;
            setCursor(Qt::OpenHandCursor);
        }
        // F = Fit to view
        else if (event->key() == Qt::Key_F) {
            fitToContents();
        }
        // 0 or Home = Reset zoom to 100%
        else if (event->key() == Qt::Key_0 || event->key() == Qt::Key_Home) {
            resetTransform();
            centerOnContents();
        }
        QGraphicsView::keyPressEvent(event);
    }
    
    void keyReleaseEvent(QKeyEvent* event) override {
        if (event->key() == Qt::Key_Space && !event->isAutoRepeat()) {
            m_spacePressed = false;
            if (!m_isPanning) {
                setCursor(Qt::ArrowCursor);
            }
        }
        QGraphicsView::keyReleaseEvent(event);
    }
    
    void fitToContents() {
        if (scene() && !scene()->items().isEmpty()) {
            QRectF bounds = scene()->itemsBoundingRect().adjusted(-50, -50, 50, 50);
            fitInView(bounds, Qt::KeepAspectRatio);
        }
    }
    
    void centerOnContents() {
        if (scene() && !scene()->items().isEmpty()) {
            centerOn(scene()->itemsBoundingRect().center());
        }
    }

private:
    bool m_isPanning = false;
    bool m_spacePressed = false;
    QPointF m_lastPanPoint;
};

// DiagramView implementation
DiagramView::DiagramView(DocumentModel* model, QWidget* parent)
    : QWidget(parent)
    , m_model(model)
    , m_renderer(new PlantUMLRenderer(this))
    , m_scene(new DiagramScene(this))
{
    setupUI();
    
    // Connect renderer signals
    connect(m_renderer, &PlantUMLRenderer::renderStarted, this, [this]() {
        m_isRendering = true;
        m_statusLabel->setText("Rendering PlantUML...");
        m_statusLabel->setStyleSheet(
            "QLabel { background-color: #435160; color: #dcdcaa; "
            "padding: 4px 8px; font-size: 11px; }");
        m_statusLabel->setVisible(true);
        emit renderStarted();
    });
    
    connect(m_renderer, &PlantUMLRenderer::renderComplete,
            this, &DiagramView::onRenderComplete);
    connect(m_renderer, &PlantUMLRenderer::renderError,
            this, &DiagramView::onRenderError);

    // Connect document model signals
    connect(m_model, &DocumentModel::currentPageChanged,
            this, &DiagramView::onCurrentPageChanged);
    connect(m_model, &DocumentModel::documentLoaded,
            this, &DiagramView::onDocumentLoaded);
    
    // Connect scene signals
    connect(m_scene, &DiagramScene::nodeClicked,
            this, &DiagramView::nodeSelected);
    connect(m_scene, &DiagramScene::nodeDoubleClicked,
            this, &DiagramView::nodeDoubleClicked);
    connect(m_scene, &DiagramScene::layoutChanged,
            this, &DiagramView::onSceneLayoutChanged);
}

void DiagramView::setupUI() {
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    // Status bar at top
    m_statusLabel = new QLabel("");
    m_statusLabel->setStyleSheet(
        "QLabel { background-color: #435160; color: #8495A9; "
        "padding: 4px 8px; font-size: 11px; }");
    m_statusLabel->setVisible(false);
    layout->addWidget(m_statusLabel);

    // Stacked widget for different views
    m_stack = new QStackedWidget(this);

    // Placeholder
    m_placeholder = new QLabel("No diagram to display.\n\n"
                               "Select a page from the sidebar\n"
                               "and edit PlantUML in the editor below.");
    m_placeholder->setAlignment(Qt::AlignCenter);
    m_placeholder->setStyleSheet(
        "QLabel { background-color: #2D3A47; color: #8495A9; font-size: 14px; }");

    // Error display
    m_errorWidget = new QWidget(this);
    m_errorWidget->setStyleSheet("background-color: #2D3A47;");
    auto* errorLayout = new QVBoxLayout(m_errorWidget);
    errorLayout->setAlignment(Qt::AlignCenter);
    errorLayout->setSpacing(16);
    errorLayout->setContentsMargins(40, 40, 40, 40);

    m_errorTitle = new QLabel("⚠️ Render Error");
    m_errorTitle->setAlignment(Qt::AlignCenter);
    m_errorTitle->setStyleSheet(
        "QLabel { color: #f48771; font-size: 18px; font-weight: bold; "
        "background: transparent; }");
    errorLayout->addWidget(m_errorTitle);

    m_errorDetails = new QLabel("");
    m_errorDetails->setAlignment(Qt::AlignCenter);
    m_errorDetails->setWordWrap(true);
    m_errorDetails->setTextInteractionFlags(Qt::TextSelectableByMouse);
    m_errorDetails->setMaximumWidth(600);
    m_errorDetails->setStyleSheet(
        "QLabel { color: #AEB9C7; font-size: 12px; "
        "font-family: 'JetBrains Mono', 'Fira Code', monospace; "
        "background-color: #384858; border: 1px solid #60748A; "
        "border-radius: 4px; padding: 16px; }");
    errorLayout->addWidget(m_errorDetails);

    auto* hintLabel = new QLabel("Check the Run Console for full error details.");
    hintLabel->setAlignment(Qt::AlignCenter);
    hintLabel->setStyleSheet(
        "QLabel { color: #6a9955; font-size: 11px; background: transparent; }");
    errorLayout->addWidget(hintLabel);

    // Interactive graphics view
    m_graphicsView = new InteractiveGraphicsView(this);
    m_graphicsView->setScene(m_scene);

    // Static SVG view
    m_svgContainer = new QWidget(this);
    m_svgContainer->setStyleSheet("background-color: #2D3A47;");
    auto* svgLayout = new QVBoxLayout(m_svgContainer);
    svgLayout->setContentsMargins(20, 20, 20, 20);
    svgLayout->setAlignment(Qt::AlignCenter);

    m_scrollArea = new QScrollArea(m_svgContainer);
    m_scrollArea->setWidgetResizable(false);
    m_scrollArea->setAlignment(Qt::AlignCenter);
    m_scrollArea->setFrameShape(QFrame::NoFrame);
    m_scrollArea->setStyleSheet(
        "QScrollArea { background-color: #2D3A47; border: none; }");
    
    m_svgWidget = new QSvgWidget();
    m_scrollArea->setWidget(m_svgWidget);
    svgLayout->addWidget(m_scrollArea);

    // Add all to stack
    m_stack->addWidget(m_placeholder);
    m_stack->addWidget(m_errorWidget);
    m_stack->addWidget(m_graphicsView);
    m_stack->addWidget(m_svgContainer);
    m_stack->setCurrentWidget(m_placeholder);

    layout->addWidget(m_stack);
}

void DiagramView::setViewMode(ViewMode mode) {
    m_viewMode = mode;
    updateDisplay();
}

void DiagramView::loadSvg(const QString& filePath) {
    if (!QFile::exists(filePath)) {
        showError("File not found", QString("SVG file does not exist:\n%1").arg(filePath));
        return;
    }

    m_svgWidget->load(filePath);
    m_currentSvgPath = filePath;
    
    // Size the widget to the SVG's natural size
    QSvgRenderer* renderer = m_svgWidget->renderer();
    if (renderer && renderer->isValid()) {
        QSize svgSize = renderer->defaultSize();
        if (svgSize.width() < 400) {
            svgSize *= 1.5;
        }
        m_svgWidget->setFixedSize(svgSize);
    }
    
    showStaticView();
}

void DiagramView::renderPlantUML(const QString& source) {
    if (source.trimmed().isEmpty()) {
        showPlaceholder("No PlantUML content.\n\nAdd PlantUML code in the editor below.");
        return;
    }
    
    // For static mode, render to SVG
    if (m_viewMode == Static) {
        m_renderer->renderToSvg(source);
    } else {
        // For interactive mode, load into scene
        const architect::Page* page = m_model->currentPage();
        if (page) {
            m_scene->loadDiagram(source, page->metadata);
            
            // Fit the view to show all content
            m_graphicsView->fitInView(m_scene->itemsBoundingRect().adjusted(-50, -50, 50, 50), 
                                       Qt::KeepAspectRatio);
            
            showInteractiveView();
        }
    }
}

void DiagramView::clear() {
    m_currentSvgPath.clear();
    m_scene->clearDiagram();
    showPlaceholder("No diagram to display.");
}

void DiagramView::onCurrentPageChanged(const QString& pageId) {
    Q_UNUSED(pageId);
    updateDisplay();
}

void DiagramView::onDocumentLoaded() {
    updateDisplay();
}

void DiagramView::updateDisplay() {
    const architect::Page* page = m_model->currentPage();
    
    if (!page) {
        showPlaceholder("No page selected.\n\nSelect a page from the sidebar.");
        return;
    }

    m_statusLabel->setText(QString("Page: %1").arg(page->title));
    m_statusLabel->setStyleSheet(
        "QLabel { background-color: #435160; color: #8495A9; "
        "padding: 4px 8px; font-size: 11px; }");
    m_statusLabel->setVisible(true);

    if (page->plantuml.isEmpty()) {
        showPlaceholder(QString("Page: %1\n\nNo PlantUML content.\n"
                                "Edit in the PlantUML Editor tab below.").arg(page->title));
        return;
    }

    renderPlantUML(page->plantuml);
}

void DiagramView::onRenderComplete(const QString& svgPath) {
    m_isRendering = false;
    
    const auto* page = m_model->currentPage();
    m_statusLabel->setText(QString("Page: %1 ✓").arg(page ? page->title : ""));
    m_statusLabel->setStyleSheet(
        "QLabel { background-color: #435160; color: #6a9955; "
        "padding: 4px 8px; font-size: 11px; }");
    
    if (m_viewMode == Static) {
        loadSvg(svgPath);
    }
    emit renderComplete();
}

void DiagramView::onRenderError(const QString& errorTitle, const QString& errorDetails) {
    m_isRendering = false;
    
    const auto* page = m_model->currentPage();
    m_statusLabel->setText(QString("Page: %1 ✗").arg(page ? page->title : ""));
    m_statusLabel->setStyleSheet(
        "QLabel { background-color: #435160; color: #f48771; "
        "padding: 4px 8px; font-size: 11px; }");
    
    showError(errorTitle, errorDetails);
    emit renderFailed(errorTitle, errorDetails);
}

void DiagramView::onSceneLayoutChanged() {
    saveLayoutToModel();
    emit layoutChanged();
}

void DiagramView::showPlaceholder(const QString& message) {
    m_placeholder->setText(message);
    m_stack->setCurrentWidget(m_placeholder);
}

void DiagramView::showInteractiveView() {
    m_stack->setCurrentWidget(m_graphicsView);
    m_statusLabel->setText(m_statusLabel->text().replace(" ✓", "") + " (interactive)");
}

void DiagramView::showStaticView() {
    m_stack->setCurrentWidget(m_svgContainer);
}

void DiagramView::showError(const QString& title, const QString& details) {
    m_errorTitle->setText(QString("⚠️ %1").arg(title));
    
    QString displayDetails = details;
    if (displayDetails.length() > 500) {
        displayDetails = displayDetails.left(500) + "\n\n... (see Run Console for full output)";
    }
    m_errorDetails->setText(displayDetails);
    
    m_stack->setCurrentWidget(m_errorWidget);
}

void DiagramView::saveLayoutToModel() {
    architect::Page* page = m_model->currentPageMutable();
    if (!page) return;
    
    // Get layout from scene and save to model
    page->metadata.layout = m_scene->getLayout();
    m_model->setDirty(true);
}
