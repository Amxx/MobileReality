find_package(	OpenCV	REQUIRED	)

#find_package(	OpenGL	REQUIRED	)
#find_package(	GLUT		REQUIRED	)

include_directories(
  ${GKIT_INCLUDE_DIRS}
	${OpenCV_INCLUDE_DIRS}
	# ${OPENGL_INCLUDE_DIRS}
	# ${GLUT_INCLUDE_DIRS}
)

file(GLOB Reality_files
	app/*.cc
)

add_executable(Reality
	${Reality_files}
)

target_link_libraries(Reality
	${GKIT_LIBRARY}
	${OpenCV_LIBS}
	# ${OPENGL_LIBRARIES}
	# ${GLUT_LIBRARY}
)


install(
  TARGETS
    Reality
  DESTINATION
    bin/
)

