#include "DiagramPlaceholder.hpp"
#include "DocumentModel.hpp"

#include <QLabel>
#include <QVBoxLayout>

DiagramPlaceholder::DiagramPlaceholder(DocumentModel* model, QWidget* parent)
    : QWidget(parent)
    , m_model(model)
{
    auto* layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(16);

    // Title label
    m_titleLabel = new QLabel("No Page Selected");
    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_titleLabel->setStyleSheet(
        "QLabel { "
        "  color: #cccccc; "
        "  font-size: 24px; "
        "  font-weight: bold; "
        "}");
    layout->addWidget(m_titleLabel);

    // Type label
    m_typeLabel = new QLabel("");
    m_typeLabel->setAlignment(Qt::AlignCenter);
    m_typeLabel->setStyleSheet(
        "QLabel { "
        "  color: #808080; "
        "  font-size: 14px; "
        "}");
    layout->addWidget(m_typeLabel);

    // PlantUML preview (first few lines)
    m_plantumlPreview = new QLabel("");
    m_plantumlPreview->setAlignment(Qt::AlignCenter);
    m_plantumlPreview->setWordWrap(true);
    m_plantumlPreview->setMaximumWidth(600);
    m_plantumlPreview->setStyleSheet(
        "QLabel { "
        "  color: #6a9955; "
        "  font-family: 'JetBrains Mono', 'Fira Code', monospace; "
        "  font-size: 11px; "
        "  background-color: #1e1e1e; "
        "  border: 1px solid #333; "
        "  border-radius: 4px; "
        "  padding: 12px; "
        "}");
    layout->addWidget(m_plantumlPreview);

    setStyleSheet("background-color: #1e1e1e;");

    // Connect signals
    connect(m_model, &DocumentModel::currentPageChanged,
            this, &DiagramPlaceholder::onCurrentPageChanged);
    connect(m_model, &DocumentModel::documentLoaded,
            this, &DiagramPlaceholder::onDocumentLoaded);
}

void DiagramPlaceholder::onCurrentPageChanged(const QString& pageId) {
    Q_UNUSED(pageId);
    updateDisplay();
}

void DiagramPlaceholder::onDocumentLoaded() {
    updateDisplay();
}

void DiagramPlaceholder::updateDisplay() {
    const architect::Page* page = m_model->currentPage();
    
    if (!page) {
        m_titleLabel->setText("No Page Selected");
        m_typeLabel->setText("Open a project or select a page from the sidebar");
        m_plantumlPreview->setText("");
        m_plantumlPreview->setVisible(false);
        return;
    }

    m_titleLabel->setText(QString("Active Page: %1").arg(page->title));
    m_typeLabel->setText(QString("Type: %1").arg(
        architect::pageTypeToString(page->pageType)));

    // Show first ~10 lines of PlantUML
    if (!page->plantuml.isEmpty()) {
        QStringList lines = page->plantuml.split('\n');
        QStringList preview;
        for (int i = 0; i < qMin(12, lines.size()); ++i) {
            preview.append(lines[i]);
        }
        if (lines.size() > 12) {
            preview.append("...");
        }
        m_plantumlPreview->setText(preview.join('\n'));
        m_plantumlPreview->setVisible(true);
    } else {
        m_plantumlPreview->setText("(no PlantUML content)");
        m_plantumlPreview->setVisible(true);
    }
}

