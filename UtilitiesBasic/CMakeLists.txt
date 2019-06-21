
set (LIBRARY_NAME UtilitiesBasic)

set (SOURCE_FILES
	Image2D.cpp
	Image3D.cpp
	HoloVizioModel.cpp
	MultiViewModel.cpp
	tinyexr.cc
	)

set (HEADER_FILES
	Image2D.h
	Image3D.h
	geometry.h
	HoloVizioModel.h
	json.hpp
	MultiViewModel.h
	tinyexr.h
	)


add_library(${LIBRARY_NAME} ${SOURCE_FILES} ${HEADER_FILES})

set_target_properties(${LIBRARY_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)