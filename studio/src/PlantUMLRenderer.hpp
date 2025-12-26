#ifndef PLANTUMLRENDERER_HPP
#define PLANTUMLRENDERER_HPP

#include <QObject>
#include <QString>
#include <QProcess>
#include <QTemporaryFile>

/**
 * PlantUMLRenderer handles rendering PlantUML source to SVG.
 * Automatically applies the Strategient theme for consistent styling.
 * Provides detailed error reporting for diagnostics.
 */
class PlantUMLRenderer : public QObject {
    Q_OBJECT

public:
    explicit PlantUMLRenderer(QObject* parent = nullptr);
    ~PlantUMLRenderer() override;

    // Render PlantUML source to SVG (theme is auto-applied)
    void renderToSvg(const QString& plantumlSource);
    
    // Get the last rendered SVG file path
    QString lastSvgPath() const { return m_lastSvgPath; }
    
    // Check if PlantUML is available
    static bool isPlantUMLAvailable();
    
    // Theme management
    void setThemePath(const QString& path);
    QString themePath() const { return m_themePath; }
    bool isThemeEnabled() const { return m_themeEnabled; }
    void setThemeEnabled(bool enabled) { m_themeEnabled = enabled; }

signals:
    void renderStarted();
    void renderComplete(const QString& svgPath);
    
    // Error signal with title and detailed message
    void renderError(const QString& errorTitle, const QString& errorDetails);

private slots:
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onProcessError(QProcess::ProcessError error);

private:
    void cleanupProcess();
    QString loadTheme();
    QString applyTheme(const QString& plantumlSource);
    QString validateSource(const QString& source);
    
    QProcess* m_process{nullptr};
    QTemporaryFile* m_inputFile{nullptr};
    QString m_outputPath;
    QString m_lastSvgPath;
    
    // Theme settings
    QString m_themePath;
    QString m_cachedTheme;
    bool m_themeEnabled{true};
};

#endif // PLANTUMLRENDERER_HPP
