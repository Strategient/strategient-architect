#include "DiagramView.hpp"
#include "DocumentModel.hpp"
#include "PlantUMLRenderer.hpp"

#include <QSvgWidget>
#include <QLabel>
#include <QStackedWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QFile>

DiagramView::DiagramView(DocumentModel* model, QWidget* parent)
    : QWidget(parent)
    , m_model(model)
    , m_renderer(new PlantUMLRenderer(this))
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    // Status bar at top
    m_statusLabel = new QLabel("");
    m_statusLabel->setStyleSheet(
        "QLabel { "
        "  background-color: #2d2d30; "
        "  color: #808080; "
        "  padding: 4px 8px; "
        "  font-size: 11px; "
        "}");
    m_statusLabel->setVisible(false);
    layout->addWidget(m_statusLabel);

    // Stacked widget for content
    m_stack = new QStackedWidget(this);

    // Placeholder (no page selected)
    m_placeholder = new QLabel("No diagram to display.\n\nSelect a page from the sidebar\nand edit PlantUML in the editor below.");
    m_placeholder->setAlignment(Qt::AlignCenter);
    m_placeholder->setStyleSheet(
        "QLabel { "
        "  background-color: #1e1e1e; "
        "  color: #808080; "
        "  font-size: 14px; "
        "}");

    // Error display
    m_errorLabel = new QLabel("");
    m_errorLabel->setAlignment(Qt::AlignCenter);
    m_errorLabel->setWordWrap(true);
    m_errorLabel->setStyleSheet(
        "QLabel { "
        "  background-color: #1e1e1e; "
        "  color: #f48771; "
        "  font-size: 12px; "
        "  padding: 20px; "
        "}");

    // SVG view with scroll area
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setStyleSheet(
        "QScrollArea { "
        "  background-color: #ffffff; "
        "  border: none; "
        "}");
    
    m_svgWidget = new QSvgWidget();
    m_svgWidget->setStyleSheet("background-color: #ffffff;");
    m_scrollArea->setWidget(m_svgWidget);

    m_stack->addWidget(m_placeholder);
    m_stack->addWidget(m_errorLabel);
    m_stack->addWidget(m_scrollArea);
    m_stack->setCurrentWidget(m_placeholder);

    layout->addWidget(m_stack);

    // Connect renderer signals
    connect(m_renderer, &PlantUMLRenderer::renderStarted, this, [this]() {
        m_isRendering = true;
        m_statusLabel->setText("Rendering PlantUML...");
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
}

void DiagramView::loadSvg(const QString& filePath) {
    if (!QFile::exists(filePath)) {
        showError(QString("SVG file not found:\n%1").arg(filePath));
        return;
    }

    m_svgWidget->load(filePath);
    m_currentSvgPath = filePath;
    showSvg();
}

void DiagramView::renderPlantUML(const QString& source) {
    if (source.trimmed().isEmpty()) {
        showPlaceholder("No PlantUML content.\n\nAdd PlantUML code in the editor below.");
        return;
    }
    m_renderer->renderToSvg(source);
}

void DiagramView::clear() {
    m_currentSvgPath.clear();
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
    m_statusLabel->setVisible(true);

    if (page->plantuml.isEmpty()) {
        showPlaceholder(QString("Page: %1\n\nNo PlantUML content.\nEdit in the PlantUML Editor tab below.").arg(page->title));
        return;
    }

    // Auto-render the PlantUML
    renderPlantUML(page->plantuml);
}

void DiagramView::onRenderComplete(const QString& svgPath) {
    m_isRendering = false;
    m_statusLabel->setText(QString("Page: %1 (rendered)").arg(
        m_model->currentPage() ? m_model->currentPage()->title : ""));
    
    loadSvg(svgPath);
    emit renderComplete();
}

void DiagramView::onRenderError(const QString& errorMessage) {
    m_isRendering = false;
    m_statusLabel->setVisible(false);
    showError(errorMessage);
    emit renderError(errorMessage);
}

void DiagramView::showPlaceholder(const QString& message) {
    m_placeholder->setText(message);
    m_stack->setCurrentWidget(m_placeholder);
}

void DiagramView::showSvg() {
    m_stack->setCurrentWidget(m_scrollArea);
}

void DiagramView::showError(const QString& message) {
    m_errorLabel->setText(QString("⚠️ Render Error\n\n%1").arg(message));
    m_stack->setCurrentWidget(m_errorLabel);
}

