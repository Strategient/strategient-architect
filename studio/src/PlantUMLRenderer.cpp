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
    // In development, look relative to the build directory
    // The theme file is at: assets/plantuml/themes/strategient.puml
    
    // Try several possible locations
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
    m_cachedTheme.clear();  // Clear cache to reload
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
    qDebug() << "[PlantUMLRenderer] Loaded theme:" << m_cachedTheme.length() << "chars";
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
    
    // Insert theme right after @startuml line
    QRegularExpression startumlRe("(@startuml[^\\n]*\\n)");
    QRegularExpressionMatch match = startumlRe.match(plantumlSource);
    
    if (match.hasMatch()) {
        QString result = plantumlSource;
        int insertPos = match.capturedEnd(1);
        
        // Add theme content with newlines
        QString themeBlock = "\n' === Strategient Theme (auto-applied) ===\n" + theme + "\n' === End Theme ===\n\n";
        result.insert(insertPos, themeBlock);
        
        return result;
    }
    
    // If no @startuml found, just prepend
    return theme + "\n" + plantumlSource;
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
        qDebug() << "[PlantUMLRenderer] Empty source, skipping";
        emit renderError("Empty PlantUML source");
        return;
    }

    // Clean up previous process safely
    if (m_process) {
        qDebug() << "[PlantUMLRenderer] Cleaning up previous process";
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
    qDebug() << "[PlantUMLRenderer] Themed source length:" << themedSource.length();

    // Create temp file for input
    m_inputFile = new QTemporaryFile(QDir::tempPath() + "/strategient_XXXXXX.puml");
    m_inputFile->setAutoRemove(false);
    
    if (!m_inputFile->open()) {
        qDebug() << "[PlantUMLRenderer] Failed to create temp file";
        emit renderError("Failed to create temporary file");
        return;
    }

    m_inputFile->write(themedSource.toUtf8());
    m_inputFile->flush();
    m_inputFile->close();
    
    QString inputPath = m_inputFile->fileName();
    
    // PlantUML outputs based on @startuml name, not input filename
    QString diagramName = extractDiagramName(plantumlSource);  // Use original source for name
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
    
    // Run: plantuml -tsvg -o /tmp input.puml
    m_process->start("plantuml", {"-tsvg", "-o", QDir::tempPath(), inputPath});
}

void PlantUMLRenderer::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    Q_UNUSED(exitStatus);
    
    if (!m_process) {
        qDebug() << "[PlantUMLRenderer] onProcessFinished called but process is null";
        return;
    }
    
    qDebug() << "[PlantUMLRenderer] Process finished, exit code:" << exitCode;
    
    QString stderrOutput = QString::fromUtf8(m_process->readAllStandardError());
    QString stdoutOutput = QString::fromUtf8(m_process->readAllStandardOutput());
    
    if (exitCode == 0 || exitCode == 200) {
        if (QFileInfo::exists(m_outputPath)) {
            m_lastSvgPath = m_outputPath;
            qDebug() << "[PlantUMLRenderer] SVG created:" << m_outputPath;
            cleanupProcess();
            emit renderComplete(m_lastSvgPath);
        } else {
            // Try to find SVG by scanning /tmp for recent files
            qDebug() << "[PlantUMLRenderer] SVG not at expected path, scanning...";
            QDir tmpDir(QDir::tempPath());
            QStringList svgFiles = tmpDir.entryList({"*.svg"}, QDir::Files, QDir::Time);
            
            QString foundSvg;
            for (const QString& svg : svgFiles) {
                QString fullPath = tmpDir.filePath(svg);
                QFileInfo info(fullPath);
                if (info.lastModified().secsTo(QDateTime::currentDateTime()) < 10) {
                    foundSvg = fullPath;
                    break;
                }
            }
            
            if (!foundSvg.isEmpty()) {
                m_lastSvgPath = foundSvg;
                m_outputPath = foundSvg;
                qDebug() << "[PlantUMLRenderer] Found SVG:" << foundSvg;
                cleanupProcess();
                emit renderComplete(m_lastSvgPath);
            } else {
                QString errorMsg = stderrOutput.isEmpty() ? stdoutOutput : stderrOutput;
                cleanupProcess();
                emit renderError(QString("SVG file was not created. %1").arg(errorMsg.trimmed()));
            }
        }
    } else {
        QString errorMsg = stderrOutput.isEmpty() ? stdoutOutput : stderrOutput;
        qDebug() << "[PlantUMLRenderer] Error:" << errorMsg.trimmed();
        cleanupProcess();
        emit renderError(QString("PlantUML error (exit %1): %2").arg(exitCode).arg(errorMsg.trimmed()));
    }
}

void PlantUMLRenderer::onProcessError(QProcess::ProcessError error) {
    if (!m_process) {
        qDebug() << "[PlantUMLRenderer] onProcessError called but process is null";
        return;
    }
    
    QString errorMsg;
    switch (error) {
        case QProcess::FailedToStart:
            errorMsg = "PlantUML not found. Install with: sudo apt install plantuml";
            break;
        case QProcess::Crashed:
            errorMsg = "PlantUML process crashed";
            break;
        case QProcess::Timedout:
            errorMsg = "PlantUML process timed out";
            break;
        default:
            errorMsg = QString("Process error: %1").arg(error);
            break;
    }
    
    qDebug() << "[PlantUMLRenderer] Process error:" << errorMsg;
    cleanupProcess();
    emit renderError(errorMsg);
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
