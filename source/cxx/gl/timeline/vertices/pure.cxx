#include "model.hxx"

#include <son8/main.hxx>
#include <son8/core/alias/data.hxx>
#include <son8/cxx/file.hxx>
#include <glfw/son8.hxx>
#include <glad/son8.hxx>

// basic working example

#define APP_TIMELINE_IMMEDIATE  0x0100'1u
#define APP_TIMELINE_LIST       0x0100'2u
#define APP_TIMELINE_ARRAYS     0x0101'1u
#define APP_TIMELINE_INTERLEAVE 0x0101'2u
#define APP_TIMELINE APP_TIMELINE_INTERLEAVE

namespace app {
    using namespace son8;
    using namespace son8::core;

    struct Vertex {
        using Position = Flat< GLfloat, 2 >;
        using Color = Flat< GLfloat, 3 >;
        Position xy;
        Color rgb;
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

namespace app {
    struct Data final {
#if APP_TIMELINE == APP_TIMELINE_LIST
        using List = GLuint;
        List list;
#elif APP_TIMELINE == APP_TIMELINE_ARRAYS
        Grow< Vertex::Position > positions;
        Grow< Vertex::Color > colors;
#elif APP_TIMELINE == APP_TIMELINE_INTERLEAVE
        Grow< Vertex > vertices;
#endif
        using Triangle = Flat< Vertex, 3 >;
        Triangle triangle{{
            { .xy = {-.8f,-.8f}, .rgb = { 1.f, 0.f, 0.f }},
            { .xy = { .8f,-.8f}, .rgb = { 0.f, 1.f, 0.f }},
            { .xy = { .0f, .8f}, .rgb = { 0.f, 0.f, 1.f }},
        }};
    };
}

// immediate (should use `ifdefs` macros for chosen timeline
#if APP_TIMELINE == APP_TIMELINE_IMMEDIATE
void app::Model::setup( ) {
    data_ = new Data;
}
void app::Model::bind( ) { }
void app::Model::draw( ) {
    Ref< Data::Triangle > triangle = data_->triangle;
    glBegin( GL_TRIANGLES );
    for ( auto i = 0u; i < triangle.size( ); ++i ) {
        glColor3fv( triangle[i].rgb.data( ));
        glVertex2fv( triangle[i].xy.data( ));
    }
    glEnd( );
    APP_ASSERT( glGetError( ) == GL_NO_ERROR );
}
void app::Model::free( ) { }
void app::Model::clean( ) {
    delete data_;
}
#elif APP_TIMELINE == APP_TIMELINE_LIST
void app::Model::setup( ) {
    data_ = new Data;
    Ref< Data::List > list = data_->list;
    data_->list = glGenLists( 1 );
    glNewList( list, GL_COMPILE );
    glBegin( GL_TRIANGLES );
    Ref< Data::Triangle > triangle = data_->triangle;
    for ( auto i = 0u; i < triangle.size( ); ++i ) {
        glColor3fv( triangle[i].rgb.data( ));
        glVertex2fv( triangle[i].xy.data( ));
    }
    glEnd( );
    glEndList( );
    APP_ASSERT( glGetError( ) == GL_NO_ERROR );
}
void app::Model::bind( ) { }
void app::Model::draw( ) {
    Ref< Data::List > list = data_->list;
    glCallList( list );
    APP_ASSERT( glGetError( ) == GL_NO_ERROR );
}
void app::Model::free( ) { }
void app::Model::clean( ) {
    glDeleteLists( data_->list, 1 );
    delete data_;
    APP_ASSERT( glGetError( ) == GL_NO_ERROR );
}
#elif APP_TIMELINE == APP_TIMELINE_ARRAYS
void app::Model::setup( ) {
    data_ = new Data;
    Ref< Data::Triangle > triangle = data_->triangle;
    for ( auto i = 0u; i < triangle.size( ); ++i ) {
        data_->colors.emplace_back( triangle[i].rgb );
        data_->positions.emplace_back( triangle[i].xy );
    }
}
void app::Model::bind( ) {
    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_COLOR_ARRAY );
    glVertexPointer( data_->positions.back( ).size( ), GL_FLOAT, 0, data_->positions.data( ));
    glColorPointer( data_->colors.back( ).size( ), GL_FLOAT, 0, data_->colors.data( ));
}
void app::Model::draw( ) {
    glDrawArrays( GL_TRIANGLES, 0, 3 );
}
void app::Model::free( ) {
    glDisableClientState( GL_VERTEX_ARRAY );
    glDisableClientState( GL_COLOR_ARRAY );
}
void app::Model::clean( ) {
    delete data_;
}
#elif APP_TIMELINE == APP_TIMELINE_INTERLEAVE
void app::Model::setup( ) {
    data_ = new Data;
    Ref< Data::Triangle > triangle = data_->triangle;
    for ( auto i = 0u; i < triangle.size( ); ++i ) {
        data_->vertices.emplace_back( triangle[i] );
    }
}
void app::Model::bind( ) {
    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_COLOR_ARRAY );
    static constexpr auto stride = sizeof( Vertex );
    glVertexPointer( data_->triangle[0].xy.size( ), GL_FLOAT, stride, &data_->triangle[0].xy );
    glColorPointer( data_->triangle[0].rgb.size( ), GL_FLOAT, stride, &data_->triangle[0].rgb );
}
void app::Model::draw( ) {
    glDrawArrays( GL_TRIANGLES, 0, 3 );
}
void app::Model::free( ) {
    glDisableClientState( GL_VERTEX_ARRAY );
    glDisableClientState( GL_COLOR_ARRAY );
}
void app::Model::clean( ) {
    delete data_;
}
#endif
// TODO: display list, elements, shaders, vertex array, named buffers and so on
//void app::Model::setup( ) { }
//void app::Model::bind( ) { }
//void app::Model::draw( ) { }
//void app::Model::free( ) { }
//void app::Model::clean( ) { }
