cmake_minimum_required(VERSION 3.20)

project(Utility)

add_library(Utilities INTERFACE)
target_sources(Utilities
    INTERFACE
        Set.h
        LinearAlgebraTypeTraits.h
        Ray.h
        Matrix_MxN.h
        Point_X.h
        Vector_X.h
        SmartString.h)
target_include_directories(Utilities
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR})

add_library(smart_string INTERFACE)
target_sources(smart_string
    INTERFACE
        SmartString.h)
target_include_directories(smart_string
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR})

add_library(linear_algebra INTERFACE)
target_sources(linear_algebra
    INTERFACE
        LinearAlgebraTypeTraits.h
        Ray.h
        Matrix_MxN.h
        Point_X.h
        Vector_X.h)
target_include_directories(linear_algebra
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR})