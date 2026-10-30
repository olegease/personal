cmake_minimum_required( VERSION 3.8...3.27 )
project( temp VERSION 0.0.0 LANGUAGES CXX )

set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF )

add_subdirectory( son8/mainland )
add_subdirectory( son8/windowed )

add_executable( pure pure.cxx )
target_link_libraries( pure PRIVATE son8::mainland glfw::son8 )
