file(GLOB Shaders_files
	*.glsl
)

install(
  FILES
    ${Shaders_files}
  DESTINATION
    shaders/
)

