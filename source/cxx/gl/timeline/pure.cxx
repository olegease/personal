#include <son8/main.hxx>
#include <glfw/son8.hxx>

namespace app {
    struct App {
        GLFWwindow *window;
    };
}

void son8::main( Args args ) {
    using namespace app;
    App appOut;
    App const &app = appOut;
    if ( not glfwInit( )) { goto failure; }

    appOut.window = glfwCreateWindow( 640, 360, "glfw window", nullptr, nullptr );

    if ( not app.window ) { goto clean_up_glfwInit; }

    glfwMakeContextCurrent( app.window );

    while ( not glfwWindowShouldClose( app.window )) {
        glfwPollEvents( );
        glfwSwapBuffers( app.window );
    }

    glfwDestroyWindow( app.window );
    glfwTerminate( );
    return;
clean_up_glfwInit:
    glfwTerminate( );
failure:
    Exit::failure( );
}
