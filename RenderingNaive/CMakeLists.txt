
set (EXECUTABLE_NAME RenderingNaive)


set (SOURCE_FILES
	main.cpp
	RayTracer.cpp
	)

set (HEADER_FILES
	RayTracer.h
	)
	

add_executable(${EXECUTABLE_NAME} ${SOURCE_FILES} ${HEADER_FILES} ${COMMON_FILES})

set_target_properties(${EXECUTABLE_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)


target_link_libraries(${EXECUTABLE_NAME} UtilitiesBasic)
target_include_directories(${EXECUTABLE_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/UtilitiesBasic)