cmake_minimum_required(VERSION 3.5)
project(TrialCLionProj)

FIND_PACKAGE(MATLAB REQUIRED)

# Initializing MATLAB include and link directories
    message(STATUS "Matlab_INCLUDE_DIRS = ${Matlab_INCLUDE_DIRS}")
    include_directories(${Matlab_INCLUDE_DIRS})
    
    message(STATUS "Matlab_MEX_LIBRARY = ${Matlab_MEX_LIBRARY}")
    get_filename_component(Matlab_LIB_DIR ${Matlab_MEX_LIBRARY} DIRECTORY)
    
    message(STATUS "Matlab_LIB_DIR = ${Matlab_LIB_DIR}")
    link_directories(${Matlab_LIB_DIR})

# define submodules directory and adding into include path
    if(NOT DEFINED ENV{SUBMODULES_DIR} AND NOT DEFINED SUBMODULES_DIR)
        set(SUBMODULES_DIR ${CMAKE_CURRENT_SOURCE_DIR})
    elseif(NOT DEFINED SUBMODULES_DIR)
        set(SUBMODULES_DIR $ENV{SUBMODULES_DIR})
    endif()
    message(STATUS "SUBMODULES_DIR = ${SUBMODULES_DIR}")
    include_directories(BEFORE ${SUBMODULES_DIR})

# include the relevant source and header files
    file(GLOB_RECURSE HEADER_FILE_PATHS Headers/*.cpp Headers/*.hpp Headers/*.inl)
    file(GLOB MEXMEM_HEADER_FILE_PATHS ${SUBMODULES_DIR}/MexMemoryInterfacing/Headers/*)
    
    set(SOURCE_FILES Tests/main.cpp
            ${HEADER_FILE_PATHS}
            ${MEXMEM_HEADER_FILE_PATHS}
       )

# include executable
    add_executable(TrialCLionProj ${SOURCE_FILES})
    target_link_libraries(TrialCLionProj libmex libmx libut)

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

# definition of build type specific parameters
# for DEBUG CONFIG
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DIS_DEBUG=true")
# for RELEASE CONFIG
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DIS_DEBUG=false ${LTO_COMPILE_SWITCH}")
    set_target_properties(TrialCLionProj PROPERTIES LINK_FLAGS_RELEASE ${LTO_LINK_SWITCH})

# unset some cached variables so that they are restord to their
# default values the next time cmake is run
    unset(SUBMODULES_DIR CACHE)
