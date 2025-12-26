#include "MainWindow.hpp"
#include "RunConsole.hpp"
#include "PlantUMLPreview.hpp"
#include "DocumentModel.hpp"
#include "PagesSidebar.hpp"
#include "DiagramView.hpp"
#include "MonacoEditorWidget.hpp"

#include <QDockWidget>
#include <QLabel>
#include <QMenuBar>
#include <QFileDialog>
#include <QStatusBar>
#include <QMessageBox>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QIcon>

#include "engine.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    // Create document model first
    m_documentModel = new DocumentModel(this);

    setWindowTitle("Strategient Architect");
    
    // Set window icon (also set via QApplication, but some WMs need it here too)
    QIcon windowIcon;
    windowIcon.addFile(":/icons/strategient-16.png", QSize(16, 16));
    windowIcon.addFile(":/icons/strategient-32.png", QSize(32, 32));
    windowIcon.addFile(":/icons/strategient-48.png", QSize(48, 48));
    windowIcon.addFile(":/icons/strategient-64.png", QSize(64, 64));
    setWindowIcon(windowIcon);
    
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
    // Central widget - DiagramView (shows rendered PlantUML)
    m_diagramView = new DiagramView(m_documentModel, this);
    m_diagramView->setMinimumSize(600, 400);
    setCentralWidget(m_diagramView);

    // Connect diagram view signals
    connect(m_diagramView, &DiagramView::renderStarted, this, [this]() {
        statusBar()->showMessage("Rendering Graphviz...");
    });
    connect(m_diagramView, &DiagramView::renderComplete, this, [this]() {
        statusBar()->showMessage("Render complete", 3000);
    });
    connect(m_diagramView, &DiagramView::renderFailed, this, 
            [this](const QString& title, const QString& details) {
        // Log full error to Run Console
        logMessage(QString("[render] ⚠️ %1").arg(title));
        logMessage(QString("[render] %1").arg(details));
        statusBar()->showMessage("Render failed - see Run Console", 5000);
    });

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

    // Right dock - PlantUML Preview (SVG file viewer)
    auto* plantUMLDock = new QDockWidget("SVG Preview", this);
    plantUMLDock->setAllowedAreas(Qt::AllDockWidgetAreas);
    m_plantUMLPreview = new PlantUMLPreview(this);
    plantUMLDock->setWidget(m_plantUMLPreview);
    addDockWidget(Qt::RightDockWidgetArea, plantUMLDock);

    // Bottom dock - Tab widget with Run Console, PlantUML Editor, Inspector
    m_bottomDock = new QDockWidget("Tools", this);
    m_bottomDock->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
    m_bottomDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);

    m_bottomTabs = new QTabWidget(this);
    m_bottomTabs->setTabPosition(QTabWidget::South);
    m_bottomTabs->setStyleSheet(
        "QTabWidget::pane { "
        "  border: none; "
        "  background-color: #1e1e1e; "
        "} "
        "QTabBar::tab { "
        "  background-color: #2d2d30; "
        "  color: #808080; "
        "  padding: 8px 16px; "
        "  border: none; "
        "  border-right: 1px solid #3e3e42; "
        "} "
        "QTabBar::tab:selected { "
        "  background-color: #1e1e1e; "
        "  color: #ffffff; "
        "} "
        "QTabBar::tab:hover:!selected { "
        "  background-color: #3e3e42; "
        "}");

    // Tab 1: Run Console
    m_runConsole = new RunConsole(this);
    m_bottomTabs->addTab(m_runConsole, "Run Console");

    // Tab 2: DOT/Graphviz Editor (Monaco)
    m_monacoEditor = new MonacoEditorWidget(this);
    m_bottomTabs->addTab(m_monacoEditor, "DOT Editor");

    // Connect Monaco editor signals
    connect(m_monacoEditor, &MonacoEditorWidget::textChanged,
            this, &MainWindow::onEditorTextChanged);
    connect(m_monacoEditor, &MonacoEditorWidget::editorReady,
            this, &MainWindow::onEditorReady);

    // Tab 3: Inspector
    m_inspectorWidget = new QWidget(this);
    auto* inspectorLayout = new QVBoxLayout(m_inspectorWidget);
    auto* inspectorPlaceholder = new QLabel("Inspector Panel\n\nSelect a node in the diagram\nto view its properties.");
    inspectorPlaceholder->setAlignment(Qt::AlignCenter);
    inspectorPlaceholder->setStyleSheet("QLabel { background-color: #252526; color: #808080; }");
    inspectorLayout->addWidget(inspectorPlaceholder);
    m_bottomTabs->addTab(m_inspectorWidget, "Inspector");

    m_bottomDock->setWidget(m_bottomTabs);
    addDockWidget(Qt::BottomDockWidgetArea, m_bottomDock);

    // Set minimum height for bottom dock
    m_bottomDock->setMinimumHeight(200);
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
    updateEditorForCurrentPage();
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
    updateEditorForCurrentPage();
    Q_UNUSED(pageId);
}

void MainWindow::onEditorTextChanged(const QString& newText) {
    // Guard against circular updates
    if (m_updatingFromModel) {
        return;
    }

    // Update the page's Graphviz DOT source in the model
    auto* page = m_documentModel->currentPage();
    if (page && page->graphviz != newText) {
        page->graphviz = newText;
        m_documentModel->setDirty(true);
        
        // Trigger re-render of the diagram view
        m_diagramView->renderGraphviz(newText);
    }
}

void MainWindow::onEditorReady() {
    logMessage("[editor] Monaco editor ready");
    updateEditorForCurrentPage();
}

void MainWindow::updateEditorForCurrentPage() {
    // Safety check - editor might not be ready yet
    if (!m_monacoEditor) {
        return;
    }
    
    const auto* page = m_documentModel->currentPage();
    
    m_updatingFromModel = true;
    
    if (page) {
        m_monacoEditor->setReadOnly(false);
        m_monacoEditor->setText(page->graphviz);
    } else {
        m_monacoEditor->setText("");
        m_monacoEditor->setReadOnly(true);
    }
    
    m_updatingFromModel = false;
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
