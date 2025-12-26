#ifndef MONACOEDITORWIDGET_HPP
#define MONACOEDITORWIDGET_HPP

#include <QWidget>

class QWebEngineView;
class QWebChannel;
class MonacoBridge;

/**
 * MonacoEditorWidget wraps QWebEngineView to provide a Monaco-based code editor.
 * Used for editing Graphviz DOT source with syntax highlighting and autocomplete.
 */
class MonacoEditorWidget : public QWidget {
    Q_OBJECT

public:
    explicit MonacoEditorWidget(QWidget* parent = nullptr);
    ~MonacoEditorWidget() override;

    // Editor API
    void setText(const QString& text);
    QString text() const;
    void setLanguage(const QString& language);
    void setReadOnly(bool readOnly);
    
    bool isEditorReady() const { return m_editorReady; }

signals:
    void textChanged(const QString& newText);
    void editorReady();

private slots:
    void onEditorReady();
    void onBridgeTextChanged(const QString& newText);

private:
    void setupWebEngine();

    QWebEngineView* m_webView{nullptr};
    QWebChannel* m_channel{nullptr};
    MonacoBridge* m_bridge{nullptr};
    
    bool m_editorReady{false};
    QString m_pendingText;
    QString m_pendingLanguage{"dot"};
    bool m_pendingReadOnly{false};
};

#endif // MONACOEDITORWIDGET_HPP

