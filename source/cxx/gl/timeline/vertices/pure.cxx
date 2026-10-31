#define APP_TYPE_PURE
#include "vertices.hxx"
// son8
#include <son8/main.hxx>
#include <son8/c/byte.hxx>
#include <son8/c/file.hxx>
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

    enum Flag : unsigned {
        Flag_Goto_Printed_Bit = 1u << 0u,
        Flag_Goto_Success_Bit = 1u << 1u,
        Flag_Goto_All_Bit = 0b11u,
    };

    struct App {
        Model model;
        GLFWwindow *window{ };
        unsigned flags{ };
    };

    void goto_success( App *appOut ) {
        appOut->flags = Flag_Goto_All_Bit;
    }

    bool goto_is_success( App const *app ) { return app->flags & Flag_Goto_Success_Bit; }

    void goto_print_error( App *appOut, char const *message ) {
        if ( appOut->flags & Flag_Goto_Printed_Bit ) { return; }
        appOut->flags |= Flag_Goto_Printed_Bit;
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
        typedef GLfloat Position_Type;
        typedef GLubyte Color_Type;
        struct Data_XY {
            Position_Type x, y;
        };
        struct Data_RGB {
            Color_Type r, g, b, a;
        };
        typedef Data_XY Position;
        typedef Data_RGB Color;
        static constexpr GLenum Position_Type_GL = GL_FLOAT;
        static constexpr GLenum Color_Type_GL = GL_UNSIGNED_BYTE;
        static constexpr auto Position_Size = 2;
        static constexpr auto Color_Size = 4;
        Position xy;
        Color rgba;
        GLubyte pad[4];
    };
    static_assert( sizeof(Vertex) == 16
        , "Vertex must be exactly 16 bytes for cache line alignment" );
    static_assert( offsetof( Vertex, xy ) == 0
        , "xy offset must be 0" );
    static_assert( offsetof( Vertex, rgba ) == 8
        , "rgba offset must be 8" );
    static_assert( offsetof( Vertex, pad ) == 12
        , "pad offset must be 12");

    static constexpr auto Data_Vertices_Count_ = 4;
    static constexpr auto Data_Indices_Count_ = 6;
    typedef Vertex Data_Vertices_[Data_Vertices_Count_];
    typedef GLubyte Data_Index;
    typedef Data_Index Data_Indices_[Data_Indices_Count_];
    typedef GLuint Data_List;
    typedef GLuint Data_Buffer;

    struct Data {
        Data_Vertices_ vertices_ = {
            { .xy = {-.8,-.8}, .rgba = {255u, Zero, Zero, 255u }},
            { .xy = { .8,-.8}, .rgba = {Zero, 255u, Zero, 255u }},
            { .xy = { .8, .8}, .rgba = {Zero, Zero, 255u, 255u }},
            { .xy = {-.8, .8}, .rgba = {255u, 255u, 255u, 255u }},
        };
        Data_Indices_ indices_ = {
            0u, 1u, 2u,
            2u, 3u, 0u
        };
#if APP_TIMELINE == APP_TIMELINE_LIST
        Data_List list;
#elif APP_TIMELINE == APP_TIMELINE_ARRAY
        Vertex::Position *positions;
        Vertex::Color *colors;
#elif APP_TIMELINE == APP_TIMELINE_INTERLEAVE
        Vertex *vertices;
#elif APP_TIMELINE == APP_TIMELINE_ELEMENT
        Vertex *vertices;
        Data_Index *indices;
#elif APP_TIMELINE == APP_TIMELINE_BUFFER
        static constexpr auto Index_VBO = 0;
        static constexpr auto Index_EBO = 1;
        static constexpr auto Buffer_Count = 2;
        Data_Buffer buffers[Buffer_Count];
        Vertex *vertices;
        Data_Index *indices;
        // NOTE: cache counts as data would be passed to buffer object and cleared
        Size indicesCount = 0;
        Size verticesCount = 0;
#endif
    }; // struct Data

    Vertex::Color_Type const *data_rgba_v( Data const *data, Size i ) {
        Data_Index index = data->indices_[i];
        return &data->vertices_[index].rgba.r;
    }
    Vertex::Position_Type const *data_xy_v( Data const *data, Size i ) {
        Data_Index index = data->indices_[i];
        return &data->vertices_[index].xy.x;
    }
#if APP_TIMELINE == APP_TIMELINE_BUFFER
    Data_Buffer data_buffer_vbo( Data const *data ) { return data->buffers[Data::Index_VBO]; }
    Data_Buffer data_buffer_ebo( Data const *data ) { return data->buffers[Data::Index_EBO]; }
#endif

}
// Vertices RAII
app::Model::Model( ) : data_{ new Data } { }
app::Model::~Model( ) { delete data_; }
// timelines
#if APP_TIMELINE == APP_TIMELINE_IMMEDIATE
bool app::Model::setup( ) { return true; }
void app::Model::bind( ) { }

void app::Model::draw( ) {
    glBegin( GL_TRIANGLES );
    for ( Size i = 0; i < Data_Indices_Count_; ++i ) {
        glColor4ubv( data_rgba_v( data_, i ));
        glVertex2fv( data_xy_v( data_, i ));
    }
    glEnd( );
    APP_ASSERT( glGetError( ) == GL_NO_ERROR );
}

void app::Model::free( ) { }

void app::Model::clean( ) { }
#elif APP_TIMELINE == APP_TIMELINE_LIST
bool app::Model::setup( ) {
    Ref< Data_List > list = data_->list;
    data_->list = glGenLists( 1 );
    glNewList( list, GL_COMPILE );
    glBegin( GL_TRIANGLES );
    for ( Size i = 0; i < Data_Indices_Count_; ++i ) {
        glColor4ubv( data_rgba_v( data_, i ));
        glVertex2fv( data_xy_v( data_, i ));
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
bool app::Model::setup( ) {
    Size size = Data_Indices_Count_;
    data_->positions = ( Vertex::Position *)c::malloc( sizeof(Vertex::Position) * size );
    data_->colors = ( Vertex::Color *)c::malloc( sizeof(Vertex::Color) * size );

    if ( not data_->positions or not data_->colors ) return false;

    for ( Size i = 0u; i < size; ++i ) {
        c::memcpy( data_->positions + i, data_xy_v( data_, i ), sizeof( Vertex::Position ));
        c::memcpy( data_->colors + i, data_rgba_v( data_, i ), sizeof( Vertex::Color ));
    }

    return true;
}

void app::Model::bind( ) {
    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_COLOR_ARRAY );
    glVertexPointer( Vertex::Position_Size, Vertex::Position_Type_GL, Zero, data_->positions );
    glColorPointer( Vertex::Color_Size, Vertex::Color_Type_GL, Zero, data_->colors );
    APP_ASSERT( glGetError( ) == GL_NO_ERROR );
}

void app::Model::draw( ) {
    glDrawArrays( GL_TRIANGLES, Zero, Data_Indices_Count_);
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
bool app::Model::setup( ) {
    Size size = Data_Indices_Count_;
    data_->vertices = (Vertex *)c::malloc( sizeof(Vertex) * size );

    if ( not data_->vertices ) return false;

    for ( Size i = 0u; i < size; ++i ) {
        c::memcpy( data_->vertices + i, &data_->vertices_[data_->indices_[i]], sizeof(Vertex) );
    }

    return true;
}

void app::Model::bind( ) {
    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_COLOR_ARRAY );
    static constexpr auto stride = sizeof( Vertex );
    glVertexPointer( Vertex::Position_Size, Vertex::Position_Type_GL, stride, &data_->vertices[Zero].xy );
    glColorPointer( Vertex::Color_Size, Vertex::Color_Type_GL, stride, &data_->vertices[Zero].rgba );
    APP_ASSERT( glGetError( ) == GL_NO_ERROR );
}

void app::Model::draw( ) {
    glDrawArrays( GL_TRIANGLES, Zero, Data_Indices_Count_ );
    APP_ASSERT( glGetError( ) == GL_NO_ERROR );
}

void app::Model::free( ) {
    glDisableClientState( GL_VERTEX_ARRAY );
    glDisableClientState( GL_COLOR_ARRAY );
    APP_ASSERT( glGetError( ) == GL_NO_ERROR );
}

void app::Model::clean( ) {
    c::free( data_->vertices );
}
#elif APP_TIMELINE == APP_TIMELINE_ELEMENT
bool app::Model::setup( ) {
    data_->vertices = (Vertex *)c::malloc( sizeof(Vertex) * Data_Vertices_Count_ );
    data_->indices = (Data_Index *)c::malloc( sizeof(Data_Index) * Data_Indices_Count_ );

    if ( not data_->vertices or not data_->indices ) { return false; }

    c::memcpy( data_->vertices, &data_->vertices_[0], sizeof( data_->vertices_ ));
    c::memcpy( data_->indices, &data_->indices_[0], sizeof( data_->indices_ ));

    return true;
}

void app::Model::bind( ) {
    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_COLOR_ARRAY );
    static constexpr auto stride = sizeof( Vertex );
    glVertexPointer( Vertex::Position_Size, Vertex::Position_Type_GL, stride, &data_->vertices[0].xy );
    glColorPointer( Vertex::Color_Size, Vertex::Color_Type_GL, stride, &data_->vertices[0].rgba );
    APP_ASSERT( glGetError( ) == GL_NO_ERROR );
}

void app::Model::draw( ) {
    glDrawElements( GL_TRIANGLES, Data_Indices_Count_, GL_UNSIGNED_BYTE, data_->indices);
    APP_ASSERT( glGetError( ) == GL_NO_ERROR );
}

void app::Model::free( ) {
    glDisableClientState( GL_VERTEX_ARRAY );
    glDisableClientState( GL_COLOR_ARRAY );
    APP_ASSERT( glGetError( ) == GL_NO_ERROR );
}

void app::Model::clean( ) {
    c::free( data_->vertices );
    c::free( data_->indices );
}
#elif APP_TIMELINE == APP_TIMELINE_BUFFER
bool app::Model::setup( ) {
    data_->vertices = (Vertex *)c::malloc( sizeof(Vertex) * Data_Vertices_Count_ );
    data_->indices = (Data_Index *)c::malloc( sizeof(Data_Index) * Data_Indices_Count_ );

    if ( not data_->vertices or not data_->indices ) { return false; }

    c::memcpy( data_->vertices, &data_->vertices_[0], sizeof( data_->vertices_ ));
    c::memcpy( data_->indices, &data_->indices_[0], sizeof( data_->indices_ ));
    data_->verticesCount = Data_Vertices_Count_;
    data_->indicesCount = Data_Indices_Count_;
    glGenBuffers( Data::Buffer_Count, data_->buffers );
    glBindBuffer( GL_ARRAY_BUFFER, data_buffer_vbo( data_ ));
    glBufferData( GL_ARRAY_BUFFER, data_->verticesCount * sizeof( Vertex ), data_->vertices, GL_STATIC_DRAW );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, data_buffer_ebo( data_ ));
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, data_->indicesCount * sizeof( Data_Index ), data_->indices, GL_STATIC_DRAW );
    c::free( data_->vertices );
    c::free( data_->indices );
    APP_ASSERT( glGetError( ) == GL_NO_ERROR );

    return true;
}

void app::Model::bind( ) {
    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_COLOR_ARRAY );
    glBindBuffer( GL_ARRAY_BUFFER, data_buffer_vbo( data_ ));
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, data_buffer_ebo( data_ ));
    glVertexPointer( Vertex::Position_Size, Vertex::Position_Type_GL, sizeof( Vertex), (void *)offsetof( Vertex, xy ));
    glColorPointer( Vertex::Color_Size, Vertex::Color_Type_GL, sizeof( Vertex), (void *)offsetof( Vertex, rgba ));
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
    glDeleteBuffers( Data::Buffer_Count, data_->buffers );
    APP_ASSERT( glGetError( ) == GL_NO_ERROR );
}
#else
// TODO: shaders vertex array object, named buffers and so on
bool app::Model::setup( ) { return true }

void app::Model::bind( ) { }

void app::Model::draw( ) { }

void app::Model::free( ) { }

void app::Model::clean( ) { }
#endif // timelines
