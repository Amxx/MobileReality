find_package( OpenGL REQUIRED )
find_package( GLUT REQUIRED   )
find_package( OpenCV REQUIRED )

include_directories(
  ${PROJECT_SOURCE_DIR}/src
	${OPENGL_INCLUDE_DIRS}
	${GLUT_INCLUDE_DIRS}
	${OpenCV_INCLUDE_DIRS}
)

add_executable(Reality
	calibration.cc
  camera.cc
	envmap.cc
	main.cc
	mattools.cc
	scanner.cc
	symbol.cc
)



target_link_libraries(Reality
	${OPENGL_LIBRARIES}
	${GLUT_LIBRARY}
	${OpenCV_LIBS}
)


install(
  TARGETS
    Reality
  DESTINATION
    bin/
)

