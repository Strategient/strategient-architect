
GOAL
Add a Monaco Editor for editing PlantUML that:
1) Uses Monaco Editor embedded via Qt WebEngine (no native port attempts).
2) Is docked as a tab in the LOWER pane, next to the existing “Run Console”.
3) Provides syntax highlighting and autocomplete for PlantUML.
4) Edits the active page’s PlantUML text.
5) Automatically re-renders the PlantUML Preview in the MAIN CENTRAL VIEW when edited.
6) Keeps IR authoritative (editor edits update page payload, not bypass it).

STRICT CONSTRAINTS
- Use Qt6 Widgets + Qt WebEngine + Qt WebChannel.
- Do NOT replace the existing PlantUML preview widget.
- Do NOT move the preview into the editor pane.
- Do NOT make Monaco the source of truth.
- Do NOT introduce React, Electron, or external runtimes.
- Monaco must load from local static assets (no CDN dependency).

ARCHITECTURE OVERVIEW
- Central View:
  - Make this the interactive PlantUML Preview (SVG + hotspots).
- Bottom Pane (QTabWidget):
  - Tab 1: Run Console (existing)
  - Tab 2: “PlantUML Editor” (new Monaco editor)
- Editing flow:
  Monaco → Page Payload["plantuml"] → PlantUML Render Service → Central Preview

TASKS

1) Add Qt WebEngine support
   - Ensure Qt::WebEngineWidgets is enabled in CMake.
   - Add required initialization in main().

2) Create MonacoEditorWidget (C++)
   Location:
     ui/editor/MonacoEditorWidget.{h,cpp}

   Responsibilities:
   - Wrap QWebEngineView
   - Load editor.html from assets/monaco/
   - Expose methods:
       void setText(const QString&)
       QString text() const
       void setLanguage(const QString&)  // “plantuml”
       void setReadOnly(bool)
   - Emit signal:
       textChanged(QString newText)

3) Create WebChannel bridge
   - Expose a C++ QObject to JS:
       MonacoBridge
   - Methods:
       setText(QString)
       notifyTextChanged(QString)
   - JS must call notifyTextChanged on editor content change.

4) Add Monaco static assets
   Location:
     assets/monaco/

   Include:
   - editor.html
   - Monaco loader + JS
   - PlantUML language registration:
       - keywords
       - comments
       - strings
       - @startuml / @enduml
   - Basic autocomplete for:
       rectangle, component, node, database,
       skinparam, title, package

5) Dock Monaco Editor in LOWER pane
   - Locate existing bottom pane QTabWidget that hosts “Run Console”.
   - Add a second tab:
       Title: “PlantUML Editor”
       Widget: MonacoEditorWidget
   - Editor must be hidden/disabled when active page is NOT a PlantUML-capable page.
   - Also -  move the 'Inspector' tab to the lower pane as one of the 3 tabs with the run console and editor tab.

6) Wire editor to Page model
   - When active page changes:
       - If page.type supports PlantUML:
           - Load payload["plantuml"] into editor.
           - Enable editor.
       - Else:
           - Disable editor.
   - On Monaco textChanged:
       - Update page.payload["plantuml"]
       - Trigger PlantUML re-render
       - Update central preview SVG + hotspots

7) Ensure no circular updates
   - Updating preview must NOT re-trigger editor changes.
   - Use explicit “source of change” guards.

8) Visual behavior
   - Dark theme (match app theme).
   - No scrollbars fighting Qt layout.
   - Editor fills its tab completely.

DELIVERABLES
- Build succeeds.
- Editing PlantUML in Monaco updates the central preview live.
- Syntax highlighting works.
- Autocomplete works for basic PlantUML constructs.
- Run Console remains unchanged.
- No architectural shortcuts or bypasses.

DO NOT:
- Re-architect page system.
- Store editor state outside page payload.
- Embed Monaco anywhere except the lower tab pane.

END.
