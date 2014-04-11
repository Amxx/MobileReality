find_package(	OpenCV	REQUIRED	)

include_directories(
  ${GKIT_INCLUDE_DIRS}
	${OpenCV_INCLUDE_DIRS}
)

file(GLOB PrecomputeAmbient_files
	*.cc
)

add_executable(PrecomputeAmbient
	${PrecomputeAmbient_files}
)

target_link_libraries(PrecomputeAmbient
	${GKIT_LIBRARY}
	${OpenCV_LIBS}
)

install(
  TARGETS
		PrecomputeAmbient
  DESTINATION
    bin/
)

