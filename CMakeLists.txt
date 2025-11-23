cmake_minimum_required(VERSION 3.11)
project(horses)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Configuración del estándar de C++
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Incluir FetchContent para descargar dependencias
include(FetchContent)

# Descargar y compilar raylib automáticamente
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.0
)

# Opciones de raylib antes de compilar
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)

# Usar X11 en vez de Wayland para evitar problemas en NixOS
if(UNIX AND NOT APPLE)
    set(USE_WAYLAND OFF CACHE BOOL "" FORCE)
endif()

FetchContent_MakeAvailable(raylib)

# Descargar raygui también
FetchContent_Declare(
    raygui
    GIT_REPOSITORY https://github.com/raysan5/raygui.git
    GIT_TAG 4.0
)
FetchContent_MakeAvailable(raygui)

# Tu ejecutable
add_executable(${PROJECT_NAME} src/main.cpp src/horse.cpp src/modes.cpp src/reader.cpp)

# IMPORTANTE: Incluir raylib ANTES de linkear
target_include_directories(${PROJECT_NAME} PRIVATE
    ${raylib_SOURCE_DIR}/src
    ${raygui_SOURCE_DIR}/src
)

# Linkear con raylib
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)

# Agregar flags de compilación para evitar warnings
if(NOT MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
endif()

# En Windows, hacer ejecutable estático
if(WIN32)
    target_link_options(${PROJECT_NAME} PRIVATE -static)
endif()

# Copiar assets al build directory
file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})
