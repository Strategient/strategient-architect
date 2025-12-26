#ifndef GRAPHVIZRENDERER_HPP
#define GRAPHVIZRENDERER_HPP

#include <QObject>
#include <QString>
#include <QByteArray>

// Forward declarations for Graphviz types
typedef struct GVC_s GVC_t;
typedef struct Agraph_s graph_t;

/**
 * GraphvizRenderer handles rendering DOT language diagrams to SVG.
 * Uses the native Graphviz library (libgvc, libcgraph) for high-quality
 * rendering with multiple layout engine options.
 * 
 * Supported layout engines:
 * - dot: hierarchical layouts (default, best for directed graphs)
 * - neato: spring model layouts (undirected graphs)
 * - fdp: force-directed placement (large undirected graphs)
 * - sfdp: scalable force-directed placement (very large graphs)
 * - circo: circular layouts
 * - twopi: radial layouts
 * - osage: array-based layouts
 * - patchwork: squarified tree maps
 */
class GraphvizRenderer : public QObject {
    Q_OBJECT

public:
    // Available layout engines
    enum class LayoutEngine {
        Dot,       // Hierarchical (default)
        Neato,     // Spring model
        Fdp,       // Force-directed
        Sfdp,      // Scalable force-directed
        Circo,     // Circular
        Twopi,     // Radial
        Osage,     // Array-based
        Patchwork  // Treemap
    };

    explicit GraphvizRenderer(QObject* parent = nullptr);
    ~GraphvizRenderer() override;

    // Render DOT source to SVG
    void renderToSvg(const QString& dotSource);
    
    // Get the last rendered SVG file path
    QString lastSvgPath() const { return m_lastSvgPath; }
    
    // Get last rendered SVG content (in memory)
    QByteArray lastSvgContent() const { return m_lastSvgContent; }
    
    // Check if Graphviz is available
    static bool isGraphvizAvailable();
    
    // Layout engine selection
    void setLayoutEngine(LayoutEngine engine);
    LayoutEngine layoutEngine() const { return m_layoutEngine; }
    QString layoutEngineName() const;
    
    // Static helper to convert engine enum to string
    static QString engineToString(LayoutEngine engine);
    static LayoutEngine engineFromString(const QString& name);

signals:
    void renderStarted();
    void renderComplete(const QString& svgPath);
    void renderCompleteInMemory(const QByteArray& svgContent);
    
    // Error signal with title and detailed message
    void renderError(const QString& errorTitle, const QString& errorDetails);

private:
    QString validateDotSource(const QString& source);
    bool initializeGraphviz();
    void cleanupGraphviz();
    void renderToSvgViaCLI(const QString& dotSource);
    
#ifndef NO_GRAPHVIZ
    GVC_t* m_gvc{nullptr};
#endif
    LayoutEngine m_layoutEngine{LayoutEngine::Dot};
    
    QString m_lastSvgPath;
    QByteArray m_lastSvgContent;
    QString m_tempDir;
};

#endif // GRAPHVIZRENDERER_HPP

