#include "PlantUMLRenderer.hpp"

#include <QDir>
#include <QFileInfo>
#include <QRegularExpression>
#include <QCoreApplication>
#include <QDebug>

PlantUMLRenderer::PlantUMLRenderer(QObject* parent)
    : QObject(parent)
{
    // Set default theme path relative to the application
    QStringList searchPaths = {
        QCoreApplication::applicationDirPath() + "/../assets/plantuml/themes/strategient.puml",
        QCoreApplication::applicationDirPath() + "/../../assets/plantuml/themes/strategient.puml",
        QDir::homePath() + "/projects/strategient-architect/assets/plantuml/themes/strategient.puml",
        "./assets/plantuml/themes/strategient.puml"
    };
    
    for (const QString& path : searchPaths) {
        if (QFileInfo::exists(path)) {
            m_themePath = QFileInfo(path).absoluteFilePath();
            qDebug() << "[PlantUMLRenderer] Found theme at:" << m_themePath;
            break;
        }
    }
    
    if (m_themePath.isEmpty()) {
        qWarning() << "[PlantUMLRenderer] Theme file not found, styling will be default";
    }
}

PlantUMLRenderer::~PlantUMLRenderer() {
    if (m_process) {
        m_process->disconnect();
        m_process->kill();
        m_process->waitForFinished(1000);
        delete m_process;
    }
    if (m_inputFile) {
        QString tempPath = m_inputFile->fileName();
        delete m_inputFile;
        QFile::remove(tempPath);
    }
}

bool PlantUMLRenderer::isPlantUMLAvailable() {
    QProcess check;
    check.start("which", {"plantuml"});
    check.waitForFinished(3000);
    return check.exitCode() == 0;
}

void PlantUMLRenderer::setThemePath(const QString& path) {
    m_themePath = path;
    m_cachedTheme.clear();
}

QString PlantUMLRenderer::loadTheme() {
    if (!m_cachedTheme.isEmpty()) {
        return m_cachedTheme;
    }
    
    if (m_themePath.isEmpty() || !QFileInfo::exists(m_themePath)) {
        return QString();
    }
    
    QFile file(m_themePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "[PlantUMLRenderer] Failed to open theme file:" << m_themePath;
        return QString();
    }
    
    m_cachedTheme = QString::fromUtf8(file.readAll());
    return m_cachedTheme;
}

QString PlantUMLRenderer::applyTheme(const QString& plantumlSource) {
    if (!m_themeEnabled) {
        return plantumlSource;
    }
    
    QString theme = loadTheme();
    if (theme.isEmpty()) {
        return plantumlSource;
    }
    
    QRegularExpression startumlRe("(@startuml[^\\n]*\\n)");
    QRegularExpressionMatch match = startumlRe.match(plantumlSource);
    
    if (match.hasMatch()) {
        QString result = plantumlSource;
        int insertPos = match.capturedEnd(1);
        QString themeBlock = "\n' === Strategient Theme (auto-applied) ===\n" + theme + "\n' === End Theme ===\n\n";
        result.insert(insertPos, themeBlock);
        return result;
    }
    
    return theme + "\n" + plantumlSource;
}

// Validate PlantUML source for common errors
QString PlantUMLRenderer::validateSource(const QString& source) {
    QString trimmed = source.trimmed();
    
    // Check for @startuml
    if (!trimmed.contains("@startuml")) {
        return "Missing @startuml directive. PlantUML diagrams must start with @startuml";
    }
    
    // Check for @enduml
    if (!trimmed.contains("@enduml")) {
        return "Missing @enduml directive. PlantUML diagrams must end with @enduml";
    }
    
    // Check that @enduml comes after @startuml
    int startPos = trimmed.indexOf("@startuml");
    int endPos = trimmed.indexOf("@enduml");
    if (endPos < startPos) {
        return "@enduml appears before @startuml. Check diagram structure.";
    }
    
    return QString(); // No validation errors
}

// Extract diagram name from @startuml <name>
static QString extractDiagramName(const QString& source) {
    QRegularExpression re("@startuml\\s+(\\w+)");
    QRegularExpressionMatch match = re.match(source);
    if (match.hasMatch()) {
        return match.captured(1);
    }
    return QString();
}

void PlantUMLRenderer::renderToSvg(const QString& plantumlSource) {
    qDebug() << "[PlantUMLRenderer] renderToSvg called, source length:" << plantumlSource.length();
    
    if (plantumlSource.trimmed().isEmpty()) {
        emit renderError("Empty PlantUML source", "No PlantUML content to render.");
        return;
    }

    // Validate source before attempting render
    QString validationError = validateSource(plantumlSource);
    if (!validationError.isEmpty()) {
        emit renderError("PlantUML validation failed", validationError);
        return;
    }

    // Clean up previous process safely
    if (m_process) {
        m_process->disconnect();
        m_process->kill();
        m_process->waitForFinished(500);
        m_process->deleteLater();
        m_process = nullptr;
    }
    
    if (m_inputFile) {
        QString tempPath = m_inputFile->fileName();
        delete m_inputFile;
        m_inputFile = nullptr;
        QFile::remove(tempPath);
    }

    // Apply theme to the source
    QString themedSource = applyTheme(plantumlSource);

    // Create temp file for input
    m_inputFile = new QTemporaryFile(QDir::tempPath() + "/strategient_XXXXXX.puml");
    m_inputFile->setAutoRemove(false);
    
    if (!m_inputFile->open()) {
        emit renderError("File system error", "Failed to create temporary file for PlantUML input.");
        return;
    }

    m_inputFile->write(themedSource.toUtf8());
    m_inputFile->flush();
    m_inputFile->close();
    
    QString inputPath = m_inputFile->fileName();
    
    // Determine output path based on diagram name
    QString diagramName = extractDiagramName(plantumlSource);
    if (!diagramName.isEmpty()) {
        m_outputPath = QDir::tempPath() + "/" + diagramName + ".svg";
    } else {
        m_outputPath = inputPath;
        m_outputPath.replace(".puml", ".svg");
    }

    qDebug() << "[PlantUMLRenderer] Input:" << inputPath;
    qDebug() << "[PlantUMLRenderer] Expected output:" << m_outputPath;

    // Remove old output if exists
    QFile::remove(m_outputPath);

    // Start PlantUML process
    m_process = new QProcess(this);
    
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &PlantUMLRenderer::onProcessFinished);
    connect(m_process, &QProcess::errorOccurred,
            this, &PlantUMLRenderer::onProcessError);

    emit renderStarted();
    
    m_process->start("plantuml", {"-tsvg", "-o", QDir::tempPath(), inputPath});
}

void PlantUMLRenderer::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    Q_UNUSED(exitStatus);
    
    if (!m_process) {
        return;
    }
    
    qDebug() << "[PlantUMLRenderer] Process finished, exit code:" << exitCode;
    
    // Capture both stdout and stderr
    QString stderrOutput = QString::fromUtf8(m_process->readAllStandardError()).trimmed();
    QString stdoutOutput = QString::fromUtf8(m_process->readAllStandardOutput()).trimmed();
    
    // Combine outputs for logging
    QString fullOutput;
    if (!stdoutOutput.isEmpty()) {
        fullOutput += "stdout: " + stdoutOutput;
    }
    if (!stderrOutput.isEmpty()) {
        if (!fullOutput.isEmpty()) fullOutput += "\n";
        fullOutput += "stderr: " + stderrOutput;
    }
    
    // Log all output for diagnostics
    if (!fullOutput.isEmpty()) {
        qDebug() << "[PlantUMLRenderer] Process output:\n" << fullOutput;
    }

    // Check if SVG was created
    bool svgExists = QFileInfo::exists(m_outputPath);
    
    if (!svgExists) {
        // Try to find SVG by scanning /tmp for recent files
        QDir tmpDir(QDir::tempPath());
        QStringList svgFiles = tmpDir.entryList({"*.svg"}, QDir::Files, QDir::Time);
        
        for (const QString& svg : svgFiles) {
            QString fullPath = tmpDir.filePath(svg);
            QFileInfo info(fullPath);
            if (info.lastModified().secsTo(QDateTime::currentDateTime()) < 10) {
                m_outputPath = fullPath;
                svgExists = true;
                break;
            }
        }
    }

    if (svgExists) {
        m_lastSvgPath = m_outputPath;
        qDebug() << "[PlantUMLRenderer] SVG created:" << m_outputPath;
        cleanupProcess();
        emit renderComplete(m_lastSvgPath);
    } else {
        // Build detailed error message
        QString errorTitle;
        QString errorDetails;
        
        if (exitCode == 0) {
            errorTitle = "SVG not generated";
            errorDetails = "PlantUML completed but no SVG file was created.";
        } else if (exitCode == 200) {
            errorTitle = "PlantUML syntax error";
            errorDetails = "The diagram contains errors.";
        } else {
            errorTitle = QString("PlantUML failed (exit code %1)").arg(exitCode);
            errorDetails = "PlantUML encountered an error during rendering.";
        }
        
        // Append process output to details
        if (!stderrOutput.isEmpty()) {
            // Parse PlantUML error output for line numbers
            QRegularExpression lineErrorRe("Error line (\\d+)");
            QRegularExpressionMatch match = lineErrorRe.match(stderrOutput);
            if (match.hasMatch()) {
                errorDetails += QString("\n\nError at line %1").arg(match.captured(1));
            }
            errorDetails += "\n\n" + stderrOutput;
        } else if (!stdoutOutput.isEmpty()) {
            errorDetails += "\n\n" + stdoutOutput;
        }
        
        cleanupProcess();
        emit renderError(errorTitle, errorDetails);
    }
}

void PlantUMLRenderer::onProcessError(QProcess::ProcessError error) {
    if (!m_process) {
        return;
    }
    
    QString errorTitle;
    QString errorDetails;
    
    switch (error) {
        case QProcess::FailedToStart:
            errorTitle = "PlantUML not found";
            errorDetails = "The PlantUML command could not be started.\n\n"
                          "Install PlantUML with:\n"
                          "  sudo apt install plantuml";
            break;
        case QProcess::Crashed:
            errorTitle = "PlantUML crashed";
            errorDetails = "The PlantUML process terminated unexpectedly.";
            break;
        case QProcess::Timedout:
            errorTitle = "PlantUML timeout";
            errorDetails = "The PlantUML process took too long to respond.";
            break;
        case QProcess::ReadError:
            errorTitle = "Read error";
            errorDetails = "Failed to read PlantUML output.";
            break;
        case QProcess::WriteError:
            errorTitle = "Write error";
            errorDetails = "Failed to write to PlantUML process.";
            break;
        default:
            errorTitle = "Process error";
            errorDetails = QString("An unknown error occurred (code %1).").arg(static_cast<int>(error));
            break;
    }
    
    qDebug() << "[PlantUMLRenderer] Process error:" << errorTitle << "-" << errorDetails;
    cleanupProcess();
    emit renderError(errorTitle, errorDetails);
}

void PlantUMLRenderer::cleanupProcess() {
    if (m_inputFile) {
        QString tempPath = m_inputFile->fileName();
        delete m_inputFile;
        m_inputFile = nullptr;
        QFile::remove(tempPath);
    }
    
    if (m_process) {
        m_process->disconnect();
        m_process->deleteLater();
        m_process = nullptr;
    }
}
