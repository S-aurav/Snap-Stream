cmake_minimum_required(VERSION 3.10)
project(ScreenCapture)

set(CMAKE_CXX_STANDARD 14)

find_package(X11 REQUIRED)
find_package(SDL2 REQUIRED)

include_directories(${X11_INCLUDE_DIR} ${SDL2_INCLUDE_DIRS})

add_executable(ScreenCapture main.cpp)
target_link_libraries(ScreenCapture ${X11_LIBRARIES} ${SDL2_LIBRARIES})
