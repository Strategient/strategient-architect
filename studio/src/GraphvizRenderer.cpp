#include "GraphvizRenderer.hpp"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTemporaryDir>
#include <QDebug>
#include <QRegularExpression>
#include <QCoreApplication>
#include <QProcess>

#ifndef NO_GRAPHVIZ
// Graphviz headers (only when library is available)
#include <graphviz/gvc.h>
#include <graphviz/cgraph.h>
#endif

GraphvizRenderer::GraphvizRenderer(QObject* parent)
    : QObject(parent)
{
    m_tempDir = QDir::tempPath();
    
#ifndef NO_GRAPHVIZ
    // Initialize Graphviz context
    if (!initializeGraphviz()) {
        qWarning() << "[GraphvizRenderer] Failed to initialize Graphviz context";
    }
#else
    qDebug() << "[GraphvizRenderer] Running without libgraphviz - using CLI fallback";
#endif
}

GraphvizRenderer::~GraphvizRenderer() {
#ifndef NO_GRAPHVIZ
    cleanupGraphviz();
#endif
}

bool GraphvizRenderer::initializeGraphviz() {
#ifndef NO_GRAPHVIZ
    if (m_gvc) {
        return true;  // Already initialized
    }
    
    m_gvc = gvContext();
    if (!m_gvc) {
        qWarning() << "[GraphvizRenderer] gvContext() returned null";
        return false;
    }
    
    qDebug() << "[GraphvizRenderer] Graphviz context initialized successfully";
    return true;
#else
    return false;
#endif
}

void GraphvizRenderer::cleanupGraphviz() {
#ifndef NO_GRAPHVIZ
    if (m_gvc) {
        gvFreeContext(m_gvc);
        m_gvc = nullptr;
    }
#endif
}

bool GraphvizRenderer::isGraphvizAvailable() {
#ifndef NO_GRAPHVIZ
    GVC_t* testCtx = gvContext();
    if (testCtx) {
        gvFreeContext(testCtx);
        return true;
    }
    return false;
#else
    // Check if dot CLI is available
    QProcess check;
    check.start("which", {"dot"});
    check.waitForFinished(3000);
    return check.exitCode() == 0;
#endif
}

void GraphvizRenderer::setLayoutEngine(LayoutEngine engine) {
    m_layoutEngine = engine;
}

QString GraphvizRenderer::layoutEngineName() const {
    return engineToString(m_layoutEngine);
}

QString GraphvizRenderer::engineToString(LayoutEngine engine) {
    switch (engine) {
        case LayoutEngine::Dot:       return "dot";
        case LayoutEngine::Neato:     return "neato";
        case LayoutEngine::Fdp:       return "fdp";
        case LayoutEngine::Sfdp:      return "sfdp";
        case LayoutEngine::Circo:     return "circo";
        case LayoutEngine::Twopi:     return "twopi";
        case LayoutEngine::Osage:     return "osage";
        case LayoutEngine::Patchwork: return "patchwork";
    }
    return "dot";
}

GraphvizRenderer::LayoutEngine GraphvizRenderer::engineFromString(const QString& name) {
    QString lower = name.toLower();
    if (lower == "neato")     return LayoutEngine::Neato;
    if (lower == "fdp")       return LayoutEngine::Fdp;
    if (lower == "sfdp")      return LayoutEngine::Sfdp;
    if (lower == "circo")     return LayoutEngine::Circo;
    if (lower == "twopi")     return LayoutEngine::Twopi;
    if (lower == "osage")     return LayoutEngine::Osage;
    if (lower == "patchwork") return LayoutEngine::Patchwork;
    return LayoutEngine::Dot;  // Default
}

QString GraphvizRenderer::validateDotSource(const QString& source) {
    QString trimmed = source.trimmed();
    
    // Check for basic graph structure
    static QRegularExpression graphRe(
        R"(^\s*(strict\s+)?(di)?graph\s+)",
        QRegularExpression::CaseInsensitiveOption | QRegularExpression::MultilineOption
    );
    
    if (!graphRe.match(trimmed).hasMatch()) {
        return "Invalid DOT syntax: Missing 'digraph' or 'graph' declaration.\n"
               "DOT files should start with:\n"
               "  digraph G { ... }  (for directed graphs)\n"
               "  graph G { ... }    (for undirected graphs)";
    }
    
    // Check for balanced braces
    int braceCount = 0;
    for (const QChar& c : trimmed) {
        if (c == '{') braceCount++;
        if (c == '}') braceCount--;
        if (braceCount < 0) {
            return "Invalid DOT syntax: Unbalanced braces - found '}' without matching '{'";
        }
    }
    if (braceCount != 0) {
        return QString("Invalid DOT syntax: Unbalanced braces - missing %1 closing '}'")
               .arg(braceCount);
    }
    
    return QString();  // No validation errors
}

void GraphvizRenderer::renderToSvg(const QString& dotSource) {
    qDebug() << "[GraphvizRenderer] renderToSvg called, source length:" << dotSource.length();
    
    if (dotSource.trimmed().isEmpty()) {
        emit renderError("Empty DOT source", "No DOT content to render.");
        return;
    }

    // Validate source before attempting render
    QString validationError = validateDotSource(dotSource);
    if (!validationError.isEmpty()) {
        emit renderError("DOT validation failed", validationError);
        return;
    }

    emit renderStarted();

#ifndef NO_GRAPHVIZ
    // Use native Graphviz library
    
    // Ensure Graphviz is initialized
    if (!m_gvc && !initializeGraphviz()) {
        emit renderError("Graphviz not available", 
                        "Failed to initialize Graphviz context.\n\n"
                        "Install Graphviz with:\n"
                        "  sudo apt install graphviz libgraphviz-dev");
        return;
    }

    // Parse the DOT source into a graph
    QByteArray sourceBytes = dotSource.toUtf8();
    graph_t* graph = agmemread(sourceBytes.constData());
    
    if (!graph) {
        emit renderError("DOT parse error", 
                        "Failed to parse DOT source.\n"
                        "Check syntax and ensure all strings are properly quoted.");
        return;
    }

    // Apply layout using selected engine
    QString engineName = layoutEngineName();
    int layoutResult = gvLayout(m_gvc, graph, engineName.toUtf8().constData());
    
    if (layoutResult != 0) {
        agclose(graph);
        emit renderError("Layout failed", 
                        QString("Graphviz layout engine '%1' failed.\n"
                                "Try a different layout engine or simplify the graph.")
                        .arg(engineName));
        return;
    }

    // Render to SVG in memory
    char* svgData = nullptr;
    unsigned int svgLength = 0;
    
    int renderResult = gvRenderData(m_gvc, graph, "svg", &svgData, &svgLength);
    
    if (renderResult != 0 || !svgData) {
        gvFreeLayout(m_gvc, graph);
        agclose(graph);
        emit renderError("Render failed", 
                        "Graphviz failed to render SVG output.\n"
                        "The graph may be too complex or contain unsupported features.");
        return;
    }

    // Store SVG content
    m_lastSvgContent = QByteArray(svgData, svgLength);
    
    // Write to temp file
    QString outputPath = m_tempDir + "/strategient_graphviz.svg";
    QFile outFile(outputPath);
    if (outFile.open(QIODevice::WriteOnly)) {
        outFile.write(m_lastSvgContent);
        outFile.close();
        m_lastSvgPath = outputPath;
        qDebug() << "[GraphvizRenderer] SVG written to:" << outputPath;
    } else {
        qWarning() << "[GraphvizRenderer] Failed to write temp SVG file";
    }

    // Free Graphviz memory
    gvFreeRenderData(svgData);
    gvFreeLayout(m_gvc, graph);
    agclose(graph);

    // Emit success
    emit renderCompleteInMemory(m_lastSvgContent);
    if (!m_lastSvgPath.isEmpty()) {
        emit renderComplete(m_lastSvgPath);
    }
    
    qDebug() << "[GraphvizRenderer] Render complete, SVG size:" << m_lastSvgContent.size() << "bytes";

#else
    // Fallback: Use dot CLI
    renderToSvgViaCLI(dotSource);
#endif
}

// CLI fallback when libgraphviz is not available
void GraphvizRenderer::renderToSvgViaCLI(const QString& dotSource) {
    // Write DOT source to temp file
    QString inputPath = m_tempDir + "/strategient_input.dot";
    QString outputPath = m_tempDir + "/strategient_graphviz.svg";
    
    QFile inputFile(inputPath);
    if (!inputFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        emit renderError("File error", "Failed to create temporary input file.");
        return;
    }
    inputFile.write(dotSource.toUtf8());
    inputFile.close();
    
    // Run dot command
    QProcess process;
    QString engineName = layoutEngineName();
    QStringList args = {"-Tsvg", "-K" + engineName, "-o", outputPath, inputPath};
    
    process.start("dot", args);
    if (!process.waitForFinished(30000)) {
        emit renderError("Timeout", "Graphviz dot command timed out.");
        return;
    }
    
    if (process.exitCode() != 0) {
        QString errorOutput = QString::fromUtf8(process.readAllStandardError());
        emit renderError("Graphviz error", 
                        QString("dot command failed:\n%1").arg(errorOutput));
        return;
    }
    
    // Read the generated SVG
    QFile outputFile(outputPath);
    if (outputFile.open(QIODevice::ReadOnly)) {
        m_lastSvgContent = outputFile.readAll();
        outputFile.close();
        m_lastSvgPath = outputPath;
        
        emit renderCompleteInMemory(m_lastSvgContent);
        emit renderComplete(m_lastSvgPath);
        
        qDebug() << "[GraphvizRenderer] CLI render complete, SVG size:" << m_lastSvgContent.size() << "bytes";
    } else {
        emit renderError("Read error", "Failed to read generated SVG file.");
    }
}

