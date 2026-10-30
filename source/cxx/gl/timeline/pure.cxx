#include <son8/main.hxx>
#include <glfw/son8.hxx>
#include <glad/son8.hxx>
#include <iostream>
#include <stdexcept>

namespace app {
    struct App {
        GLFWwindow *window;
        void print_goto_error( char const *message ) {
            if ( not App::Is_Goto_Error ) { return; }
            App::Is_Goto_Error = false;
            std::cerr << message << std::endl;
        }
    private:
        inline static bool Is_Goto_Error = true;
    };

    using Error = std::runtime_error;
}

void son8::main( Args args ) {
    using namespace app;
    App appOut;
    App const &app = appOut;
    if ( not glfwInit( )) { goto failed_init; }

    appOut.window = glfwCreateWindow( 640, 360, "glfw window", nullptr, nullptr );

    if ( app.window ) { goto failed_window; }

    glfwMakeContextCurrent( app.window );

    if ( not gladLoadGL(reinterpret_cast<GLADloadfunc>(glfwGetProcAddress))) { goto failed_glad; }

    while ( not glfwWindowShouldClose( app.window )) {
        glfwPollEvents( );
        glClearColor( 1.f, 0.f, 0.f, 0.f );
        glClear( GL_COLOR_BUFFER_BIT );
        glfwSwapBuffers( app.window );
    }

    glfwDestroyWindow( app.window );
    glfwTerminate( );
    return;
failed_glad:
    appOut.print_goto_error( "failed to load glad" );
    glfwDestroyWindow( app.window );
failed_window:
    appOut.print_goto_error( "failed to create window" );
    glfwTerminate( );
failed_init:
    appOut.print_goto_error( "failed to initialize glfw" );
    Exit::failure( );
}
