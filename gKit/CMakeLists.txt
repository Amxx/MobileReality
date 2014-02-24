cmake_minimum_required(VERSION 2.6)
project(gKit)
set(gKit_MAJOR_VERSION 1)
set(gKit_MINOR_VERSION 0)
set(gKit_REVISION_VERSION 0)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include_directories(
  include
)

file(GLOB gKit_files
	src/*.cpp
	src/GL/*.cpp
	src/Widgets/*.cpp
)

add_library(gKit SHARED
	${gKit_files}
)

add_definitions(-DGK_OPENGL3)
add_definitions(-DVERBOSE)

set(CMAKE_CXX_FLAGS_DEBUG			"-DDEBUG")
#set(CMAKE_CXX_FLAGS_REALEASE	"-NDEBUG")

IF (WIN32)
# ======================= WINDOWS =======================

	add_definitions( -DWIN32 -DNVWIDGETS_EXPORTS -D_USE_MATH_DEFINES -D_CRT_SECURE_NO_WARNINGS)
	add_definitions(-DNOMINMAX)
	
	include_directories(
		local/windows/include
	)
	
	link_directories(
		local/windows/lib32
	)
	
	target_link_libraries(gKit
		opengl32
		glu32
		glew32
		SDL2
		SDL2main
		SDL2_image
		SDL2_ttf
	)
	
# =======================================================
ENDIF(WIN32)

IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
# ======================= MAC OS ========================

	add_definitions(-DAPPLE_OSX)
	add_definitions( -framework OpenGL -framework Cocoa -framework SDL2 -framework SDL2_image -framework SDL2_ttf)
	
	list(APPEND CMAKE_SHARED_LINKER_FLAGS "-framework OpenGL -framework Cocoa -framework SDL2 -framework SDL2_image -framework SDL2_ttf")
	
	include_directories(
		opt/local/include
		/opt/local/include/OpenEXR
	)
	
	link_directories(
		/opt/local/lib
	)
	
	target_link_libraries(gKit
		GLEW
		IlmImf
		IlmThread
		Imath
		Half
	)
	
# =======================================================
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
# ======================== LINUX ========================

	add_definitions(-W -Wall -Wno-unused-parameter -pipe)
		
	find_package(GLEW REQUIRED)
	find_package(SDL2 REQUIRED)
	find_package(SDL2_image REQUIRED)
	find_package(SDL2_ttf REQUIRED)
	find_package(OpenEXR REQUIRED)

	#add_definitions(-DGK_OPENEXR)
	
	include_directories(
		${GLEW_INCLUDE_DIRS}
		${SDL2_INCLUDE_DIRS}
		${SDL2IMAGE_INCLUDE_DIRS}
		${SDL2TTF_INCLUDE_DIRS}
		#${OPENEXR_INCLUDE_DIRS}
	)
	target_link_libraries(gKit
		GL
		${GLEW_LIBRARY}
		${SDL2_LIBRARY}
		${SDL2IMAGE_LIBRARY}
		${SDL2TTF_LIBRARY}
		#${OPENEXR_LIBRARIES}
	)
	
	list(APPEND CMAKE_CXX_FLAGS_RELEASE "-mtune=native")
	
# =======================================================
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")


install(
  TARGETS
    gKit
  DESTINATION
    lib/
)


