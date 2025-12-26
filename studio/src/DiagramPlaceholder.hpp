#ifndef DIAGRAMPLACEHOLDER_HPP
#define DIAGRAMPLACEHOLDER_HPP

#include <QWidget>

class QLabel;
class DocumentModel;

/**
 * DiagramPlaceholder is a temporary central widget that displays
 * the current page title. Will be replaced by DiagramView in later sessions.
 */
class DiagramPlaceholder : public QWidget {
    Q_OBJECT

public:
    explicit DiagramPlaceholder(DocumentModel* model, QWidget* parent = nullptr);
    ~DiagramPlaceholder() override = default;

private slots:
    void onCurrentPageChanged(const QString& pageId);
    void onDocumentLoaded();

private:
    void updateDisplay();

    DocumentModel* m_model;
    QLabel* m_titleLabel;
    QLabel* m_typeLabel;
    QLabel* m_dotPreview;
};

#endif // DIAGRAMPLACEHOLDER_HPP

