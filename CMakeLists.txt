cmake_minimum_required(VERSION 3.16)
project(ScreenStreamer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Streamer source and header files
set(SRC_FILES_STREAMER
    src/main/streamer_main.cpp
    src/capture/capture_dxgi.cpp
    src/encoder/encoder_nvenc.cpp
    src/network/network_sender.cpp
)

set(HEADER_FILES_STREAMER
    src/capture/capture_dxgi.h
    src/encoder/encoder_nvenc.h
    src/network/network_sender.h
)

# Client source and header files
set(SRC_FILES_CLIENT
    src/main/client_main.cpp
    src/decoder/decoder_ffmpeg.cpp
    src/network/network_receiver.cpp
    src/renderer/renderer_opengl.cpp
)

set(HEADER_FILES_CLIENT
    src/decoder/decoder_ffmpeg.h
    src/network/network_receiver.h
    src/renderer/renderer_opengl.h
)

# SDL and FFmpeg
# find_package(SDL2 REQUIRED)
# include_directories(${SDL2_INCLUDE_DIRS})
find_path(SDL2_INCLUDE_DIR SDL.h PATH_SUFFIXES SDL2)
find_library(SDL2_LIBRARY NAMES SDL2)
include_directories(${SDL2_INCLUDE_DIR})

find_package(PkgConfig REQUIRED)
pkg_check_modules(AVUTIL REQUIRED IMPORTED_TARGET libavutil)
pkg_check_modules(AVCODEC REQUIRED IMPORTED_TARGET libavcodec)
pkg_check_modules(AVFORMAT REQUIRED IMPORTED_TARGET libavformat)
pkg_check_modules(SWSCALE REQUIRED IMPORTED_TARGET libswscale)

# Add executables
add_executable(streamer_main ${SRC_FILES_STREAMER} ${HEADER_FILES_STREAMER})
add_executable(client_main ${SRC_FILES_CLIENT} ${HEADER_FILES_CLIENT})

set_target_properties(client_main PROPERTIES WIN32_EXECUTABLE OFF)

# Include directories for both
target_include_directories(streamer_main PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_include_directories(client_main PRIVATE ${PROJECT_SOURCE_DIR}/src)

target_link_options(client_main PRIVATE -mconsole)

# Link libraries
target_link_libraries(streamer_main PRIVATE
    PkgConfig::AVCODEC
    PkgConfig::AVUTIL
    PkgConfig::AVFORMAT
    PkgConfig::SWSCALE
    d3d11 dxgi dxguid
    ws2_32
)

target_link_libraries(client_main PRIVATE
    ${SDL2_LIBRARY}
    PkgConfig::AVCODEC
    PkgConfig::AVUTIL
    PkgConfig::AVFORMAT
    PkgConfig::SWSCALE
    opengl32 glu32
    ws2_32
)

# Debug output
message(STATUS "SDL2_INCLUDE_DIRS: ${SDL2_INCLUDE_DIRS}")
message(STATUS "SDL2_LIBRARIES: ${SDL2_LIBRARIES}")
message(STATUS "AVCODEC_INCLUDE_DIRS: ${AVCODEC_INCLUDE_DIRS}")
message(STATUS "AVUTIL_INCLUDE_DIRS: ${AVUTIL_INCLUDE_DIRS}")