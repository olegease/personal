#define APP_TYPE_PURE
//#define APP_TIMELINE APP_TIMELINE_BUFFER
#include "vertices.hxx"
// `son8`
#include <son8/main.hxx>
#include <son8/c/byte.hxx>
#include <son8/c/char.hxx>
#include <son8/c/file.hxx>
// `son8` pure wrappers
#include <glfw/son8.hxx>
#include <glad/son8.hxx>
/**
 * Vertices Pure Timeline Examples
 */

// TODO: rewrite it using mostly `c` style coding, like raw loops, no exceptions leaked
// \ `wrap` version should use modern techniques instead

namespace app {

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
    std::string title = std::string{ "Pure " } + Window_Title;
    // operations on same stack frame
    App appData;
    // passing to mutation functions
    App *appPtr = &appData;
    // passing to constant functions
    App const *app = appPtr;
    if ( not glfwInit( )) { goto failed_init; }

    glfwDefaultWindowHints( );
    glfwWindowHint( GLFW_RESIZABLE, GLFW_FALSE );

    appData.window = glfwCreateWindow( Window_W, Window_H, title.c_str( ), nullptr, nullptr );

    if ( not appData.window ) { goto failed_window; }
    // NOTE: fix for tiled window managers
    glfwSetWindowSize( app->window, Window_W, Window_H );

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

    if ( not goto_is_success( app )) { Exit::failure( ); };
}

namespace app {

    typedef GLfloat Vertex_Position_Type;
    typedef GLubyte Vertex_Color_Type;

    static constexpr GLenum Vertex_Position_Type_GL = GL_FLOAT;
    static constexpr GLenum Vertex_Color_Type_GL = GL_UNSIGNED_BYTE;
    static constexpr auto Vertex_Position_Size = 2;
    static constexpr auto Vertex_Color_Size = 4;
    static constexpr auto Vertex_Pad_Size = 4;

    typedef Vertex_Position_Type Vertex_Position[Vertex_Position_Size];
    typedef Vertex_Color_Type Vertex_Color[Vertex_Color_Size];

    struct Vertex {
        Vertex_Position xy;
        Vertex_Color rgba;
        GLubyte pad[Vertex_Pad_Size];
    };
    static_assert( sizeof(Vertex) == 16
        , "Vertex must be exactly 16 bytes for cache line alignment" );
    static_assert( offsetof( Vertex, xy ) == 0
        , "xy offset must be 0" );
    static_assert( offsetof( Vertex, rgba ) == 8
        , "rgba offset must be 8" );
    static_assert( offsetof( Vertex, pad ) == 12
        , "pad offset must be 12");

    static constexpr auto Data_Vertices_Count_ = 14;
    static constexpr auto Data_Indices_Count_ = 36;
    typedef Vertex Data_Vertices_[Data_Vertices_Count_];
    typedef GLubyte Data_Index;
    typedef Data_Index Data_Indices_[Data_Indices_Count_];
    typedef GLuint Data_List;
    typedef GLuint Data_Buffer;

    struct Data {
        Data_Vertices_ vertices_ = {
            { .xy = {0.0f,-.2f}, .rgba = { Zero,Zero,Zero,255u }}, // 0
            { .xy = {0.0f,+.2f}, .rgba = { Zero,255u,Zero,255u }}, // 1
            { .xy = {+.2f,+.2f}, .rgba = { 255u,255u,Zero,255u }}, // 2
            { .xy = {+.2f,-.2f}, .rgba = { 255u,Zero,Zero,255u }}, // 3
            { .xy = {-.4f,-.2f}, .rgba = { 255u,Zero,255u,255u }}, // 4
            { .xy = {-.2f,-.2f}, .rgba = { Zero,Zero,255u,255u }}, // 5
            { .xy = {-.2f,+.2f}, .rgba = { Zero,255u,255u,255u }}, // 6
            { .xy = {-.4f,+.2f}, .rgba = { 255u,255u,255u,255u }}, // 7
            { .xy = {+.4f,-.2f}, .rgba = { 255u,Zero,255u,255u }}, // 8
            { .xy = {+.4f,+.2f}, .rgba = { 255u,255u,255u,255u }}, // 9
            { .xy = {+.2f,-.6f}, .rgba = { 255u,Zero,255u,255u }}, // 10
            { .xy = {0.0f,-.6f}, .rgba = { Zero,Zero,255u,255u }}, // 11
            { .xy = {0.0f,+.6f}, .rgba = { Zero,255u,255u,255u }}, // 12
            { .xy = {+.2f,+.6f}, .rgba = { 255u,255u,255u,255u }}, // 13
        };
        Data_Indices_ indices_ = {
            // back
             0u, 3u, 2u,
             0u, 2u, 1u,
            // front
             4u, 5u, 6u,
             4u, 6u, 7u,
            // left
             5u, 0u, 1u,
             5u, 1u, 6u,
            // right
             3u, 8u, 9u,
             3u, 9u, 2u,
            // bottom
            11u,10u, 3u,
            11u, 3u, 0u,
            // top
             1u, 2u,13u,
             1u,13u,12u
        };
#if APP_TIMELINE == APP_TIMELINE_LIST
        Data_List list;
#elif APP_TIMELINE == APP_TIMELINE_ARRAY
        Vertex_Position *positions;
        Vertex_Color *colors;
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
#elif APP_TIMELINE == APP_TIMELINE_SHADER
        typedef GLuint Program;
        typedef GLuint Attrib;
        static constexpr Attrib Attrib_Pos = 0;
        static constexpr Attrib Attrib_Color = 1;
        Program idProgram;
        static constexpr auto Index_VBO = 0;
        static constexpr auto Index_EBO = 1;
        static constexpr auto Buffer_Count = 2;
        Data_Buffer buffers[Buffer_Count];
        Vertex *vertices;
        Data_Index *indices;
        // NOTE: cache counts as data would be passed to buffer object and cleared
        Size indicesCount = 0;
        Size verticesCount = 0;
#elif APP_TIMELINE == APP_TIMELINE_OBJECT
        typedef GLuint Object;
        typedef GLuint Program;
        typedef GLuint Attrib;
        Object object;
        Program idProgram;
        static constexpr Attrib Location_Pos = 0;
        static constexpr Attrib Location_Color = 1;
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

    Vertex_Color_Type const *data_color_v( Data const *data, Size i ) {
        Data_Index index = data->indices_[i];
        return &data->vertices_[index].rgba[0];
    }
    Vertex_Position_Type const *data_vertex_v( Data const *data, Size i ) {
        Data_Index index = data->indices_[i];
        return &data->vertices_[index].xy[0];
    }
#if APP_TIMELINE >= APP_TIMELINE_BUFFER
    Data_Buffer data_buffer_vbo( Data const *data ) { return data->buffers[Data::Index_VBO]; }
    Data_Buffer data_buffer_ebo( Data const *data ) { return data->buffers[Data::Index_EBO]; }
#endif

}
// Model RAII
app::Model::Model( ) : data_{ new Data } { }
app::Model::~Model( ) { delete data_; }
// timelines
#if APP_TIMELINE == APP_TIMELINE_IMMEDIATE
bool app::Model::setup( ) { return true; }
void app::Model::bind( ) { }

void app::Model::draw( ) {
    glBegin( GL_TRIANGLES );
    for ( Size i = 0; i < Data_Indices_Count_; ++i ) {
        glColor4ubv( data_color_v( data_, i ));
        glVertex2fv( data_vertex_v( data_, i ));
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
        glColor4ubv( data_color_v( data_, i ));
        glVertex2fv( data_vertex_v( data_, i ));
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
    data_->positions = ( Vertex_Position *)c::malloc( sizeof(Vertex_Position) * size );
    data_->colors = ( Vertex_Color *)c::malloc( sizeof(Vertex_Color) * size );

    if ( not data_->positions or not data_->colors ) return false;

    for ( Size i = 0u; i < size; ++i ) {
        c::memcpy( data_->positions + i, data_vertex_v( data_, i ), sizeof( Vertex_Position ));
        c::memcpy( data_->colors + i, data_color_v( data_, i ), sizeof( Vertex_Color ));
    }

    return true;
}

void app::Model::bind( ) {
    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_COLOR_ARRAY );
    glVertexPointer( Vertex_Position_Size, Vertex_Position_Type_GL, Zero, data_->positions );
    glColorPointer( Vertex_Color_Size, Vertex_Color_Type_GL, Zero, data_->colors );
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
    glVertexPointer( Vertex_Position_Size, Vertex_Position_Type_GL, stride, &data_->vertices[Zero].xy );
    glColorPointer( Vertex_Color_Size, Vertex_Color_Type_GL, stride, &data_->vertices[Zero].rgba );
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
    glVertexPointer( Vertex_Position_Size, Vertex_Position_Type_GL, stride, &data_->vertices[0].xy );
    glColorPointer( Vertex_Color_Size, Vertex_Color_Type_GL, stride, &data_->vertices[0].rgba );
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
    glVertexPointer( Vertex_Position_Size, Vertex_Position_Type_GL, sizeof( Vertex), (void *)offsetof( Vertex, xy ));
    glColorPointer( Vertex_Color_Size, Vertex_Color_Type_GL, sizeof( Vertex), (void *)offsetof( Vertex, rgba ));
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
#elif APP_TIMELINE == APP_TIMELINE_SHADER
bool app::Model::setup( ) {
    char const *codeShared = R"GLSL(#version 120
    varying vec4 vColor;
)GLSL";

    char const *codeVertex = R"GLSL(
    attribute vec2 aPos;
    attribute vec4 aColor;

void main( ) {
    gl_Position = vec4( aPos, 0.0, 1.0 );
    vColor = aColor;
}
    )GLSL";

    char const *codeFragment = R"GLSL(
void main( ) {
    gl_FragColor = vColor.rgba;
}
    )GLSL";

    data_->idProgram = glCreateProgram( );
    Data::Program idProgram = data_->idProgram;
    GLuint idVertex = glCreateShader( GL_VERTEX_SHADER );
    GLuint idFragment = glCreateShader( GL_FRAGMENT_SHADER );
    APP_ASSERT( idProgram and idVertex and idFragment );
    glAttachShader( idProgram, idVertex );
    glAttachShader( idProgram, idFragment );
    char const *sourcesVertex[] = { codeShared, codeVertex };
    char const *sourcesFragment[] = { codeShared, codeFragment };

    glShaderSource( idVertex, 2, sourcesVertex, nullptr );
    glShaderSource( idFragment, 2, sourcesFragment, nullptr );

    glCompileShader( idVertex );
    glCompileShader( idFragment );

    glBindAttribLocation( idProgram, Data::Attrib_Pos, "aPos" );
    glBindAttribLocation( idProgram, Data::Attrib_Color, "aColor" );

    glLinkProgram( idProgram );
    GLint statusProgram = GL_FALSE;
    glGetProgramiv( idProgram, GL_LINK_STATUS, &statusProgram );
    auto info = [&]{
        APP_ASSERT( not statusProgram );
        int v = 0; int f = 1; int p = 2;
        GLint len[3] = { };
        c::fprintf( stderr, "info log:\n" );
        glGetShaderiv( idVertex, GL_INFO_LOG_LENGTH, &len[v] );
        glGetShaderiv( idFragment, GL_INFO_LOG_LENGTH, &len[f] );
        glGetProgramiv( idProgram, GL_INFO_LOG_LENGTH, &len[p] );
        GLint totalLen =  len[v] + len[f] + len[p] + 1;
        GLint writed1 = 0, writed2 = 0;
        char *infoLog = (char *)c::malloc( totalLen );
        infoLog[0] = '\0';
        glGetShaderInfoLog( idVertex, len[v], &writed1, infoLog );
        glGetShaderInfoLog( idFragment, len[f], &writed2, infoLog + writed1 );
        glGetProgramInfoLog( idProgram, len[p], nullptr, infoLog + writed1 + writed2 );
        c::fprintf( stderr, "%s\n", infoLog );
        c::free( infoLog );
    };

    if ( not statusProgram ) { info( ); return false; }
    // clean, not needed after attach
    glDetachShader( idProgram, idVertex );
    glDetachShader( idProgram, idFragment );
    glDeleteShader( idVertex );
    glDeleteShader( idFragment );
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
    glBindBuffer( GL_ARRAY_BUFFER, data_buffer_vbo( data_ ));
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, data_buffer_ebo( data_ ));
    glEnableVertexAttribArray( Data::Attrib_Pos );
    glEnableVertexAttribArray( Data::Attrib_Color );
    glVertexAttribPointer( Data::Attrib_Pos, Vertex_Position_Size, Vertex_Position_Type_GL, GL_FALSE, sizeof( Vertex), (void *)offsetof( Vertex, xy ));
    glVertexAttribPointer( Data::Attrib_Color, Vertex_Color_Size, Vertex_Color_Type_GL, GL_TRUE, sizeof( Vertex), (void *)offsetof( Vertex, rgba ));
    glUseProgram( data_->idProgram );
    APP_ASSERT( glGetError( ) == GL_NO_ERROR );
}

void app::Model::draw( ) {
    glDrawElements(GL_TRIANGLES, data_->indicesCount, GL_UNSIGNED_BYTE, (void *)Zero);
    APP_ASSERT( glGetError( ) == GL_NO_ERROR );
}

void app::Model::free( ) {
    glUseProgram( Zero );
    glDisableVertexAttribArray( Data::Attrib_Color );
    glDisableVertexAttribArray( Data::Attrib_Pos );
    glBindBuffer( GL_ARRAY_BUFFER, Zero );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, Zero );
    APP_ASSERT( glGetError( ) == GL_NO_ERROR );
}

void app::Model::clean( ) {
    glDeleteProgram( data_->idProgram );
    glDeleteBuffers( Data::Buffer_Count, data_->buffers );
    APP_ASSERT( glGetError( ) == GL_NO_ERROR );
}
#elif APP_TIMELINE == APP_TIMELINE_OBJECT
bool app::Model::setup( ) {
    char const *codeShared = R"GLSL(#version 330 core
)GLSL";

    char const *codeVertex = R"GLSL(
    layout( location = 0 ) in vec2 aPos;
    layout( location = 1 ) in vec4 aColor;

    out vec4 vColor;

void main( ) {
    gl_Position = vec4( aPos, 0.0, 1.0 );
    vColor = aColor;
}
    )GLSL";

    char const *codeFragment = R"GLSL(
    in vec4 vColor;
    out vec4 FragColor;
void main( ) {
    FragColor = vColor;
}
    )GLSL";
    data_->idProgram = glCreateProgram( );
    Data::Program idProgram = data_->idProgram;
    GLuint idVertex = glCreateShader( GL_VERTEX_SHADER );
    GLuint idFragment = glCreateShader( GL_FRAGMENT_SHADER );
    APP_ASSERT( idProgram and idVertex and idFragment );
    glAttachShader( idProgram, idVertex );
    glAttachShader( idProgram, idFragment );
    char const *sourcesVertex[] = { codeShared, codeVertex };
    char const *sourcesFragment[] = { codeShared, codeFragment };

    glShaderSource( idVertex, 2, sourcesVertex, nullptr );
    glShaderSource( idFragment, 2, sourcesFragment, nullptr );

    glCompileShader( idVertex );
    glCompileShader( idFragment );

    glLinkProgram( idProgram );
    GLint statusProgram = GL_FALSE;
    glGetProgramiv( idProgram, GL_LINK_STATUS, &statusProgram );
    auto info = [&]{
        APP_ASSERT( not statusProgram );
        int v = 0; int f = 1; int p = 2;
        GLint len[3] = { };
        c::fprintf( stderr, "info log:\n" );
        glGetShaderiv( idVertex, GL_INFO_LOG_LENGTH, &len[v] );
        glGetShaderiv( idFragment, GL_INFO_LOG_LENGTH, &len[f] );
        glGetProgramiv( idProgram, GL_INFO_LOG_LENGTH, &len[p] );
        GLint totalLen =  len[v] + len[f] + len[p] + 1;
        GLint writed1 = 0, writed2 = 0;
        char *infoLog = (char *)c::malloc( totalLen );
        infoLog[0] = '\0';
        glGetShaderInfoLog( idVertex, len[v], &writed1, infoLog );
        glGetShaderInfoLog( idFragment, len[f], &writed2, infoLog + writed1 );
        glGetProgramInfoLog( idProgram, len[p], nullptr, infoLog + writed1 + writed2 );
        c::fprintf( stderr, "%s\n", infoLog );
        c::free( infoLog );
    };

    if ( not statusProgram ) { info( ); return false; }
    // clean, not needed after attach
    glDetachShader( idProgram, idVertex );
    glDetachShader( idProgram, idFragment );
    glDeleteShader( idVertex );
    glDeleteShader( idFragment );

    data_->vertices = (Vertex *)c::malloc( sizeof(Vertex) * Data_Vertices_Count_ );
    data_->indices = (Data_Index *)c::malloc( sizeof(Data_Index) * Data_Indices_Count_ );

    if ( not data_->vertices or not data_->indices ) { return false; }

    c::memcpy( data_->vertices, &data_->vertices_[0], sizeof( data_->vertices_ ));
    c::memcpy( data_->indices, &data_->indices_[0], sizeof( data_->indices_ ));

    data_->verticesCount = Data_Vertices_Count_;
    data_->indicesCount = Data_Indices_Count_;
    glGenVertexArrays( 1, &data_->object );
    glGenBuffers( Data::Buffer_Count, data_->buffers );
    glBindVertexArray( data_->object );
    glBindBuffer( GL_ARRAY_BUFFER, data_buffer_vbo( data_ ));
    glBufferData( GL_ARRAY_BUFFER, data_->verticesCount * sizeof( Vertex ), data_->vertices, GL_STATIC_DRAW );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, data_buffer_ebo( data_ ));
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, data_->indicesCount * sizeof( Data_Index ), data_->indices, GL_STATIC_DRAW );

    glEnableVertexAttribArray( Data::Location_Pos );
    glEnableVertexAttribArray( Data::Location_Color );
    glVertexAttribPointer( Data::Location_Pos, Vertex_Position_Size, Vertex_Position_Type_GL, GL_FALSE, sizeof( Vertex), (void *)offsetof( Vertex, xy ));
    glVertexAttribPointer( Data::Location_Color, Vertex_Color_Size, Vertex_Color_Type_GL, GL_TRUE, sizeof( Vertex), (void *)offsetof( Vertex, rgba ));

    glBindVertexArray( Zero );
    c::free( data_->vertices );
    c::free( data_->indices );
    APP_ASSERT( glGetError( ) == GL_NO_ERROR );

    return true;
}

void app::Model::bind( ) {
    glUseProgram( data_->idProgram );
    glBindVertexArray( data_->object );
    APP_ASSERT( glGetError( ) == GL_NO_ERROR );
}

void app::Model::draw( ) {
    glDrawElements(GL_TRIANGLES, data_->indicesCount, GL_UNSIGNED_BYTE, (void *)Zero);
    APP_ASSERT( glGetError( ) == GL_NO_ERROR );
}

void app::Model::free( ) {
    glUseProgram( Zero );
    glBindVertexArray( Zero );
    APP_ASSERT( glGetError( ) == GL_NO_ERROR );
}

void app::Model::clean( ) {
    glDeleteVertexArrays( 1, &data_->object );
    glDeleteBuffers( Data::Buffer_Count, data_->buffers );
    glDeleteProgram( data_->idProgram );
    APP_ASSERT( glGetError( ) == GL_NO_ERROR );

}
#else
// TODO: shaders vertex array object, named buffers and so on
bool app::Model::setup( ) { return true; }

void app::Model::bind( ) { }

void app::Model::draw( ) { }

void app::Model::free( ) { }

void app::Model::clean( ) { }
#endif // timelines
