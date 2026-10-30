cmake_minimum_required( VERSION 3.8...3.27 )
project( cxx_gl_timeline VERSION 0.0.0 LANGUAGES CXX )
# options
set( CMAKE_CXX_STANDARD 20 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF )
# dependencies
add_subdirectory( son8/mainland )
add_subdirectory( son8/c_header )
add_subdirectory( son8/cxx_unit )
add_subdirectory( son8/core_lib )
# NOTE: overglad are added before windowed to avoid fetching
add_subdirectory( son8/overglad )
add_subdirectory( son8/windowed )
# pure basic skeleton
add_executable( pure pure.cxx )
target_link_libraries( pure PRIVATE son8::mainland son8::core_lib glad::son8 glfw::son8 )
# wrap basic skeleton
add_executable( wrap wrap.cxx )
target_link_libraries( wrap PRIVATE son8::mainland son8::core_lib son8::overglad son8::windowed )
# OpenGL timelines of vertices processing through versions 1.0-4.6 (data, models, buffers)
# -- pure
add_executable( vertices-pure vertices/pure.cxx )
target_link_libraries( vertices-pure PRIVATE son8::mainland son8::core_lib glad::son8 glfw::son8 )
# -- wrap
add_executable( vertices-wrap vertices/wrap.cxx )
target_link_libraries( vertices-wrap PRIVATE son8::mainland son8::core_lib son8::overglad son8::windowed )
# TODO timelines for lighting and textures processing
