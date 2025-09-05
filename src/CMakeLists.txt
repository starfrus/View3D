cmake_minimum_required(VERSION 3.16)
project(3DModelViewer LANGUAGES CXX)

# Включаем дополнительные флаги для отладки
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Найти требуемые модули Qt6
find_package(Qt6 REQUIRED COMPONENTS Widgets OpenGL OpenGLWidgets)

# Автообработка UI, MOC и ресурсов
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Указываем пути к файлам в папке gui
set(UI_FILES gui/mainwindow.ui)

# Добавляем исходные файлы с учетом папки gui
set(SOURCES
    main.cpp
    gui/mainwindow.cpp
    gui/glwidget.cpp
    model/model.cpp
)

set(HEADERS
    gui/mainwindow.h
    gui/glwidget.h
    model/model.hpp
    patterns/command.hpp
    patterns/model_manager.hpp
    controller/controller.hpp
)

# Создаем GUI-приложение
add_executable(3DModelViewer
    ${SOURCES}
    ${HEADERS}
    ${UI_FILES}
)

# Привязываем к Qt6 Widgets
target_link_libraries(3DModelViewer PRIVATE
    Qt6::Widgets
    Qt6::OpenGLWidgets
    Qt6::OpenGL
    OpenGL::GLU
)

# Добавляем папку gui в include директории
target_include_directories(3DModelViewer PRIVATE
    gui
    model
    patterns
    controller
    # ${CMAKE_CURRENT_SOURCE_DIR}  # на случай, если main.cpp использует какие-то заголовки
)

# Выводим информацию о включенных файлах
message(STATUS "Source files: ${SOURCES}")
message(STATUS "Header files: ${HEADERS}")
message(STATUS "UI files: ${UI_FILES}")

# Опция для сборки в режиме Debug/Release
set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build" FORCE)
