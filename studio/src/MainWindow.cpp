#include "MainWindow.hpp"
#include "RunConsole.hpp"
#include "PlantUMLPreview.hpp"
#include "DocumentModel.hpp"
#include "PagesSidebar.hpp"
#include "DiagramPlaceholder.hpp"

#include <QDockWidget>
#include <QLabel>
#include <QMenuBar>
#include <QFileDialog>
#include <QStatusBar>
#include <QMessageBox>

#include "engine.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    // Create document model first
    m_documentModel = new DocumentModel(this);

    setWindowTitle("Strategient Architect");
    resize(1400, 900);

    setupUI();
    setupMenus();

    // Connect document model signals
    connect(m_documentModel, &DocumentModel::documentLoaded,
            this, &MainWindow::onDocumentLoaded);
    connect(m_documentModel, &DocumentModel::dirtyChanged,
            this, &MainWindow::onDirtyChanged);
    connect(m_documentModel, &DocumentModel::currentPageChanged,
            this, &MainWindow::onCurrentPageChanged);

    // Log engine version on startup
    char* version = engine_version();
    logMessage(QString("[engine] %1").arg(version));
    engine_free_string(version);

    updateWindowTitle();
    statusBar()->showMessage("Ready", 3000);
}

void MainWindow::setupUI() {
    // Central widget - DiagramPlaceholder (shows active page)
    m_diagramPlaceholder = new DiagramPlaceholder(m_documentModel, this);
    m_diagramPlaceholder->setMinimumSize(600, 400);
    setCentralWidget(m_diagramPlaceholder);

    // Left dock - Pages sidebar
    auto* pagesDock = new QDockWidget("Pages", this);
    pagesDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    m_pagesSidebar = new PagesSidebar(m_documentModel, this);
    pagesDock->setWidget(m_pagesSidebar);
    addDockWidget(Qt::LeftDockWidgetArea, pagesDock);

    // Left dock - Agent Chat / Prompt Runner (below Pages)
    auto* agentDock = new QDockWidget("Agent Chat", this);
    agentDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    auto* agentPlaceholder = new QLabel("Agent Chat / Prompt Runner\n(placeholder for Cursor CLI)");
    agentPlaceholder->setAlignment(Qt::AlignCenter);
    agentPlaceholder->setMinimumWidth(200);
    agentPlaceholder->setStyleSheet("QLabel { background-color: #252526; color: #808080; }");
    agentDock->setWidget(agentPlaceholder);
    addDockWidget(Qt::LeftDockWidgetArea, agentDock);

    // Right dock - Inspector panel
    auto* inspectorDock = new QDockWidget("Inspector", this);
    inspectorDock->setAllowedAreas(Qt::RightDockWidgetArea | Qt::LeftDockWidgetArea);
    auto* inspectorPlaceholder = new QLabel("Inspector Panel\n(placeholder)");
    inspectorPlaceholder->setAlignment(Qt::AlignCenter);
    inspectorPlaceholder->setMinimumWidth(250);
    inspectorPlaceholder->setStyleSheet("QLabel { background-color: #252526; color: #808080; }");
    inspectorDock->setWidget(inspectorPlaceholder);
    addDockWidget(Qt::RightDockWidgetArea, inspectorDock);

    // Right dock - PlantUML Preview (tabbed with Inspector)
    auto* plantUMLDock = new QDockWidget("PlantUML Preview", this);
    plantUMLDock->setAllowedAreas(Qt::AllDockWidgetAreas);
    m_plantUMLPreview = new PlantUMLPreview(this);
    plantUMLDock->setWidget(m_plantUMLPreview);
    addDockWidget(Qt::RightDockWidgetArea, plantUMLDock);
    tabifyDockWidget(inspectorDock, plantUMLDock);
    inspectorDock->raise();

    // Bottom dock - Run/Log Console
    auto* consoleDock = new QDockWidget("Run Console", this);
    consoleDock->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
    m_runConsole = new RunConsole(this);
    consoleDock->setWidget(m_runConsole);
    addDockWidget(Qt::BottomDockWidgetArea, consoleDock);
}

void MainWindow::setupMenus() {
    // File menu
    auto* fileMenu = menuBar()->addMenu("&File");

    // Open Project
    auto* openProjectAction = fileMenu->addAction("&Open Project...");
    openProjectAction->setShortcut(QKeySequence::Open);
    connect(openProjectAction, &QAction::triggered, this, &MainWindow::onOpenProject);

    // Save Project
    auto* saveProjectAction = fileMenu->addAction("&Save Project");
    saveProjectAction->setShortcut(QKeySequence::Save);
    connect(saveProjectAction, &QAction::triggered, this, &MainWindow::onSaveProject);

    // Save Project As
    auto* saveAsAction = fileMenu->addAction("Save Project &As...");
    saveAsAction->setShortcut(QKeySequence::SaveAs);
    connect(saveAsAction, &QAction::triggered, this, &MainWindow::onSaveProjectAs);

    fileMenu->addSeparator();

    // Open SVG (for PlantUML preview)
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

    // Exit
    auto* exitAction = fileMenu->addAction("E&xit");
    exitAction->setShortcut(QKeySequence::Quit);
    connect(exitAction, &QAction::triggered, this, &QMainWindow::close);

    // View menu
    auto* viewMenu = menuBar()->addMenu("&View");
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

void MainWindow::onOpenProject() {
    QString filePath = QFileDialog::getOpenFileName(
        this, "Open Project", QString(),
        "Strategient Architect Projects (*.sarch.json);;All Files (*)");
    
    if (filePath.isEmpty()) {
        return;
    }

    if (m_documentModel->loadFromFile(filePath)) {
        logMessage(QString("[project] Opened: %1").arg(filePath));
        statusBar()->showMessage(QString("Opened %1").arg(filePath), 5000);
    } else {
        QMessageBox::warning(this, "Open Failed",
            QString("Failed to open project:\n%1").arg(filePath));
        logMessage(QString("[project] Failed to open: %1").arg(filePath));
    }
}

void MainWindow::onSaveProject() {
    if (m_documentModel->currentFilePath().isEmpty()) {
        onSaveProjectAs();
        return;
    }

    if (m_documentModel->save()) {
        logMessage(QString("[project] Saved: %1").arg(m_documentModel->currentFilePath()));
        statusBar()->showMessage("Saved", 3000);
    } else {
        QMessageBox::warning(this, "Save Failed", "Failed to save project.");
    }
}

void MainWindow::onSaveProjectAs() {
    QString filePath = QFileDialog::getSaveFileName(
        this, "Save Project As", QString(),
        "Strategient Architect Projects (*.sarch.json);;All Files (*)");
    
    if (filePath.isEmpty()) {
        return;
    }

    // Ensure .sarch.json extension
    if (!filePath.endsWith(".sarch.json")) {
        filePath += ".sarch.json";
    }

    if (m_documentModel->saveToFile(filePath)) {
        logMessage(QString("[project] Saved: %1").arg(filePath));
        statusBar()->showMessage(QString("Saved to %1").arg(filePath), 5000);
        updateWindowTitle();
    } else {
        QMessageBox::warning(this, "Save Failed",
            QString("Failed to save project:\n%1").arg(filePath));
    }
}

void MainWindow::onDocumentLoaded() {
    updateWindowTitle();
    logMessage(QString("[project] Loaded: %1 (%2 pages)")
        .arg(m_documentModel->documentTitle())
        .arg(m_documentModel->pageCount()));
}

void MainWindow::onDirtyChanged(bool dirty) {
    Q_UNUSED(dirty);
    updateWindowTitle();
}

void MainWindow::onCurrentPageChanged(const QString& pageId) {
    const auto* page = m_documentModel->currentPage();
    if (page) {
        logMessage(QString("[page] Switched to: %1").arg(page->title));
    }
    Q_UNUSED(pageId);
}

void MainWindow::logMessage(const QString& message) {
    if (m_runConsole) {
        m_runConsole->appendLog(message);
    }
}

void MainWindow::updateWindowTitle() {
    QString title = "Strategient Architect";
    
    if (m_documentModel->hasDocument()) {
        title = QString("%1 - %2").arg(m_documentModel->documentTitle(), title);
        if (m_documentModel->isDirty()) {
            title = "* " + title;
        }
    }
    
    setWindowTitle(title);
}
