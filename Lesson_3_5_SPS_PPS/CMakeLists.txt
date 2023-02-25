cmake_minimum_required(VERSION 3.16.3)

project(Lesson_3_5_SPS_PPS)

set (CMAKE_EXPORT_COMPILE_COMMANDS ON)
set (CMAKE_C_COMPILER clang-14)
set (CMAKE_CXX_COMPILER clang++-14)
set (CMAKE_VERBOSE_MAKEFILE ON)
set (CMAKE_BUILD_TYPE "Debug")
#set (CMAKE_BUILD_TYPE "Release")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    message("debug mode set")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
    add_definitions(-DMICRO_SECONDS)    # logs time micro seconds
    add_definitions(-DLOGS_LEVEL_DEBUG)
    #add_definitions(-DLOGS_LEVEL_INFO)
    #add_definitions(-DLOGS_LEVEL_VERBOSE)
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    message("release mode set")
    add_definitions(-DMICRO_SECONDS)
    add_definitions(-DLOGS_LEVEL_INFO)
endif()

add_definitions(-w) # 禁止编译器的警告输出

set (SOURCES
    src/getWidth_and_Height.cpp
    src/AnnexBReader.cpp
    src/Nalu.cpp
    src/logs.c
    src/EBSP.cpp
    src/RBSP.cpp
    src/NaluSPS.cpp
    src/NaluPPS.cpp
    src/BitStream.cpp
)

add_executable( ${PROJECT_NAME}
    ${SOURCES}
)

target_include_directories( ${PROJECT_NAME}
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)