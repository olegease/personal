#include "model.hxx"

#include <son8/main.hxx>
#include <son8/core/alias/data.hxx>
#include <son8/cxx/file.hxx>
#include <glfw/son8.hxx>
#include <glad/son8.hxx>

#define APP_TIMELINE_IMMEDIATE  0x0100'1u
#define APP_TIMELINE_LIST       0x0100'2u
#define APP_TIMELINE_ARRAYS     0x0101'1u
#define APP_TIMELINE_INTERLEAVE 0x0101'2u
#define APP_TIMELINE_ELEMENTS   0x0101'3u
#define APP_TIMELINE APP_TIMELINE_ELEMENTS

namespace app {
    using namespace son8;
    using namespace son8::core;

    static constexpr signed Zero = 0;
    static constexpr unsigned Zero_U = 0u;

    struct Vertex {
        static constexpr auto Position_Size = 2;
        static constexpr auto Color_Size = 3;
        using Position = Flat< GLfloat, Position_Size >;
        using Color = Flat< GLfloat, Color_Size >;
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
#elif APP_TIMELINE == APP_TIMELINE_ELEMENTS
        Grow< Vertex > vertices;
#endif
        static constexpr auto Triangle_Size = 4;
        static constexpr auto Elements_Size = 6;
        using Triangle = Flat< Vertex, Triangle_Size >;
        using Element = GLubyte;
        Triangle triangles{{
            { .xy = {-.8f,-.8f}, .rgb = { 1.f, 0.f, 0.f }},
            { .xy = { .8f,-.8f}, .rgb = { 0.f, 1.f, 0.f }},
            { .xy = { .8f, .8f}, .rgb = { 0.f, 0.f, 1.f }},
            { .xy = {-.8f, .8f}, .rgb = { 1.f, 1.f, 1.f }},
        }};
        Flat< Element, Elements_Size > elements{{ 0u, 1u, 2u, 2u, 3u, 0u }};
        auto xy( Element index ) const -> Ref< Vertex::Position > { return triangles[index].xy; }
        auto xy_v( Element index ) const -> Ptr< GLfloat const > { return xy( index ).data( ); }
        auto rgb( Element index ) const -> Ref< Vertex::Color > { return triangles[index].rgb; }
        auto rgb_v( Element index ) const -> Ptr< GLfloat const > { return rgb( index ).data( ); }
    };
}

// immediate (should use `ifdefs` macros for chosen timeline
#if APP_TIMELINE == APP_TIMELINE_IMMEDIATE
void app::Model::setup( ) {
    data_ = new Data;
}
void app::Model::bind( ) { }
void app::Model::draw( ) {
    glBegin( GL_TRIANGLES );
    for ( auto element : data_->elements ) {
        glColor3fv( data_->rgb_v( element ));
        glVertex2fv( data_->xy_v( element ));
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
    for ( auto element : data_->elements ) {
        glColor3fv( data_->rgb_v( element ));
        glVertex2fv( data_->xy_v( element ));
    }
    glEnd( );
    glEndList( );
    APP_ASSERT( glGetError( ) == GL_NO_ERROR );
}
void app::Model::bind( ) { }
void app::Model::draw( ) {
    glCallList( data_->list );
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
    for ( auto index : data_->elements ) {
        data_->colors.emplace_back( data_->rgb( index ));
        data_->positions.emplace_back( data_->xy( index ));
    }
}
void app::Model::bind( ) {
    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_COLOR_ARRAY );
    glVertexPointer( Vertex::Position_Size, GL_FLOAT, Zero, data_->positions.data( ));
    glColorPointer( Vertex::Color_Size, GL_FLOAT, Zero, data_->colors.data( ));
}
void app::Model::draw( ) {
    glDrawArrays( GL_TRIANGLES, Zero, Data::Elements_Size );
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
    for ( auto index : data_->elements ) {
        data_->vertices.emplace_back( Vertex{ data_->xy( index ), data_->rgb( index )});
    }
}
void app::Model::bind( ) {
    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_COLOR_ARRAY );
    static constexpr auto stride = sizeof( Vertex );
    glVertexPointer( Vertex::Position_Size, GL_FLOAT, stride, &data_->vertices[Zero].xy );
    glColorPointer( Vertex::Color_Size, GL_FLOAT, stride, &data_->vertices[Zero].rgb );
}
void app::Model::draw( ) {
    glDrawArrays( GL_TRIANGLES, Zero, Data::Elements_Size );
}
void app::Model::free( ) {
    glDisableClientState( GL_VERTEX_ARRAY );
    glDisableClientState( GL_COLOR_ARRAY );
}
void app::Model::clean( ) {
    delete data_;
}
#elif APP_TIMELINE == APP_TIMELINE_ELEMENTS
void app::Model::setup( ) {
    data_ = new Data;
    data_->vertices.reserve( data_->triangles.size( ));
    for ( Ref< Vertex > vertex : data_->triangles ) data_->vertices.emplace_back( vertex );
}
void app::Model::bind( ) {
    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_COLOR_ARRAY );
    static constexpr auto stride = sizeof( Vertex );
    glVertexPointer( Vertex::Position_Size, GL_FLOAT, stride, &data_->vertices[0].xy );
    glColorPointer( Vertex::Color_Size, GL_FLOAT, stride, &data_->vertices[0].rgb );
}
void app::Model::draw( ) {
    glDrawElements( GL_TRIANGLES, data_->elements.size( ), GL_UNSIGNED_BYTE, data_->elements.data( ));
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
