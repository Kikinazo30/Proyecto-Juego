cmake_minimum_required(VERSION 3.16)
project(JuegoProyecto)

set(CMAKE_CXX_STANDARD 17)

set(SFML_DIR "C:/SFML/lib/cmake/SFML")

find_package(SFML 3 COMPONENTS Graphics Window System REQUIRED)

include_directories(include)

add_executable(JuegoProyecto
    src/main.cpp
    src/Player.cpp
)

target_link_libraries(JuegoProyecto SFML::Graphics SFML::Window SFML::System)