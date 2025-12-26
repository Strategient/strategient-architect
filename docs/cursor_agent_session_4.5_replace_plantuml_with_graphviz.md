Cursor Prompt: Migrate Qt Application to Graphviz & DOT Editor

Goal: Refactor the strategient-architect Qt desktop application to replace the PlantUML diagram rendering engine with the native Graphviz library for superior, complex system architecture visualization. Simultaneously, enhance the existing embedded Monaco editor to support DOT language syntax highlighting and auto-completion.

Part 1: C++ Backend - Integrating the Graphviz Library (libgvc)

Please provide the necessary code modifications and setup instructions to transition the backend from using PlantUML executables to calling the native Graphviz C libraries (libgvc, libcgraph, etc.). The application uses standard Qt Widgets/Graphics View Framework (e.g., QGV or a custom implementation as suggested by the user's previous queries).

Specific Tasks:
    1. CMake/QMake Integration: Detail the required CMakeLists.txt or .pro file changes to link against the Graphviz libraries on Ubuntu (e.g., sudo apt-get install graphviz-lib libgraphviz-dev).
    2. Rendering Function: Provide a C++ function that accepts a DOT language string, uses the gvContext(), agread(), gvLayout(), and gvRender() APIs to generate a diagram image (preferably SVG for quality and scalability within Qt's QGraphicsView), and loads it into the QGraphicsScene or a similar Qt display mechanism.
    3. Engine Selection: Demonstrate how to dynamically select different layout engines (e.g., dot, neato, fdp) based on user input, which is a key advantage over PlantUML's limited options.
    4. Error Handling: Implement basic error checking for invalid DOT syntax during parsing using the Graphviz APIs.

Part 2: Frontend - Configuring the Monaco Editor for DOT Language
The existing application uses a Monaco editor instance in the bottom pane (like the screenshot provided) for text input. The goal is to switch this editor's language mode from PlantUML to the official DOT language syntax and provide rich editing features.
Specific Tasks:

    1. Syntax Definition: Outline the JavaScript/TypeScript code required to register the DOT language within the embedded Monaco editor instance, enabling correct syntax highlighting for keywords, attributes, node/edge definitions, and comments.
    2. Basic Auto-Complete (Snippets): Provide a set of configuration examples for a simple auto-completion provider that suggests common Graphviz keywords/shapes, such as digraph, subgraph, node, edge, shape=cylinder, splines=ortho, etc.

Part 3: Suggest an Open-Source Shape Library
Suggest a library of open-source, free-to-use shapes and icons that are suitable for system architecture diagrams (e.g., AWS, Kubernetes, Azure icons) that can be easily incorporated into the generated Graphviz diagrams using the image attribute in DOT files. The shapes should be compatible with an MIT or similar permissive license.
