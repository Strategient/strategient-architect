cmake_minimum_required(VERSION 3.20)
project(StrategientArchitect VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS Widgets Svg SvgWidgets WebEngineWidgets WebChannel)

# Source files
set(SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/RunConsole.cpp
    src/PlantUMLPreview.cpp
    src/ProjectModel.cpp
    src/DocumentModel.cpp
    src/PagesSidebar.cpp
    src/DiagramPlaceholder.cpp
    src/DiagramView.cpp
    src/MonacoBridge.cpp
    src/MonacoEditorWidget.cpp
    src/PlantUMLRenderer.cpp
)

set(HEADERS
    src/MainWindow.hpp
    src/RunConsole.hpp
    src/PlantUMLPreview.hpp
    src/ProjectModel.hpp
    src/DocumentModel.hpp
    src/PagesSidebar.hpp
    src/DiagramPlaceholder.hpp
    src/DiagramView.hpp
    src/MonacoBridge.hpp
    src/MonacoEditorWidget.hpp
    src/PlantUMLRenderer.hpp
)

# Qt resources
set(RESOURCES
    resources.qrc
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${RESOURCES})

# Include engine header
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../engine/include
)

# Link Qt6
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Widgets
    Qt6::Svg
    Qt6::SvgWidgets
    Qt6::WebEngineWidgets
    Qt6::WebChannel
)

# Link Rust engine static library
set(ENGINE_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../engine/target/release)
target_link_libraries(${PROJECT_NAME} PRIVATE
    ${ENGINE_LIB_DIR}/libengine.a
    pthread
    dl
    m
)

# Ensure engine is built first (handled by build_all.sh, but good practice)
add_custom_target(engine_build
    COMMAND cargo build --release
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../engine
    COMMENT "Building Rust engine..."
)
