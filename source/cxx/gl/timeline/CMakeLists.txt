cmake_minimum_required( VERSION 3.8...3.27 )
project( cxx_gl_timeline VERSION 0.0.0 LANGUAGES CXX )
# options
set( CMAKE_CXX_STANDARD 23 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF )
# dependencies
add_subdirectory( son8/mainland )
add_subdirectory( son8/c_header )
add_subdirectory( son8/cxx_unit )
add_subdirectory( son8/core_lib )
# NOTE: overglad are added before windowed to avoid fetching
add_subdirectory( fork/son8-overglad )
#add_subdirectory( son8/overglad )
add_subdirectory( son8/windowed )
# pure basic skeleton
add_executable( pure pure.cxx )
target_link_libraries( pure PRIVATE son8::mainland son8::core_lib glad::son8 glfw::son8 )
# wrap basic skeleton
add_executable( wrap wrap.cxx )
target_link_libraries( wrap PRIVATE son8::mainland son8::core_lib son8::overglad son8::windowed )
# shared warnings
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(APP_CXX_WARNINGS
        -Wall
        -Wextra
        -Wno-missing-field-initializers
        -Wno-deprecated-declarations
        -Wpedantic
        -Wshadow
        -Wconversion
        -Wsign-conversion
        -Wnon-virtual-dtor
        -Woverloaded-virtual
        -Wformat=2
    )
    add_compile_options("$<$<COMPILE_LANGUAGE:CXX>:${APP_CXX_WARNINGS}>")
endif()
# Helper Library
add_library( app_timelines )
add_library( app::timelines ALIAS app_timelines )
target_sources( app_timelines PRIVATE debug.cxx )
target_include_directories( app_timelines PUBLIC include )
target_link_libraries( app_timelines PUBLIC son8::core_lib PRIVATE glad::son8 )
# OpenGL timelines of vertices processing through versions 1.0-4.6 (data, models, buffers)
# -- pure
add_executable( vertices-pure vertices/pure.cxx )
target_link_libraries( vertices-pure PRIVATE app::timelines son8::mainland glad::son8 glfw::son8 )
# -- wrap
add_executable( vertices-wrap vertices/wrap.cxx )
target_link_libraries( vertices-wrap PRIVATE app::timelines son8::mainland son8::core_lib son8::overglad son8::windowed )
# TODO timelines for matrices, instance, textures, renderer, lighting, material, and blending processing
