# videodevice_opencv module

include_directories(
	include
	${CMAKE_SOURCE_DIR}/src/app
)

add_library(videodevice_opencv MODULE
	src/videodevice_opencv.cc
)

target_link_libraries(videodevice_opencv
	
)

install(
  TARGETS
		videodevice_opencv
  DESTINATION
    share/
)