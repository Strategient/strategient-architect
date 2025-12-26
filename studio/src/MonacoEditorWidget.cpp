#include "MonacoEditorWidget.hpp"
#include "MonacoBridge.hpp"

#include <QVBoxLayout>
#include <QWebEngineView>
#include <QWebEnginePage>
#include <QWebEngineSettings>
#include <QWebChannel>
#include <QFile>
#include <QDebug>

MonacoEditorWidget::MonacoEditorWidget(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    setupWebEngine();
    layout->addWidget(m_webView);
}

MonacoEditorWidget::~MonacoEditorWidget() = default;

void MonacoEditorWidget::setupWebEngine() {
    m_webView = new QWebEngineView(this);
    m_channel = new QWebChannel(this);
    m_bridge = new MonacoBridge(this);

    // Register bridge with channel
    m_channel->registerObject(QStringLiteral("bridge"), m_bridge);
    m_webView->page()->setWebChannel(m_channel);

    // Configure web settings
    auto* settings = m_webView->page()->settings();
    settings->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
    settings->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, false);
    settings->setAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls, true);

    // Connect bridge signals
    connect(m_bridge, &MonacoBridge::editorReady,
            this, &MonacoEditorWidget::onEditorReady);
    connect(m_bridge, &MonacoBridge::textChanged,
            this, &MonacoEditorWidget::onBridgeTextChanged);

    // Load editor HTML from resources
    m_webView->setUrl(QUrl("qrc:/monaco/editor.html"));
}

void MonacoEditorWidget::onEditorReady() {
    m_editorReady = true;
    
    // Apply any pending state
    if (!m_pendingText.isEmpty()) {
        m_bridge->setText(m_pendingText);
    }
    m_bridge->setLanguage(m_pendingLanguage);
    m_bridge->setReadOnly(m_pendingReadOnly);
    
    emit editorReady();
}

void MonacoEditorWidget::onBridgeTextChanged(const QString& newText) {
    emit textChanged(newText);
}

void MonacoEditorWidget::setText(const QString& text) {
    m_pendingText = text;
    if (m_editorReady) {
        m_bridge->setText(text);
    }
}

QString MonacoEditorWidget::text() const {
    return m_bridge->text();
}

void MonacoEditorWidget::setLanguage(const QString& language) {
    m_pendingLanguage = language;
    if (m_editorReady) {
        m_bridge->setLanguage(language);
    }
}

void MonacoEditorWidget::setReadOnly(bool readOnly) {
    m_pendingReadOnly = readOnly;
    if (m_editorReady) {
        m_bridge->setReadOnly(readOnly);
    }
}

