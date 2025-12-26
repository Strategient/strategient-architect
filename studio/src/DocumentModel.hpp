#ifndef DOCUMENTMODEL_HPP
#define DOCUMENTMODEL_HPP

#include <QObject>
#include <QString>
#include <QVector>
#include "ProjectModel.hpp"

/**
 * DocumentModel owns the current project state and provides
 * signals for UI updates when the document changes.
 */
class DocumentModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool dirty READ isDirty NOTIFY dirtyChanged)
    Q_PROPERTY(QString currentPageId READ currentPageId WRITE setCurrentPage NOTIFY currentPageChanged)

public:
    explicit DocumentModel(QObject* parent = nullptr);
    ~DocumentModel() override = default;

    // File operations
    bool loadFromFile(const QString& filePath);
    bool saveToFile(const QString& filePath);
    bool save();  // Save to current path

    // Project access
    const architect::Project& project() const { return m_project; }
    architect::Project& project() { return m_project; }
    
    // Pages
    QVector<architect::Page>& pages() { return m_project.pages; }
    const QVector<architect::Page>& pages() const { return m_project.pages; }
    int pageCount() const { return m_project.pages.size(); }
    
    const architect::Page* currentPage() const;
    architect::Page* currentPage();
    architect::Page* currentPageMutable() { return currentPage(); }  // Explicit mutable access
    int currentPageIndex() const;
    
    QString currentPageId() const { return m_currentPageId; }
    void setCurrentPage(const QString& pageId);
    void setCurrentPageByIndex(int index);

    // State
    bool isDirty() const { return m_dirty; }
    void setDirty(bool dirty);
    
    bool hasDocument() const { return m_hasDocument; }
    QString currentFilePath() const { return m_currentFilePath; }
    QString documentTitle() const;

    // Create new empty project
    void newProject(const QString& projectId, const QString& title);

signals:
    void documentLoaded();
    void documentSaved();
    void currentPageChanged(const QString& pageId);
    void dirtyChanged(bool dirty);
    void pagesChanged();

private:
    architect::Project m_project;
    QString m_currentPageId;
    QString m_currentFilePath;
    bool m_dirty = false;
    bool m_hasDocument = false;
};

#endif // DOCUMENTMODEL_HPP

