find_package(	OpenCV	REQUIRED	)

include_directories(
  ${GKIT_INCLUDE_DIRS}
	${OpenCV_INCLUDE_DIRS}
)

file(GLOB_RECURSE Sandbox_files
	*.cc
)

add_executable(Sandbox
	${Sandbox_files}
)

target_link_libraries(Sandbox
	${GKIT_LIBRARY}
	${OpenCV_LIBS}
)

install(
  TARGETS
		Sandbox
  DESTINATION
    bin/
)

