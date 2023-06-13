
if(RETDEC_LOCAL_DIR)
	message(STATUS "RetDec: using local RetDec directory.")
	add_subdirectory(
		${RETDEC_LOCAL_DIR}
		${CMAKE_CURRENT_BINARY_DIR}/retdec-build
		EXCLUDE_FROM_ALL
	)
	set(retdec_SOURCE_DIR ${RETDEC_LOCAL_DIR})
else()
	message(STATUS "RetDec: using remote RetDec revision.")
	include(FetchContent)
	
	FetchContent_Declare(retdec
		GIT_REPOSITORY https://github.com/avast/retdec
		GIT_TAG        53e55b4b26e9b843787f0e06d867441e32b1604e
	)

	FetchContent_GetProperties(retdec)
	if(NOT retdec_POPULATED)
		FetchContent_Populate(retdec)
		add_definitions(-D__ARM64__)
		
		add_subdirectory(
			${retdec_SOURCE_DIR}
			${retdec_BINARY_DIR}
			EXCLUDE_FROM_ALL
		)
	endif()
endif()

set(retdec_SOURCE_DIR ${retdec_SOURCE_DIR} PARENT_SCOPE)
