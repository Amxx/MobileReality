cmake_minimum_required(VERSION 2.6)
project(Eigen)
set(gKit_MAJOR_VERSION 3)
set(gKit_MINOR_VERSION 2)
set(gKit_REVISION_VERSION 1)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include_directories(
	include
)

file(GLOB Eigen_files
	include/*
)

set(EIGEN_FILES					${Eigen_files}											CACHE INTERNAL	"Eigen source files"	)
set(EIGEN_INCLUDE_DIRS	${CMAKE_CURRENT_SOURCE_DIR}/include	CACHE PATH			"Eigen include dir"	)

