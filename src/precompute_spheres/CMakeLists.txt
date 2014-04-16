find_package(	OpenCV	REQUIRED	)

include_directories(
	${EIGEN_INCLUDE_DIRS}
  ${GKIT_INCLUDE_DIRS}
	${OpenCV_INCLUDE_DIRS}
)

file(GLOB PrecomputeSpheres_files
	*.cc
)

add_executable(PrecomputeSpheres
	${PrecomputeSpheres_files}
)

target_link_libraries(PrecomputeSpheres
	${GKIT_LIBRARY}
	${OpenCV_LIBS}
)

install(
  TARGETS
		PrecomputeSpheres
  DESTINATION
    bin/
)

