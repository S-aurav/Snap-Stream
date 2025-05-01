cmake_minimum_required(VERSION 3.16)
project(ScreenStreamer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Hardcode MSYS2 paths - adjust if different
set(MSYS2_PREFIX "C:/msys64/ucrt64")
set(FFMPEG_LIB_DIR "${MSYS2_PREFIX}/lib")
set(FFMPEG_INCLUDE_DIR "${MSYS2_PREFIX}/include")

# Set compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I${FFMPEG_INCLUDE_DIR}")

# Find libraries using absolute paths
find_library(AVUTIL_LIBRARY avutil PATHS ${FFMPEG_LIB_DIR} REQUIRED)
find_library(AVCODEC_LIBRARY avcodec PATHS ${FFMPEG_LIB_DIR} REQUIRED)
find_library(AVFORMAT_LIBRARY avformat PATHS ${FFMPEG_LIB_DIR} REQUIRED)
find_library(SWSCALE_LIBRARY swscale PATHS ${FFMPEG_LIB_DIR} REQUIRED)

# Find other dependencies
find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

# Source files
set(STREAMER_SOURCES
    src/main/streamer_main.cpp
    src/capture/capture_dxgi.cpp
    src/encoder/encoder_nvenc.cpp
    src/network/network_sender.cpp
)

set(CLIENT_SOURCES
    src/main/client_main.cpp
    src/decoder/decoder_ffmpeg.cpp
    src/network/network_receiver.cpp
    src/renderer/renderer_opengl.cpp
)

# Create executables
add_executable(streamer_main ${STREAMER_SOURCES})
add_executable(client_main ${CLIENT_SOURCES})

# Include directories
target_include_directories(streamer_main PRIVATE 
    ${PROJECT_SOURCE_DIR}/src
    ${FFMPEG_INCLUDE_DIR}
)

target_include_directories(client_main PRIVATE 
    ${PROJECT_SOURCE_DIR}/src
    ${FFMPEG_INCLUDE_DIR}
    ${GLEW_INCLUDE_DIRS}
    ${OPENGL_INCLUDE_DIR}
)

# Link libraries - IMPORTANT: Order matters!
target_link_libraries(streamer_main PRIVATE
    ${AVCODEC_LIBRARY}
    ${AVFORMAT_LIBRARY}
    ${SWSCALE_LIBRARY}
    ${AVUTIL_LIBRARY}
    d3d11 dxgi dxguid
    ws2_32
)

target_link_libraries(client_main PRIVATE
    SDL2::SDL2
    GLEW::GLEW
    ${AVCODEC_LIBRARY}
    ${AVFORMAT_LIBRARY}
    ${SWSCALE_LIBRARY}
    ${AVUTIL_LIBRARY}
    OpenGL::GL
    glu32
    ws2_32
)

# Windows-specific settings
if(WIN32)
    target_compile_definitions(client_main PRIVATE -D_WINDOWS)
    target_link_options(client_main PRIVATE -mconsole)
endif()

# Verification output
message(STATUS "FFmpeg libraries found at: ${FFMPEG_LIB_DIR}")
message(STATUS "AVUTIL: ${AVUTIL_LIBRARY}")
message(STATUS "AVCODEC: ${AVCODEC_LIBRARY}")
message(STATUS "AVFORMAT: ${AVFORMAT_LIBRARY}")
message(STATUS "SWSCALE: ${SWSCALE_LIBRARY}")