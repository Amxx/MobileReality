# scanner_zbar module

include_directories(
	include
	${CMAKE_SOURCE_DIR}/src
)

add_library(scanner_zbar MODULE
	src/scanner_zbar.cc
)

target_link_libraries(scanner_zbar
	-lzbar
)

install(
  TARGETS
		scanner_zbar
  DESTINATION
    share/
)