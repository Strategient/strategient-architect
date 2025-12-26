#ifndef MONACOBRIDGE_HPP
#define MONACOBRIDGE_HPP

#include <QObject>
#include <QString>

/**
 * MonacoBridge provides C++ <-> JavaScript communication for the Monaco Editor.
 * Exposed to JavaScript via QWebChannel.
 */
class MonacoBridge : public QObject {
    Q_OBJECT

public:
    explicit MonacoBridge(QObject* parent = nullptr);
    ~MonacoBridge() override = default;

    // Called from C++ to set editor content
    Q_INVOKABLE void setText(const QString& text);
    
    // Called from C++ to get current text (synchronous via cached value)
    QString text() const { return m_currentText; }
    
    // Called from C++ to set language
    Q_INVOKABLE void setLanguage(const QString& language);
    
    // Called from C++ to set read-only state
    Q_INVOKABLE void setReadOnly(bool readOnly);

public slots:
    // Called from JavaScript when editor content changes
    void notifyTextChanged(const QString& newText);
    
    // Called from JavaScript when editor is ready
    void notifyEditorReady();

signals:
    // Emitted when text changes (from JS)
    void textChanged(const QString& newText);
    
    // Emitted when editor is fully initialized
    void editorReady();
    
    // Signals to JS (connected in JavaScript)
    void requestSetText(const QString& text);
    void requestSetLanguage(const QString& language);
    void requestSetReadOnly(bool readOnly);

private:
    QString m_currentText;
    QString m_currentLanguage{"plantuml"};
    bool m_readOnly{false};
};

#endif // MONACOBRIDGE_HPP

