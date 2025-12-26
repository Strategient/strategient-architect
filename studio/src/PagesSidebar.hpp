#ifndef PAGESSIDEBAR_HPP
#define PAGESSIDEBAR_HPP

#include <QWidget>

class QListWidget;
class QListWidgetItem;
class DocumentModel;

/**
 * PagesSidebar displays the list of pages in the current project
 * and allows switching the active page.
 */
class PagesSidebar : public QWidget {
    Q_OBJECT

public:
    explicit PagesSidebar(DocumentModel* model, QWidget* parent = nullptr);
    ~PagesSidebar() override = default;

signals:
    void pageSelected(const QString& pageId);

private slots:
    void onDocumentLoaded();
    void onPagesChanged();
    void onCurrentPageChanged(const QString& pageId);
    void onItemClicked(QListWidgetItem* item);

private:
    void refreshPageList();
    void updateSelection();

    DocumentModel* m_model;
    QListWidget* m_listWidget;
};

#endif // PAGESSIDEBAR_HPP

