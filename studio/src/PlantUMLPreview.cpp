#include "PlantUMLPreview.hpp"

#include <QSvgWidget>
#include <QLabel>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QFile>

PlantUMLPreview::PlantUMLPreview(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    m_stack = new QStackedWidget(this);

    // Placeholder when no SVG is loaded
    m_placeholder = new QLabel("No SVG loaded.\n\nUse File → Open SVG\nor render a PlantUML diagram.");
    m_placeholder->setAlignment(Qt::AlignCenter);
    m_placeholder->setStyleSheet(
        "QLabel { background-color: #252526; color: #808080; font-size: 12px; }");

    // SVG widget for rendering
    m_svgWidget = new QSvgWidget(this);
    m_svgWidget->setStyleSheet("background-color: #ffffff;");

    m_stack->addWidget(m_placeholder);
    m_stack->addWidget(m_svgWidget);
    m_stack->setCurrentWidget(m_placeholder);

    layout->addWidget(m_stack);
}

void PlantUMLPreview::loadSvg(const QString& filePath) {
    if (!QFile::exists(filePath)) {
        m_placeholder->setText(QString("File not found:\n%1").arg(filePath));
        m_stack->setCurrentWidget(m_placeholder);
        return;
    }

    m_svgWidget->load(filePath);
    m_currentPath = filePath;
    m_stack->setCurrentWidget(m_svgWidget);
}

void PlantUMLPreview::clear() {
    m_currentPath.clear();
    m_stack->setCurrentWidget(m_placeholder);
}

