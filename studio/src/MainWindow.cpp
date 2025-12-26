#include "MainWindow.hpp"
#include "RunConsole.hpp"
#include "PlantUMLPreview.hpp"

#include <QDockWidget>
#include <QLabel>
#include <QMenuBar>
#include <QFileDialog>
#include <QStatusBar>

#include "engine.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle("Strategient Architect");
    resize(1400, 900);

    setupUI();
    setupMenus();

    // Log engine version on startup
    char* version = engine_version();
    logMessage(QString("[engine] %1").arg(version));
    engine_free_string(version);

    statusBar()->showMessage("Ready", 3000);
}

void MainWindow::setupUI() {
    // Central widget - placeholder for DiagramView (later sessions)
    auto* centralWidget = new QLabel("Diagram View (placeholder)");
    centralWidget->setAlignment(Qt::AlignCenter);
    centralWidget->setMinimumSize(600, 400);
    centralWidget->setStyleSheet(
        "QLabel { background-color: #1e1e1e; color: #808080; "
        "border: 1px solid #333; font-size: 14px; }");
    setCentralWidget(centralWidget);

    // Right dock - Inspector panel
    auto* inspectorDock = new QDockWidget("Inspector", this);
    inspectorDock->setAllowedAreas(Qt::RightDockWidgetArea | Qt::LeftDockWidgetArea);
    auto* inspectorPlaceholder = new QLabel("Inspector Panel\n(placeholder)");
    inspectorPlaceholder->setAlignment(Qt::AlignCenter);
    inspectorPlaceholder->setMinimumWidth(250);
    inspectorPlaceholder->setStyleSheet("QLabel { background-color: #252526; color: #808080; }");
    inspectorDock->setWidget(inspectorPlaceholder);
    addDockWidget(Qt::RightDockWidgetArea, inspectorDock);

    // Bottom dock - Run/Log Console
    auto* consoleDock = new QDockWidget("Run Console", this);
    consoleDock->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
    m_runConsole = new RunConsole(this);
    consoleDock->setWidget(m_runConsole);
    addDockWidget(Qt::BottomDockWidgetArea, consoleDock);

    // Left dock - Agent Chat / Prompt Runner
    auto* agentDock = new QDockWidget("Agent Chat", this);
    agentDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    auto* agentPlaceholder = new QLabel("Agent Chat / Prompt Runner\n(placeholder for Cursor CLI)");
    agentPlaceholder->setAlignment(Qt::AlignCenter);
    agentPlaceholder->setMinimumWidth(280);
    agentPlaceholder->setStyleSheet("QLabel { background-color: #252526; color: #808080; }");
    agentDock->setWidget(agentPlaceholder);
    addDockWidget(Qt::LeftDockWidgetArea, agentDock);

    // Optional dock - PlantUML Preview
    auto* plantUMLDock = new QDockWidget("PlantUML Preview", this);
    plantUMLDock->setAllowedAreas(Qt::AllDockWidgetAreas);
    m_plantUMLPreview = new PlantUMLPreview(this);
    plantUMLDock->setWidget(m_plantUMLPreview);
    addDockWidget(Qt::RightDockWidgetArea, plantUMLDock);

    // Stack PlantUML preview under Inspector
    tabifyDockWidget(inspectorDock, plantUMLDock);
    inspectorDock->raise();
}

void MainWindow::setupMenus() {
    // File menu
    auto* fileMenu = menuBar()->addMenu("&File");
    
    auto* openSvgAction = fileMenu->addAction("Open SVG...");
    connect(openSvgAction, &QAction::triggered, this, [this]() {
        QString filePath = QFileDialog::getOpenFileName(
            this, "Open SVG", QString(), "SVG Files (*.svg)");
        if (!filePath.isEmpty()) {
            m_plantUMLPreview->loadSvg(filePath);
            logMessage(QString("[ui] Loaded SVG: %1").arg(filePath));
        }
    });

    fileMenu->addSeparator();
    
    auto* exitAction = fileMenu->addAction("E&xit");
    exitAction->setShortcut(QKeySequence::Quit);
    connect(exitAction, &QAction::triggered, this, &QMainWindow::close);

    // View menu
    auto* viewMenu = menuBar()->addMenu("&View");
    
    // Add dock visibility toggles (Qt handles this automatically via dockWidget->toggleViewAction())
    for (auto* dock : findChildren<QDockWidget*>()) {
        viewMenu->addAction(dock->toggleViewAction());
    }

    // Help menu
    auto* helpMenu = menuBar()->addMenu("&Help");
    auto* aboutAction = helpMenu->addAction("About");
    connect(aboutAction, &QAction::triggered, this, [this]() {
        char* version = engine_version();
        logMessage(QString("[help] Strategient Architect v0.1.0, Engine: %1").arg(version));
        engine_free_string(version);
    });
}

void MainWindow::logMessage(const QString& message) {
    if (m_runConsole) {
        m_runConsole->appendLog(message);
    }
}

