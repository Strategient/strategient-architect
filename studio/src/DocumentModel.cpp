#include "DocumentModel.hpp"

#include <QFile>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QDebug>

DocumentModel::DocumentModel(QObject* parent)
    : QObject(parent)
{
}

bool DocumentModel::loadFromFile(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open file:" << filePath;
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        qWarning() << "JSON parse error:" << parseError.errorString();
        return false;
    }

    m_project = architect::Project::fromJsonDocument(doc);
    m_currentFilePath = filePath;
    m_hasDocument = true;
    m_dirty = false;

    // Set current page to first page if available
    if (!m_project.pages.isEmpty()) {
        m_currentPageId = m_project.pages.first().pageId;
    } else {
        m_currentPageId.clear();
    }

    emit documentLoaded();
    emit currentPageChanged(m_currentPageId);
    emit dirtyChanged(m_dirty);
    emit pagesChanged();

    return true;
}

bool DocumentModel::saveToFile(const QString& filePath) {
    QJsonDocument doc = m_project.toJsonDocument();
    QByteArray data = doc.toJson(QJsonDocument::Indented);

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Failed to open file for writing:" << filePath;
        return false;
    }

    file.write(data);
    file.close();

    m_currentFilePath = filePath;
    m_dirty = false;

    emit documentSaved();
    emit dirtyChanged(m_dirty);

    return true;
}

bool DocumentModel::save() {
    if (m_currentFilePath.isEmpty()) {
        return false;
    }
    return saveToFile(m_currentFilePath);
}

const architect::Page* DocumentModel::currentPage() const {
    for (const auto& page : m_project.pages) {
        if (page.pageId == m_currentPageId) {
            return &page;
        }
    }
    return nullptr;
}

architect::Page* DocumentModel::currentPage() {
    for (auto& page : m_project.pages) {
        if (page.pageId == m_currentPageId) {
            return &page;
        }
    }
    return nullptr;
}

int DocumentModel::currentPageIndex() const {
    for (int i = 0; i < m_project.pages.size(); ++i) {
        if (m_project.pages[i].pageId == m_currentPageId) {
            return i;
        }
    }
    return -1;
}

void DocumentModel::setCurrentPage(const QString& pageId) {
    if (m_currentPageId == pageId) {
        return;
    }

    // Verify page exists
    bool found = false;
    for (const auto& page : m_project.pages) {
        if (page.pageId == pageId) {
            found = true;
            break;
        }
    }

    if (found) {
        m_currentPageId = pageId;
        emit currentPageChanged(m_currentPageId);
    }
}

void DocumentModel::setCurrentPageByIndex(int index) {
    if (index >= 0 && index < m_project.pages.size()) {
        setCurrentPage(m_project.pages[index].pageId);
    }
}

void DocumentModel::setDirty(bool dirty) {
    if (m_dirty != dirty) {
        m_dirty = dirty;
        emit dirtyChanged(m_dirty);
    }
}

QString DocumentModel::documentTitle() const {
    if (!m_hasDocument) {
        return "Untitled";
    }
    return m_project.title.isEmpty() ? "Untitled Project" : m_project.title;
}

void DocumentModel::newProject(const QString& projectId, const QString& title) {
    m_project = architect::Project();
    m_project.projectId = projectId;
    m_project.title = title;
    m_currentFilePath.clear();
    m_currentPageId.clear();
    m_hasDocument = true;
    m_dirty = true;

    emit documentLoaded();
    emit dirtyChanged(m_dirty);
    emit pagesChanged();
}

