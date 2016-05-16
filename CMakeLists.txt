cmake_minimum_required(VERSION 3.5)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/")
include(CppGlob)
project(Grid2DTest)

FIND_PACKAGE(Matlab REQUIRED)

message(STATUS "C Compiler   = ${CMAKE_C_COMPILER}")
message(STATUS "C++ Compiler = ${CMAKE_CXX_COMPILER}")

# Initializing MATLAB include and link directories
    message(STATUS "Matlab_INCLUDE_DIRS = ${Matlab_INCLUDE_DIRS}")
    include_directories(${Matlab_INCLUDE_DIRS})
    
    message(STATUS "Matlab_MEX_LIBRARY = ${Matlab_MEX_LIBRARY}")
    get_filename_component(Matlab_LIB_DIR ${Matlab_MEX_LIBRARY} DIRECTORY)
    
    message(STATUS "Matlab_LIB_DIR = ${Matlab_LIB_DIR}")
    link_directories(${Matlab_LIB_DIR})

    message(STATUS "Initializing relevant matlab libraries")
    set(Matlab_LINK_LIBS mex mx ut)
    if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
        foreach(Matlab_lib in ${Matlab_LINK_LIBS})
            list(APPEND Temp_Matlab_LINK_LIBS lib${Matlab_lib})
        endforeach()
        set(Matlab_LINK_LIBS ${Temp_Matlab_LINK_LIBS})
    endif()

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
    get_cxx_files(MEXMEM_HEADER_FILE_PATHS ${SUBMODULES_DIR}/MexMemoryInterfacing/Headers)
    
    set(SOURCE_FILES Tests/main.cpp
            ${HEADER_FILE_PATHS}
            ${MEXMEM_HEADER_FILE_PATHS}
       )

    # source group information to generate filters for VS
    source_group(
        "Header Files\\MexMemoryInterfacing" 
        FILES ${MEXMEM_HEADER_FILE_PATHS})
    source_group(
        "Header Files\\Grid2D" 
        FILES ${HEADER_FILE_PATHS})

# include executable
    add_executable(Grid2DTest ${SOURCE_FILES})
    target_link_libraries(Grid2DTest ${Matlab_LINK_LIBS})

# definition of MEX_TYPE
    add_definitions(-DMEX_EXE)

# assigning compiler independent WholeProgram optimization flags
    if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        set(LTO_COMPILE_SWITCH "/GL")
        set(LTO_LINK_SWITCH "/LTCG")
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(LTO_COMPILE_SWITCH "-flto") # These variables are unset because I have not optimized for gcc yet
        set(LTO_LINK_SWITCH "-flto")
    endif()
    
    message(STATUS "LTO_COMPILE_SWITCH = ${LTO_COMPILE_SWITCH}" )
    message(STATUS "LTO_LINK_SWITCH = ${LTO_LINK_SWITCH}" )

# assigning platform specific compiler flags
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -march=native")
    endif()

# assign default build type in case of single config generator 
    if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
        message(STATUS "CMAKE_BUILD_TYPE not defined")
        set(CMAKE_BUILD_TYPE "Release")
    endif()
    message(STATUS "CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}")

# definition of build type specific parameters
# for DEBUG CONFIG
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DIS_DEBUG=true")
# for RELEASE CONFIG
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DIS_DEBUG=false ${LTO_COMPILE_SWITCH}")
    set_target_properties(Grid2DTest PROPERTIES LINK_FLAGS_RELEASE ${LTO_LINK_SWITCH})

# unset some cached variables so that they are restord to their
# default values the next time cmake is run
    unset(SUBMODULES_DIR CACHE)
