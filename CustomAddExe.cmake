function(custom_add_executable exec_name)
    add_executable(${exec_name} ${ARGN})
    target_link_libraries(${exec_name} ${Matlab_LINK_LIBS})
    if(NOT DEFINED LTO_LINK_SWITCH)
        if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
            set(LTO_COMPILE_SWITCH "/GL")
            set(LTO_LINK_SWITCH "/LTCG")
        elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            set(LTO_COMPILE_SWITCH "-flto") # These variables are unset because I have not optimized for gcc yet
            set(LTO_LINK_SWITCH "-flto")
        endif()
    endif()
    set_target_properties(${exec_name} PROPERTIES LINK_FLAGS_RELEASE ${LTO_LINK_SWITCH})
    set_target_properties(${exec_name} PROPERTIES INSTALL_RPATH ${Matlab_LIB_DIR})
endfunction()