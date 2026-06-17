cmake_minimum_required(VERSION 3.16)

project(JuegoProyecto)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Ruta de SFML
set(SFML_DIR "C:/SFML/lib/cmake/SFML")

find_package(SFML 3 COMPONENTS Graphics Window System Audio REQUIRED)

include_directories(include)

add_executable(JuegoProyecto
    src/main.cpp
    src/Player.cpp
    src/AnimatedSprite.cpp
)

target_link_libraries(JuegoProyecto
    SFML::Graphics
    SFML::Window
    SFML::System
    SFML::Audio
)

# ================================
# SALIDA DEL EJECUTABLE EN /bin
# ================================

set(OUTPUT_DIR "${CMAKE_SOURCE_DIR}/bin")

set_target_properties(JuegoProyecto PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${OUTPUT_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${OUTPUT_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${OUTPUT_DIR}"
)

# ================================
# COPIAR ASSETS A /bin/assets
# ================================

add_custom_command(TARGET JuegoProyecto POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/assets"
    "$<TARGET_FILE_DIR:JuegoProyecto>/assets"
)

# ================================
# COPIAR DLLs DE SFML A /bin
# ================================

file(GLOB SFML_DLLS "C:/SFML/bin/*.dll")

if(SFML_DLLS)
    add_custom_command(TARGET JuegoProyecto POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${SFML_DLLS}
        "$<TARGET_FILE_DIR:JuegoProyecto>"
    )
endif()