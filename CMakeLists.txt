cmake_minimum_required(VERSION 3.16)
project(QtGraph VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

set(SOURCES
    # App entrypoint
    src/main.cpp
    resources/app.qrc

    # Core
    src/core/filemanager.cpp
    src/core/fonts/fontmanager.cpp

    # Settings
    src/settings/settingsmanager.cpp
    src/settings/settingstab.cpp

    # Editor view
    src/editor/view/codeviewer.cpp

    # UI
    src/ui/mainwindow/mainwindow.cpp
    src/ui/tabs/tabmanager.cpp
    src/ui/tree/projecttreeview.cpp
    src/ui/canvas/canvasview.cpp
    src/ui/welcome/welcometab.cpp

    # Syntax highlighting core
    src/editor/syntax/base/syntaxhighlighter.cpp
    src/editor/syntax/highlighterfactory.cpp

    # Syntax highlighting languages
    src/editor/syntax/languages/cpp/cpphighlighter.cpp
    src/editor/syntax/languages/python/pythonhighlighter.cpp
    src/editor/syntax/languages/java/javahighlighter.cpp
    src/editor/syntax/languages/javascript/jshighlighter.cpp
    src/editor/syntax/languages/typescript/tshighlighter.cpp
    src/editor/syntax/languages/html/htmlhighlighter.cpp
    src/editor/syntax/languages/css/csshighlighter.cpp
    src/editor/syntax/languages/json/jsonhighlighter.cpp
)
set(HEADERS
    # Core
    src/core/filemanager.h
    src/core/fonts/fontmanager.h

    # Settings
    src/settings/appsettings.h
    src/settings/settingsmanager.h
    src/settings/settingstab.h

    # Editor view
    src/editor/view/codeviewer.h

    # UI
    src/ui/mainwindow/mainwindow.h
    src/ui/tabs/tabmanager.h
    src/ui/tree/projecttreeview.h
    src/ui/canvas/canvasview.h
    src/ui/welcome/welcometab.h

    # Syntax highlighting core
    src/editor/syntax/base/syntaxhighlighter.h
    src/editor/syntax/highlighterfactory.h

    # Syntax highlighting languages
    src/editor/syntax/languages/cpp/cpphighlighter.h
    src/editor/syntax/languages/python/pythonhighlighter.h
    src/editor/syntax/languages/java/javahighlighter.h
    src/editor/syntax/languages/javascript/jshighlighter.h
    src/editor/syntax/languages/typescript/tshighlighter.h
    src/editor/syntax/languages/html/htmlhighlighter.h
    src/editor/syntax/languages/css/csshighlighter.h
    src/editor/syntax/languages/json/jsonhighlighter.h
)
set(FORMS
    src/ui/mainwindow/mainwindow.ui
)

add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
    ${FORMS}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

target_include_directories(${PROJECT_NAME} PRIVATE
    src
)
