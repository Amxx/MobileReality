find_package(	OpenCV	REQUIRED	)




add_definitions(-DGK_OPENEXR)



include_directories(
  ${GKIT_INCLUDE_DIRS}
	${OpenCV_INCLUDE_DIRS}
	
	
	
	
		${GLEW_INCLUDE_DIRS}
		${SDL2_INCLUDE_DIRS}
		${SDL2IMAGE_INCLUDE_DIRS}
		${SDL2TTF_INCLUDE_DIRS}
		${OPENEXR_INCLUDE_DIRS}
)

file(GLOB Reality_files
	*.cc
)

add_executable(Reality
	${Reality_files}
	
	${GKIT_FILES}
)
	
target_link_libraries(Reality
	#${GKIT_LIBRARY}
	${OpenCV_LIBS}
	
	
		GL
		${GLEW_LIBRARY}
		${SDL2_LIBRARY}
		${SDL2IMAGE_LIBRARY}
		${SDL2TTF_LIBRARY}
		${OPENEXR_LIBRARIES}
		
)

install(
  TARGETS
    Reality
  DESTINATION
    bin/
)

