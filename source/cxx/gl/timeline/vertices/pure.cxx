#include <son8/main.hxx>
#include <son8/core/alias/pour.hxx>
#include <son8/cxx/file.hxx>
#include <glfw/son8.hxx>
#include <glad/son8.hxx>

// basic working example

#define APP_TIMELINE_IMMEDIATE 0
#define APP_TIMELINE APP_TIMELINE_IMMEDIATE

namespace app {
    using namespace son8;
    using namespace son8::core;

    struct Data;

    class Model final {
        using DataPtr_ = Ptr< Data >;
        DataPtr_ data_;
    public:
        void setup( );
        void bind( );
        void draw( );
        void free( );
        void clean( );
    };

    struct App {
        Model model;
        Ptr< GLFWwindow  > window;
        void goto_print_error( Ptr< char const > message ) {
            if ( isGotoPrinted ) { return; }
            isGotoPrinted = true;
            cxx::cerr << message << cxx::endl;
        }
        void goto_success( ) { isGotoSuccess = true; isGotoPrinted = true; }
        bool goto_is_success( ) const { return isGotoSuccess; }
    private:
        bool isGotoPrinted = false;
        bool isGotoSuccess = false;
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

    appOut.model.setup( );

    while ( not glfwWindowShouldClose( app.window )) {
        glfwPollEvents( );
        glClearColor( .125f, .125f, .125f, 1.f );
        glClear( GL_COLOR_BUFFER_BIT );
        appOut.model.bind( );
        appOut.model.draw( );
        appOut.model.free( );

        glfwSwapBuffers( app.window );
    }

    appOut.model.clean( );

    appOut.goto_success( );
failed_glad:
    appOut.goto_print_error( "failed to load glad" );
    glfwDestroyWindow( app.window );
failed_window:
    appOut.goto_print_error( "failed to create window" );
    glfwTerminate( );
failed_init:
    appOut.goto_print_error( "failed to initialize glfw" );
    if ( not app.goto_is_success( )) Exit::failure( );
}
// immediate (should use `ifdefs` macros for chosen timeline
void app::Model::setup( ) { }
void app::Model::bind( ) { }
void app::Model::draw( ) { }
void app::Model::free( ) { }
void app::Model::clean( ) { }
// TODO: display list, elements, shaders, vertex array, named buffers and so on
