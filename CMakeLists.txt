cmake_minimum_required(VERSION 2.6)

# =====================================================================
# =													CHECKING COMPILER													=
# =====================================================================
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "4.7")
    message(FATAL_ERROR "${CMAKE_CXX_COMPILER} v${CMAKE_CXX_COMPILER_VERSION} found when g++ 4.7 or later is required")
    message(FATAL_ERROR "If installed, please use -DCMAKE_CXX_COMPILER:FILEPATH=g++-4.7 for example")
  endif()
endif()

# =====================================================================
# =														BUILDING GKIT														=
# =====================================================================
add_subdirectory(gKit)

# =====================================================================
# =																FLAGS																=
# =====================================================================
list(APPEND CMAKE_CXX_FLAGS "-std=c++11")
add_definitions(-march=native -O3)
add_definitions(-W -g -pipe)

# =====================================================================
# =							SETTING MODULES LIST AND BUILD OPTIONS								=
# =====================================================================
file(GLOB MODULES_PATHS src/modules/*)
# Remove leading path
foreach(module ${MODULES_PATHS})
  string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/src/modules/" "" tmp ${module})
  list(APPEND MODULES ${tmp})
endforeach(module ${MODULES_PATHS})
# Create menu for select/deselect modules building
foreach(module ${MODULES})
  string( TOUPPER "BUILD_${module}" moduleName )
  option(${moduleName} "Build ${module} module" ON)
endforeach(module ${MODULES})


# =====================================================================
# =													BUILDING MODULES													=
# =====================================================================
foreach(module ${MODULES})
  string( TOUPPER "BUILD_${module}" moduleName )
  if(${moduleName})
    add_subdirectory(src/modules/${module})
  endif(${moduleName})
endforeach(module ${MODULES})

# =====================================================================
# =												BUILDING APPLICATION												=
# =====================================================================

add_subdirectory(src/app)
add_subdirectory(src/precompute_ambient)
add_subdirectory(src/precompute_spheres)
add_subdirectory(src/shaders)















