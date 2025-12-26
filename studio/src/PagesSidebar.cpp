#include "PagesSidebar.hpp"
#include "DocumentModel.hpp"

#include <QListWidget>
#include <QVBoxLayout>
#include <QLabel>

PagesSidebar::PagesSidebar(DocumentModel* model, QWidget* parent)
    : QWidget(parent)
    , m_model(model)
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    // Header
    auto* header = new QLabel("Pages");
    header->setStyleSheet(
        "QLabel { "
        "  background-color: #2d2d30; "
        "  color: #cccccc; "
        "  font-weight: bold; "
        "  padding: 8px; "
        "  border-bottom: 1px solid #3e3e42; "
        "}");
    layout->addWidget(header);

    // List widget
    m_listWidget = new QListWidget(this);
    m_listWidget->setMinimumWidth(180);
    m_listWidget->setStyleSheet(
        "QListWidget { "
        "  background-color: #252526; "
        "  color: #cccccc; "
        "  border: none; "
        "  outline: none; "
        "} "
        "QListWidget::item { "
        "  padding: 8px 12px; "
        "  border-bottom: 1px solid #3e3e42; "
        "} "
        "QListWidget::item:selected { "
        "  background-color: #094771; "
        "  color: #ffffff; "
        "} "
        "QListWidget::item:hover:!selected { "
        "  background-color: #2a2d2e; "
        "}");
    layout->addWidget(m_listWidget);

    // Connect signals
    connect(m_listWidget, &QListWidget::itemClicked,
            this, &PagesSidebar::onItemClicked);
    
    connect(m_model, &DocumentModel::documentLoaded,
            this, &PagesSidebar::onDocumentLoaded);
    connect(m_model, &DocumentModel::pagesChanged,
            this, &PagesSidebar::onPagesChanged);
    connect(m_model, &DocumentModel::currentPageChanged,
            this, &PagesSidebar::onCurrentPageChanged);
}

void PagesSidebar::onDocumentLoaded() {
    refreshPageList();
}

void PagesSidebar::onPagesChanged() {
    refreshPageList();
}

void PagesSidebar::onCurrentPageChanged(const QString& pageId) {
    Q_UNUSED(pageId);
    updateSelection();
}

void PagesSidebar::onItemClicked(QListWidgetItem* item) {
    if (!item) return;
    
    QString pageId = item->data(Qt::UserRole).toString();
    if (!pageId.isEmpty()) {
        m_model->setCurrentPage(pageId);
        emit pageSelected(pageId);
    }
}

void PagesSidebar::refreshPageList() {
    m_listWidget->clear();
    
    const auto& pages = m_model->pages();
    for (const auto& page : pages) {
        auto* item = new QListWidgetItem(page.title);
        item->setData(Qt::UserRole, page.pageId);
        
        // Add icon based on page type
        QString typeIcon;
        switch (page.pageType) {
            case architect::PageType::PipelineDiagram:
                typeIcon = "🔀";
                break;
            case architect::PageType::InfraConfig:
                typeIcon = "⚙️";
                break;
            case architect::PageType::Scheduler:
                typeIcon = "📅";
                break;
            case architect::PageType::BacktestSweeps:
                typeIcon = "📊";
                break;
            case architect::PageType::DiagramOnly:
            default:
                typeIcon = "📄";
                break;
        }
        item->setText(QString("%1  %2").arg(typeIcon, page.title));
        
        m_listWidget->addItem(item);
    }
    
    updateSelection();
}

void PagesSidebar::updateSelection() {
    QString currentId = m_model->currentPageId();
    
    for (int i = 0; i < m_listWidget->count(); ++i) {
        auto* item = m_listWidget->item(i);
        if (item->data(Qt::UserRole).toString() == currentId) {
            m_listWidget->setCurrentItem(item);
            return;
        }
    }
}

