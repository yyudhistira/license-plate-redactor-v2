cmake_minimum_required(VERSION 3.20)

# Core library sources
set(CORE_SOURCES
    FrameBuffer.cpp
    VideoProcessor.cpp
)

set(CORE_HEADERS
    FrameBuffer.h
    VideoProcessor.h
)

# Create core library
add_library(lpr_core STATIC ${CORE_SOURCES} ${CORE_HEADERS})

# Link OpenCV
target_link_libraries(lpr_core
    PUBLIC
        ${OpenCV_LIBS}
    PRIVATE
        spdlog::spdlog
        Threads::Threads
)

# Include directories
target_include_directories(lpr_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Set compile features
target_compile_features(lpr_core PUBLIC cxx_std_20)
