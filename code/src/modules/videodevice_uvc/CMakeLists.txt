# videodevice_uvc module

include_directories(
	include
	UVCLib
	${CMAKE_SOURCE_DIR}/src/app
)

set(UVC_src
	UVCLib/avilib.c
	UVCLib/color.c
	UVCLib/utils.c
	UVCLib/v4l2uvc.c
)

add_library(videodevice_uvc MODULE
	src/videodevice_uvc.cc
	${UVC_src}
)

target_link_libraries(videodevice_uvc
)

install(
  TARGETS
		videodevice_uvc
  DESTINATION
    share/
)