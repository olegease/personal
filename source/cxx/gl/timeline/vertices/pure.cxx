#define APP_TYPE_PURE
#include "vertices.hxx"
// son8
#include <son8/main.hxx>
#include <son8/core/alias/data.hxx>
#include <son8/core/alias/text.hxx>
#include <son8/cxx/file.hxx>
#include <son8/cxx/func.hxx>
#include <son8/c/byte.hxx>
#include <son8/c/file.hxx>
// son8 wrappers
#include <glfw/son8.hxx>
#include <glad/son8.hxx>
/**
 * Vertices Pure Timeline Examples
 */
#define APP_TIMELINE APP_TIMELINE_ARRAY

// TODO: rewrite it using mostly `c` style coding, like raw loops, no exceptions leaked
// \ `wrap` version should use modern techniques instead

namespace app {

    inline constexpr char const *Timeline_Title = "Pure timeline version: " APP_STRINGIFY_EXPAND( APP_TIMELINE );

    struct App {
        Model model;
        Ptr< GLFWwindow  > window;
        bool isGotoPrinted = false;
        bool isGotoSuccess = false;
    };

    void goto_success( App *appOut ) {
        appOut->isGotoSuccess = true;
        appOut->isGotoPrinted = true;
    }

    bool goto_is_success( App const *app ) { return app->isGotoSuccess; }

    void goto_print_error( App *appOut, char const *message ) {
        if ( appOut->isGotoPrinted ) { return; }
        appOut->isGotoPrinted = true;
        c::puts( message );
    }

}

void son8::main( Args args ) {
    using namespace app;
    bool isGotoPrinted, isGotoSuccess;
    // operations on same stack frame
    App appData;
    // passing to mutation functions
    App *appPtr = &appData;
    // passing to constant functions
    App const *app = appPtr;
    if ( not glfwInit( )) { goto failed_init; }

    appData.window = glfwCreateWindow( 640, 360, Timeline_Title, nullptr, nullptr );

    if ( not appData.window ) { goto failed_window; }

    glfwMakeContextCurrent( app->window );

    if ( not gladLoadGL( reinterpret_cast< GLADloadfunc >( glfwGetProcAddress ))) { goto failed_glad; }

    if ( not appData.model.setup( )) { goto failed_setup; }

    while ( not glfwWindowShouldClose( app->window )) try {
        glfwPollEvents( );
        glClearColor( .125f, .125f, .125f, 1.f );
        glClear( GL_COLOR_BUFFER_BIT );
        appData.model.bind( );
        appData.model.draw( );
        appData.model.free( );
        glfwSwapBuffers( app->window );
    } catch ( ... ) { goto failed_while; }

    goto_success( appPtr );

failed_while:
    appData.model.clean( );
    goto_print_error( appPtr, "failed while loop" );

failed_setup:
    goto_print_error( appPtr, "failed model setup" );

failed_glad:
    goto_print_error( appPtr, "failed to load glad" );
    glfwDestroyWindow( app->window );

failed_window:
    goto_print_error( appPtr, "failed to create window" );
    glfwTerminate( );

failed_init:
    goto_print_error( appPtr, "failed to initialize glfw" );

    if ( not goto_is_success( app )) Exit::failure( );
}

namespace app {

    struct Vertex {
        typedef GLdouble Position_Type;
        typedef GLfloat Color_Type;
        struct Data_XY {
            Position_Type x, y;
        };
        struct Data_RGB {
            Color_Type r, g, b;
        };
        typedef Data_XY Position;
        typedef Data_RGB Color;
        static constexpr GLenum Position_Type_GL = GL_DOUBLE;
        static constexpr GLenum Color_Type_GL = GL_FLOAT;
        static constexpr auto Position_Size = 2;
        static constexpr auto Color_Size = 3;
        Position xy;
        Color rgb;
    };

    static constexpr auto Data_Vertices_Count_ = 4;
    static constexpr auto Data_Indices_Count_ = 6;
    typedef Vertex Data_Vertices_[Data_Vertices_Count_];
    typedef GLubyte Data_Index;
    typedef Data_Index Data_Indices_[Data_Indices_Count_];
    typedef GLuint Data_List;

    struct Data final {
        Data_Vertices_ vertices_ = {
            { .xy = {-.8,-.8}, .rgb = { 1.f, 0.f, 0.f }},
            { .xy = { .8,-.8}, .rgb = { 0.f, 1.f, 0.f }},
            { .xy = { .8, .8}, .rgb = { 0.f, 0.f, 1.f }},
            { .xy = {-.8, .8}, .rgb = { 1.f, 1.f, 1.f }},
        };
        Data_Indices_ indices_ = {
            0u, 1u, 2u,
            2u, 3u, 0u
        };
#if 0
        auto raw_vertices( ) const -> Ref< Vertices_ > { return vertices_; }
        auto raw_indices( ) const -> Ref< Indices_ > { return indices_; }
        auto xy( Index index ) const -> Vertex::Position const & { return vertices_[index].xy; }
        auto xy_v( Index index ) const -> Vertex::Position_Type const * { return &xy( index ).x; }
        auto rgb( Index index ) const -> Vertex::Color const & { return vertices_[index].rgb; }
        auto rgb_v( Index index ) const -> Vertex::Color_Type const * { return &rgb( index ).r; }
#endif
#if APP_TIMELINE == APP_TIMELINE_LIST
        Data_List list;
#elif APP_TIMELINE == APP_TIMELINE_ARRAY
        Vertex::Position *positions;
        Vertex::Color *colors;
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
    }; // struct Data

    Vertex::Color_Type const *data_rgb_v( Data const *data, Size i ) {
        Data_Index index = data->indices_[i];
        return &data->vertices_[index].rgb.r;
    }
    Vertex::Position_Type const *data_xy_v( Data const *data, Size i ) {
        Data_Index index = data->indices_[i];
        return &data->vertices_[index].xy.x;
    }

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
    for ( Size i = 0; i < Data_Indices_Count_; ++i ) {
        glColor3fv( data_rgb_v( data_, i ));
        glVertex2dv( data_xy_v( data_, i ));
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
    for ( Size i = 0; i < Data_Indices_Count_; ++i ) {
        glColor3fv( data_rgb_v( data_, i ));
        glVertex2dv( data_xy_v( data_, i ));
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
    Size size = Data_Indices_Count_;
    data_->positions = ( Vertex::Position *)c::malloc( sizeof(Vertex::Position) * size );
    data_->colors = ( Vertex::Color *)c::malloc( sizeof(Vertex::Color) * size );

    if ( not data_->positions or not data_->colors ) return false;

    for ( Size i = 0u; i < size; ++i ) {
        c::memcpy( data_->positions + i, data_xy_v( data_, i ), sizeof( Vertex::Position ));
        c::memcpy( data_->colors + i, data_rgb_v( data_, i ), sizeof( Vertex::Color ));
    }

    return true;
} catch ( ... ) { return false; }

void app::Model::bind( ) {
    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_COLOR_ARRAY );
    glVertexPointer( Vertex::Position_Size, Vertex::Position_Type_GL, Zero, data_->positions );
    glColorPointer( Vertex::Color_Size, Vertex::Color_Type_GL, Zero, data_->colors );
    APP_ASSERT( glGetError( ) == GL_NO_ERROR );
}

void app::Model::draw( ) {
    glDrawArrays( GL_TRIANGLES, Zero, 6);
    APP_ASSERT( glGetError( ) == GL_NO_ERROR );
}

void app::Model::free( ) {
    glDisableClientState( GL_VERTEX_ARRAY );
    glDisableClientState( GL_COLOR_ARRAY );
    APP_ASSERT( glGetError( ) == GL_NO_ERROR );
}

void app::Model::clean( ) {
    c::free( data_->positions );
    c::free( data_->colors );
}
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
    glVertexPointer( Vertex::Position_Size, Vertex::Position_Type_GL, stride, &data_->vertices[Zero].xy );
    glColorPointer( Vertex::Color_Size, Vertex::Color_Type_GL, stride, &data_->vertices[Zero].rgb );
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
    glVertexPointer( Vertex::Position_Size, Vertex::Position_Type_GL, stride, &data_->vertices[0].xy );
    glColorPointer( Vertex::Color_Size, Vertex::Color_Type_GL, stride, &data_->vertices[0].rgb );
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
    glVertexPointer( Vertex::Position_Size, Vertex::Position_Type_GL, sizeof( Vertex), (void *)offsetof( Vertex, xy ));
    glColorPointer( Vertex::Color_Size, Vertex::Color_Type_GL, sizeof( Vertex), (void *)offsetof( Vertex, rgb ));
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
