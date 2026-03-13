cmake_minimum_required(VERSION 3.10)
include("${CMAKE_SOURCE_DIR}/FindGfxLibs.cmake")

# Name of the project
project(WhoopsnHoops)

# Enforce use of C++17
set(CMAKE_CXX_STANDARD_REQUIRED 17)
set(CMAKE_CXX_STANDARD 17)

#for compiler commands
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Use glob to get the list of all source files.
file(GLOB_RECURSE SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp" "${CMAKE_SOURCE_DIR}/ext/*/*.cpp" "${CMAKE_SOURCE_DIR}/ext/glad/src/*.c")

# We don't really need to include header and resource files to build, but it's
# nice to have them show up in IDEs.
file(GLOB_RECURSE HEADERS "src/*.h" "ext/*/*.h" "ext/glad/*/*.h")
file(GLOB_RECURSE GLSL "resources/*.glsl")

#Find and configure FreeType
find_package(Freetype REQUIRED)
find_package(OpenAL REQUIRED)
find_package(OpenGL REQUIRED)
find_package(SndFIle REQUIRED)

#Find and configure OpenAL

#For cmake to find openal-soft you may need to set:
# export CMAKE_PREFIX_PATH="/opt/homebrew/opt/openal-soft"

# Include directories (only system ones)

include_directories("ext")
include_directories("ext/glad/include")

# Set the executable.
add_executable(${CMAKE_PROJECT_NAME} ${SOURCES} ${HEADERS} ${GLSL})

# Helper function included from FindGfxLibs.cmake
findGLFW3(${CMAKE_PROJECT_NAME})
findGLM(${CMAKE_PROJECT_NAME})

# OS specific options and libraries
if(NOT WIN32)

  message(STATUS "Adding GCC style compiler flags")
  add_compile_options("-Wall")
  add_compile_options("-Werror")
  add_compile_options("-g")
	add_compile_options("-O0")
  add_compile_options("-pedantic")
  add_compile_options("-Werror=return-type")
 
  # TODO: The following links may be uneeded. 
  if(APPLE)
    # Add required frameworks for GLFW.
		target_link_libraries(${CMAKE_PROJECT_NAME} SndFile::sndfile glfw OpenGL::GL OpenAL::OpenAL Freetype::Freetype)
		#target_link_libraries(${CMAKE_PROJECT_NAME} "-framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo" ${OPENAL_LIBRARY} ${FREETYPE_LIBRARIES})
  else()
    #Link the Linux OpenGL library
    target_link_libraries(${CMAKE_PROJECT_NAME} "GL" "dl" ${FREETYPE_LIBRARIES})
  endif()

else()

  # Link OpenGL on Windows
  target_link_libraries(${CMAKE_PROJECT_NAME} opengl32.lib)
  target_link_libraries(${CMAKE_PROJECT_NAME} ${FREETYPE_LIBRARIES})

endif()
