cmake_minimum_required( VERSION 3.8...3.27 )
project( cxx_gl_timeline VERSION 0.0.0 LANGUAGES CXX )

set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF )

add_subdirectory( son8/mainland )
add_subdirectory( son8/c_header )
add_subdirectory( son8/cxx_unit )
add_subdirectory( son8/core_lib )
# NOTE: overglad are added before windowed to avoid fetching
add_subdirectory( son8/overglad )
add_subdirectory( son8/windowed )

add_executable( pure pure.cxx )
target_link_libraries( pure PRIVATE son8::mainland son8::core_lib glad::son8 glfw::son8 )

add_executable( wrap wrap.cxx )
target_link_libraries( wrap PRIVATE son8::mainland son8::core_lib son8::overglad son8::windowed )
