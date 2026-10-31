#define APP_TYPE_PURE
#include "vertices.hxx"
// son8
#include <son8/main.hxx>
#include <son8/core/alias/data.hxx>
#include <son8/core/alias/text.hxx>
#include <son8/cxx/file.hxx>
#include <son8/cxx/func.hxx>
// son8 wrappers
#include <glfw/son8.hxx>
#include <glad/son8.hxx>
/**
 * Vertices Pure Timeline Examples
 */
#define APP_TIMELINE APP_TIMELINE_BUFFER

// TODO: rewrite it using mostly `c` style coding, like raw loops, no exceptions leaked
// \ `wrap` version should use modern techniques instead

namespace app {

    inline constexpr char const *Timeline_Title = "Pure timeline version: " APP_STRINGIFY_EXPAND( APP_TIMELINE );

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

    appOut.window = glfwCreateWindow( 640, 360, Timeline_Title, nullptr, nullptr );

    if ( not app.window ) { goto failed_window; }

    glfwMakeContextCurrent( app.window );

    if ( not gladLoadGL( reinterpret_cast< GLADloadfunc >( glfwGetProcAddress ))) { goto failed_glad; }

    if ( not appOut.model.setup( )) { goto failed_setup; }

    while ( not glfwWindowShouldClose( app.window )) try {
        glfwPollEvents( );
        glClearColor( .125f, .125f, .125f, 1.f );
        glClear( GL_COLOR_BUFFER_BIT );
        appOut.model.bind( );
        appOut.model.draw( );
        appOut.model.free( );
        glfwSwapBuffers( app.window );
    } catch ( ... ) { goto failed_while; }

    appOut.goto_success( );

failed_while:
    appOut.model.clean( );
    appOut.goto_print_error( "failed while loop" );

failed_setup:
    appOut.goto_print_error( "failed model setup" );

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

    struct Vertex {
        static constexpr auto Position_Size = 2;
        static constexpr auto Color_Size = 3;
        using Position = Flat< GLfloat, Position_Size >;
        using Color = Flat< GLfloat, Color_Size >;
        Position xy;
        Color rgb;
    };

    class Data final {
        static constexpr auto Vertices_Count_ = 4;
        static constexpr auto Indices_Count_ = 6;
        using Vertices_ = Flat< Vertex, Vertices_Count_ >;
        using Index_ = GLubyte;
        using Indices_ = Flat< Index_, Indices_Count_ >;
        Vertices_ vertices_{{
            { .xy = {-.8f,-.8f}, .rgb = { 1.f, 0.f, 0.f }},
            { .xy = { .8f,-.8f}, .rgb = { 0.f, 1.f, 0.f }},
            { .xy = { .8f, .8f}, .rgb = { 0.f, 0.f, 1.f }},
            { .xy = {-.8f, .8f}, .rgb = { 1.f, 1.f, 1.f }},
        }};
        Indices_ indices_{{
            0u, 1u, 2u,
            2u, 3u, 0u
        }};
    public:
        using Index = Index_;
        auto raw_vertices( ) const -> Ref< Vertices_ > { return vertices_; }
        auto raw_indices( ) const -> Ref< Indices_ > { return indices_; }
        auto xy( Index index ) const -> Ref< Vertex::Position > { return vertices_[index].xy; }
        auto xy_v( Index index ) const -> Ptr< GLfloat const > { return xy( index ).data( ); }
        auto rgb( Index index ) const -> Ref< Vertex::Color > { return vertices_[index].rgb; }
        auto rgb_v( Index index ) const -> Ptr< GLfloat const > { return rgb( index ).data( ); }
#if APP_TIMELINE == APP_TIMELINE_LIST
        using List = GLuint;
        List list;
#elif APP_TIMELINE == APP_TIMELINE_ARRAY
        Grow< Vertex::Position > positions;
        Grow< Vertex::Color > colors;
#elif APP_TIMELINE == APP_TIMELINE_INTERLEAVE
        Grow< Vertex > vertices;
#elif APP_TIMELINE == APP_TIMELINE_ELEMENT
        Grow< Vertex > vertices;
        Grow< Index > indices;
#elif APP_TIMELINE == APP_TIMELINE_BUFFER
        using Buffer = GLuint;
        static constexpr auto Index_VBO = 0;
        static constexpr auto Index_EBO = 1;
        static constexpr auto Buffer_Count = 2;
        Flat< Buffer, Buffer_Count > buffers;
        auto vbo( ) const -> Buffer { return buffers[Index_VBO]; }
        auto ebo( ) const -> Buffer { return buffers[Index_EBO]; }
        Grow< Vertex > vertices;
        Grow< Index > indices;
        // NOTE: cache counts as data would be passed to buffer object and cleared
        Size indicesCount = 0;
        Size verticesCount = 0;
#endif
    };
}
// Vertices RAII
app::Model::Model( ) : data_{ new Data } { }
app::Model::~Model( ) { delete data_; }
// timelines
#if APP_TIMELINE == APP_TIMELINE_IMMEDIATE
bool app::Model::setup( ) try { return true; } catch ( ... ) { return false; }
void app::Model::bind( ) { }

void app::Model::draw( ) {
    glBegin( GL_TRIANGLES );
    for ( auto index : data_->raw_indices( )) {
        glColor3fv( data_->rgb_v( index ));
        glVertex2fv( data_->xy_v( index ));
    }
    glEnd( );
    APP_ASSERT( glGetError( ) == GL_NO_ERROR );
}

void app::Model::free( ) { }

void app::Model::clean( ) { }
#elif APP_TIMELINE == APP_TIMELINE_LIST
bool app::Model::setup( ) {
    Ref< Data::List > list = data_->list;
    data_->list = glGenLists( 1 );
    glNewList( list, GL_COMPILE );
    glBegin( GL_TRIANGLES );
    for ( auto index : data_->raw_indices( )) {
        glColor3fv( data_->rgb_v( index ));
        glVertex2fv( data_->xy_v( index ));
    }
    glEnd( );
    glEndList( );
    APP_ASSERT( glGetError( ) == GL_NO_ERROR );
    return true;
}

void app::Model::bind( ) { }

void app::Model::draw( ) {
    glCallList( data_->list );
    APP_ASSERT( glGetError( ) == GL_NO_ERROR );
}

void app::Model::free( ) { }

void app::Model::clean( ) {
    glDeleteLists( data_->list, 1 );
    APP_ASSERT( glGetError( ) == GL_NO_ERROR );
}
#elif APP_TIMELINE == APP_TIMELINE_ARRAY
bool app::Model::setup( ) try {
    Size size = data_->raw_indices( ).size( );
    data_->colors.reserve( size );
    data_->positions.reserve( size );
    for ( auto index : data_->raw_indices( )) {
        data_->colors.emplace_back( data_->rgb( index ));
        data_->positions.emplace_back( data_->xy( index ));
    }
    return true;
} catch ( ... ) { return false; }

void app::Model::bind( ) {
    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_COLOR_ARRAY );
    glVertexPointer( Vertex::Position_Size, GL_FLOAT, Zero, data_->positions.data( ));
    glColorPointer( Vertex::Color_Size, GL_FLOAT, Zero, data_->colors.data( ));
    APP_ASSERT( glGetError( ) == GL_NO_ERROR );
}

void app::Model::draw( ) {
    glDrawArrays( GL_TRIANGLES, Zero, data_->positions.size( ));
    APP_ASSERT( glGetError( ) == GL_NO_ERROR );
}

void app::Model::free( ) {
    glDisableClientState( GL_VERTEX_ARRAY );
    glDisableClientState( GL_COLOR_ARRAY );
    APP_ASSERT( glGetError( ) == GL_NO_ERROR );
}

void app::Model::clean( ) { }
#elif APP_TIMELINE == APP_TIMELINE_INTERLEAVE
bool app::Model::setup( ) try {
    data_->vertices.reserve( data_->raw_indices( ).size( ));
    for ( auto index : data_->raw_indices( )) {
        data_->vertices.emplace_back( Vertex{ data_->xy( index ), data_->rgb( index )});
    }
    return true;
} catch ( ... ) { return false; }

void app::Model::bind( ) {
    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_COLOR_ARRAY );
    static constexpr auto stride = sizeof( Vertex );
    glVertexPointer( Vertex::Position_Size, GL_FLOAT, stride, &data_->vertices[Zero].xy );
    glColorPointer( Vertex::Color_Size, GL_FLOAT, stride, &data_->vertices[Zero].rgb );
    APP_ASSERT( glGetError( ) == GL_NO_ERROR );
}

void app::Model::draw( ) {
    glDrawArrays( GL_TRIANGLES, Zero, data_->vertices.size( ));
    APP_ASSERT( glGetError( ) == GL_NO_ERROR );
}

void app::Model::free( ) {
    glDisableClientState( GL_VERTEX_ARRAY );
    glDisableClientState( GL_COLOR_ARRAY );
    APP_ASSERT( glGetError( ) == GL_NO_ERROR );
}

void app::Model::clean( ) { }
#elif APP_TIMELINE == APP_TIMELINE_ELEMENT
bool app::Model::setup( ) try {
    data_->vertices.reserve( data_->raw_vertices( ).size( ));
    for ( Ref< Vertex > vertex : data_->raw_vertices( )) {
        data_->vertices.emplace_back( vertex );
    }
    data_->indices.resize( data_->raw_indices( ).size( ));
    cxx::copy( data_->raw_indices( ).begin( ), data_->raw_indices( ).end( ), data_->indices.begin( ));
    return true;
} catch ( ... ) { return false; }

void app::Model::bind( ) {
    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_COLOR_ARRAY );
    static constexpr auto stride = sizeof( Vertex );
    glVertexPointer( Vertex::Position_Size, GL_FLOAT, stride, &data_->vertices[0].xy );
    glColorPointer( Vertex::Color_Size, GL_FLOAT, stride, &data_->vertices[0].rgb );
    APP_ASSERT( glGetError( ) == GL_NO_ERROR );
}

void app::Model::draw( ) {
    glDrawElements( GL_TRIANGLES, data_->indices.size( ), GL_UNSIGNED_BYTE, data_->indices.data( ));
    APP_ASSERT( glGetError( ) == GL_NO_ERROR );
}

void app::Model::free( ) {
    glDisableClientState( GL_VERTEX_ARRAY );
    glDisableClientState( GL_COLOR_ARRAY );
    APP_ASSERT( glGetError( ) == GL_NO_ERROR );
}

void app::Model::clean( ) { }
#elif APP_TIMELINE == APP_TIMELINE_BUFFER
bool app::Model::setup( ) try {
    data_->verticesCount = data_->raw_vertices( ).size( );
    data_->indicesCount = data_->raw_indices( ).size( );
    data_->vertices.resize( data_->verticesCount );
    data_->indices.resize( data_->indicesCount );
    cxx::copy( data_->raw_vertices( ).begin( ), data_->raw_vertices( ).end( ), data_->vertices.begin( ));
    cxx::copy( data_->raw_indices( ).begin( ), data_->raw_indices( ).end( ), data_->indices.begin( ));
    glGenBuffers( Data::Buffer_Count, data_->buffers.data( ));
    glBindBuffer( GL_ARRAY_BUFFER, data_->vbo( ));
    glBufferData( GL_ARRAY_BUFFER, data_->vertices.size( ) * sizeof( Vertex ), data_->vertices.data( ), GL_STATIC_DRAW );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, data_->ebo( ));
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, data_->indices.size() * sizeof( Data::Index ), data_->indices.data( ), GL_STATIC_DRAW );
    data_->vertices.clear( );
    data_->indices.clear( );
    APP_ASSERT( glGetError( ) == GL_NO_ERROR );
    return true;
} catch ( ... ) { return false; }

void app::Model::bind( ) {
    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_COLOR_ARRAY );
    glBindBuffer( GL_ARRAY_BUFFER, data_->vbo( ));
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, data_->ebo( ));
    glVertexPointer( Vertex::Position_Size, GL_FLOAT, sizeof( Vertex), (void *)offsetof( Vertex, xy ));
    glColorPointer( Vertex::Color_Size, GL_FLOAT, sizeof( Vertex), (void *)offsetof( Vertex, rgb ));
    APP_ASSERT( glGetError( ) == GL_NO_ERROR );
}

void app::Model::draw( ) {
    glDrawElements(GL_TRIANGLES, data_->indicesCount, GL_UNSIGNED_BYTE, (void *)Zero);
    APP_ASSERT( glGetError( ) == GL_NO_ERROR );
}

void app::Model::free( ) {
    glDisableClientState( GL_VERTEX_ARRAY );
    glDisableClientState( GL_COLOR_ARRAY );
    glBindBuffer( GL_ARRAY_BUFFER, Zero );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, Zero );
    APP_ASSERT( glGetError( ) == GL_NO_ERROR );
}

void app::Model::clean( ) {
    glDeleteBuffers( Data::Buffer_Count, data_->buffers.data( ));
    APP_ASSERT( glGetError( ) == GL_NO_ERROR );
}
#else
// TODO: vertex array object, named buffers and so on
bool app::Model::setup( ) try { } catch ( ... ) { return false; }
void app::Model::bind( ) { }
void app::Model::draw( ) { }
void app::Model::free( ) { }
//void app::Model::clean( ) { }
#endif // timelines
