cmake_minimum_required(VERSION 3.5)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMakeModules/")
include(CppGlob)
include(CustomMexFunctions)
project(Grid2DTest)

FIND_PACKAGE(Matlab REQUIRED)

message(STATUS "C Compiler   = ${CMAKE_C_COMPILER}")
message(STATUS "C++ Compiler = ${CMAKE_CXX_COMPILER}")

# define submodules directory and adding into include path
    if(NOT DEFINED ENV{SUBMODULES_DIR} AND NOT DEFINED SUBMODULES_DIR)
        set(SUBMODULES_DIR ${CMAKE_CURRENT_SOURCE_DIR})
    elseif(NOT DEFINED SUBMODULES_DIR)
        set(SUBMODULES_DIR $ENV{SUBMODULES_DIR})
    endif()
    message(STATUS "SUBMODULES_DIR = ${SUBMODULES_DIR}")
    include_directories(BEFORE ${SUBMODULES_DIR})

# include the relevant source and header files
    get_cxx_files(HEADER_FILE_PATHS Headers)
    get_cxx_files(MEX_HEADER_FILE_PATHS MexHeaders)
    get_cxx_files(MEXMEM_HEADER_FILE_PATHS ${SUBMODULES_DIR}/MexMemoryInterfacing/Headers)

    set(COMMON_SOURCE_FILES
            ${HEADER_FILE_PATHS}
            ${MEX_HEADER_FILE_PATHS}
            ${MEXMEM_HEADER_FILE_PATHS})
    set(SOURCE_FILES_1 Tests/main.cpp
            ${COMMON_SOURCE_FILES})
    set(SOURCE_FILES_2 Tests/mexmain.cpp
            ${COMMON_SOURCE_FILES})
    set(SOURCE_FILES ${SOURCE_FILES_1} ${SOURCE_FILES_2})

    # source group information to generate filters for VS
    source_group(
        "Header Files\\MexMemoryInterfacing" 
        FILES ${MEXMEM_HEADER_FILE_PATHS})
    source_group(
        "Header Files\\Grid2D" 
        FILES ${HEADER_FILE_PATHS})
    source_group(
        "Header Files\\Grid2D\\MexHeaders"
        FILES ${MEX_HEADER_FILE_PATHS})

# assign default build type in case of single config generator 
    if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
        message(STATUS "CMAKE_BUILD_TYPE not defined")
        set(CMAKE_BUILD_TYPE "Release")
    endif()
    message(STATUS "CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}")

# include executable
    custom_add_mex_exec(Grid2DTest ${SOURCE_FILES_1})
    custom_add_mex_exec(Grid2DMexIOTest ${SOURCE_FILES_2})

# Definition of install directory
    install(DIRECTORY DESTINATION ${CMAKE_BINARY_DIR}/install CONFIGURATIONS "Release")
    install_mex_exec(Grid2DTest ${CMAKE_BINARY_DIR}/install CONFIGURATIONS "Release")
    install_mex_exec(Grid2DMexIOTest ${CMAKE_BINARY_DIR}/install CONFIGURATIONS "Release")
    
# unset some cached variables so that they are restord to their
# default values the next time cmake is run
    unset(SUBMODULES_DIR CACHE)
