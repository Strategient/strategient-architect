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

    // Error display widget
    m_errorWidget = new QWidget(this);
    m_errorWidget->setStyleSheet("background-color: #1e1e1e;");
    auto* errorLayout = new QVBoxLayout(m_errorWidget);
    errorLayout->setAlignment(Qt::AlignCenter);
    errorLayout->setSpacing(16);
    errorLayout->setContentsMargins(40, 40, 40, 40);

    // Error icon and title
    m_errorTitle = new QLabel("⚠️ Render Error");
    m_errorTitle->setAlignment(Qt::AlignCenter);
    m_errorTitle->setStyleSheet(
        "QLabel { "
        "  color: #f48771; "
        "  font-size: 18px; "
        "  font-weight: bold; "
        "  background: transparent; "
        "}");
    errorLayout->addWidget(m_errorTitle);

    // Error details
    m_errorDetails = new QLabel("");
    m_errorDetails->setAlignment(Qt::AlignCenter);
    m_errorDetails->setWordWrap(true);
    m_errorDetails->setTextInteractionFlags(Qt::TextSelectableByMouse);
    m_errorDetails->setMaximumWidth(600);
    m_errorDetails->setStyleSheet(
        "QLabel { "
        "  color: #cccccc; "
        "  font-size: 12px; "
        "  font-family: 'JetBrains Mono', 'Fira Code', monospace; "
        "  background-color: #2d2d30; "
        "  border: 1px solid #3e3e42; "
        "  border-radius: 4px; "
        "  padding: 16px; "
        "}");
    errorLayout->addWidget(m_errorDetails);

    // Hint label
    auto* hintLabel = new QLabel("Check the Run Console for full error details.");
    hintLabel->setAlignment(Qt::AlignCenter);
    hintLabel->setStyleSheet(
        "QLabel { "
        "  color: #6a9955; "
        "  font-size: 11px; "
        "  background: transparent; "
        "  margin-top: 8px; "
        "}");
    errorLayout->addWidget(hintLabel);

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
    m_stack->addWidget(m_errorWidget);
    m_stack->addWidget(m_scrollArea);
    m_stack->setCurrentWidget(m_placeholder);

    layout->addWidget(m_stack);

    // Connect renderer signals
    connect(m_renderer, &PlantUMLRenderer::renderStarted, this, [this]() {
        m_isRendering = true;
        m_statusLabel->setText("Rendering PlantUML...");
        m_statusLabel->setStyleSheet(
            "QLabel { "
            "  background-color: #2d2d30; "
            "  color: #dcdcaa; "
            "  padding: 4px 8px; "
            "  font-size: 11px; "
            "}");
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
        showError("File not found", QString("SVG file does not exist:\n%1").arg(filePath));
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
    m_statusLabel->setStyleSheet(
        "QLabel { "
        "  background-color: #2d2d30; "
        "  color: #808080; "
        "  padding: 4px 8px; "
        "  font-size: 11px; "
        "}");
    m_statusLabel->setVisible(true);

    if (page->plantuml.isEmpty()) {
        showPlaceholder(QString("Page: %1\n\nNo PlantUML content.\nEdit in the PlantUML Editor tab below.").arg(page->title));
        return;
    }

    renderPlantUML(page->plantuml);
}

void DiagramView::onRenderComplete(const QString& svgPath) {
    m_isRendering = false;
    
    const auto* page = m_model->currentPage();
    m_statusLabel->setText(QString("Page: %1 ✓").arg(page ? page->title : ""));
    m_statusLabel->setStyleSheet(
        "QLabel { "
        "  background-color: #2d2d30; "
        "  color: #6a9955; "
        "  padding: 4px 8px; "
        "  font-size: 11px; "
        "}");
    
    loadSvg(svgPath);
    emit renderComplete();
}

void DiagramView::onRenderError(const QString& errorTitle, const QString& errorDetails) {
    m_isRendering = false;
    
    const auto* page = m_model->currentPage();
    m_statusLabel->setText(QString("Page: %1 ✗").arg(page ? page->title : ""));
    m_statusLabel->setStyleSheet(
        "QLabel { "
        "  background-color: #2d2d30; "
        "  color: #f48771; "
        "  padding: 4px 8px; "
        "  font-size: 11px; "
        "}");
    
    showError(errorTitle, errorDetails);
    emit renderFailed(errorTitle, errorDetails);
}

void DiagramView::showPlaceholder(const QString& message) {
    m_placeholder->setText(message);
    m_stack->setCurrentWidget(m_placeholder);
}

void DiagramView::showSvg() {
    m_stack->setCurrentWidget(m_scrollArea);
}

void DiagramView::showError(const QString& title, const QString& details) {
    m_errorTitle->setText(QString("⚠️ %1").arg(title));
    
    // Format details for display (limit length for UI)
    QString displayDetails = details;
    if (displayDetails.length() > 500) {
        displayDetails = displayDetails.left(500) + "\n\n... (see Run Console for full output)";
    }
    m_errorDetails->setText(displayDetails);
    
    m_stack->setCurrentWidget(m_errorWidget);
}
