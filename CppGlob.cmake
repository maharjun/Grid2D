function(GET_EXT_FILES VARIABLE DIRECTORY)

	if(ARGC EQUAL 0)
		# set nothing
	else()
		foreach(ext in ${ARGN})
			file(GLOB TEMPVAR "${DIRECTORY}/*.${ext}")
			set(EXT_FILES "${EXT_FILES};${TEMPVAR}")
		endforeach()
	endif()
    set(${VARIABLE} ${EXT_FILES} PARENT_SCOPE)
	message(STATUS "Extensions requested = ${ARGN}")
    message(STATUS "Directory requested = ${DIRECTORY}")

endfunction()

function(GET_CXX_FILES VARIABLE DIRECTORY)
	set(CXX_EXTENSIONS "cpp;c;h;hpp;inl")

    GET_EXT_FILES(CPP_FILES ${DIRECTORY} ${CXX_EXTENSIONS})
    set(${VARIABLE} ${CPP_FILES} PARENT_SCOPE)
    message(STATUS "Files Found = ${CPP_FILES}")
endfunction()