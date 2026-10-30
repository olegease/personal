#include <son8/main.hxx>
#include <son8/core/alias/pour.hxx>
#include <son8/cxx/file.hxx>
#include <glfw/son8.hxx>
#include <glad/son8.hxx>

namespace app {
    using namespace son8;
    using namespace son8::core;

    struct App {
        Ptr< GLFWwindow  > window;
        void print_goto_error( Ptr< char const > message ) {
            if ( not App::Is_Goto_Error ) { return; }
            App::Is_Goto_Error = false;
            cxx::cerr << message << cxx::endl;
        }
    private:
        inline static bool Is_Goto_Error = true;
    };
}

void son8::main( Args args ) {
    using namespace app;
    App appOut;
    Ref< App > app = appOut;
    if ( not glfwInit( )) { goto failed_init; }

    appOut.window = glfwCreateWindow( 640, 360, "glfw window", nullptr, nullptr );

    if ( not app.window ) { goto failed_window; }

    glfwMakeContextCurrent( app.window );

    if ( not gladLoadGL( reinterpret_cast< GLADloadfunc >( glfwGetProcAddress ))) { goto failed_glad; }

    while ( not glfwWindowShouldClose( app.window )) {
        glfwPollEvents( );
        glClearColor( .125f, .25f, .5f, 1.f );
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
