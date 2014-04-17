find_package(	OpenCV	REQUIRED	)

include_directories(
  ${GKIT_INCLUDE_DIRS}
	${OpenCV_INCLUDE_DIRS}
)

file(GLOB_RECURSE Reality_files
	*.cc
)

add_executable(Reality
	${Reality_files}
)

target_link_libraries(Reality
	${GKIT_LIBRARY}
	${OpenCV_LIBS}
)

install(
  TARGETS
    Reality
  DESTINATION
    bin/
)

