#include "MonacoBridge.hpp"
#include <QDebug>

MonacoBridge::MonacoBridge(QObject* parent)
    : QObject(parent)
{
}

void MonacoBridge::setText(const QString& text) {
    m_currentText = text;
    emit requestSetText(text);
}

void MonacoBridge::setLanguage(const QString& language) {
    m_currentLanguage = language;
    emit requestSetLanguage(language);
}

void MonacoBridge::setReadOnly(bool readOnly) {
    m_readOnly = readOnly;
    emit requestSetReadOnly(readOnly);
}

void MonacoBridge::notifyTextChanged(const QString& newText) {
    if (m_currentText != newText) {
        m_currentText = newText;
        emit textChanged(newText);
    }
}

void MonacoBridge::notifyEditorReady() {
    qDebug() << "[MonacoBridge] Editor ready";
    emit editorReady();
}

