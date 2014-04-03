find_package(	OpenCV	REQUIRED	)

include_directories(
  ${GKIT_INCLUDE_DIRS}
	${OpenCV_INCLUDE_DIRS}
)

file(GLOB Precompute_files
	*.cc
)

add_executable(Precompute
	${Precompute_files}
)

target_link_libraries(Precompute
	${GKIT_LIBRARY}
	${OpenCV_LIBS}
)

install(
  TARGETS
    Precompute
  DESTINATION
    bin/
)

