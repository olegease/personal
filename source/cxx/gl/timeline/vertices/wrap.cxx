#define APP_TYPE_WRAP
//#define APP_TIMELINE APP_TIMELINE_BUFFER
#include "vertices.hxx"
// son8
#include <son8/core/alias/data.hxx>
#include <son8/core/alias/text.hxx>
#include <son8/cxx/flow.hxx>
#include <son8/cxx/func.hxx>
#include <son8/main.hxx>
// `son8` wrappers
#include <son8/windowed.hxx>
#define SON8_OVERGLAD_INCLUDE_DEPRECATED
#if 0u \
    || APP_TIMELINE == APP_TIMELINE_LIST \
    || APP_TIMELINE == APP_TIMELINE_IMMEDIATE \
    || APP_TIMELINE == APP_TIMELINE_ARRAY \
    || APP_TIMELINE == APP_TIMELINE_INTERLEAVE \
    || APP_TIMELINE == APP_TIMELINE_ELEMENT
#   include <son8/overglad/v1_1.hxx>
#elif APP_TIMELINE == APP_TIMELINE_SHADER
#   include <son8/overglad/v2_1.hxx>
#elif APP_TIMELINE == APP_TIMELINE_OBJECT
#   include <son8/overglad/v3_3.hxx>
#else
#   warning "include proper overglad wrapper for specific opengl version"
#   include <son8/overglad/comp.hxx>
#endif // overglad include
#include <print>
/**
 * Vertices Wrap Timeline Examples
 */
namespace app {

    using namespace cxx::string_view_literals;

    namespace gl = overglad;

    using Error = cxx::runtime_error;
    using Window = windowed::Window;
    using WindowConfig = windowed::Config;

    struct App {
        Model model;
        RawData data;
    };
}

void son8::main( Args args ) {
    using namespace app;
    std::string title = std::string{ "Wrap " } + Window_Title;

    App appOut;
    Ref< App > app = appOut;

    WindowConfig windowCfg{ SON8_OVERGLAD_VERSION };
    windowCfg.set( WindowConfig::Width{ Window_W });
    windowCfg.set( WindowConfig::Height{ Window_H });
    windowCfg.set( WindowConfig::Title{ title });

    Window window{ windowCfg };

    if ( window.is_error( window.bind_opengl( ))) { throw Error{ "cannot bind opengl" }; }

    if ( not appOut.model.setup( app.data )) { throw Error{ "setup model failed" }; }

    window.free_opengl( );
    window.run( [&appOut]{
            gl::clear_color( .125f, .125f, .125f );
            gl::clear( gl::field::Clear::Color );
            appOut.model.bind( );
            appOut.model.draw( );
            appOut.model.free( );
    });
    appOut.model.clean( );
}

namespace app {

    struct Data final {
#if APP_TIMELINE == APP_TIMELINE_IMMEDIATE
        Grow< Vertex > vertices;
#elif APP_TIMELINE == APP_TIMELINE_LIST
        using List = GLuint;
        List list{ };
#elif APP_TIMELINE == APP_TIMELINE_ARRAY
        Grow< Vertex::Position > positions;
        Grow< Vertex::Color > colors;
#elif APP_TIMELINE == APP_TIMELINE_INTERLEAVE
        Grow< Vertex > vertices;
#elif APP_TIMELINE == APP_TIMELINE_ELEMENT
        Grow< Vertex > vertices;
        Grow< Index > indices;
#elif APP_TIMELINE == APP_TIMELINE_BUFFER
        using Vertices = Grow< Vertex >;
        using Indices = Grow< Index >;
        gl::types::BufferArray vbo;
        gl::types::BufferElement ebo;
        // NOTE: cache counts as data would be passed to buffer object and cleared
        Size indicesCount = 0;
#elif APP_TIMELINE == APP_TIMELINE_SHADER
        static constexpr GLuint Attrib_Pos = 0;
        static constexpr GLuint Attrib_Color = 1;
        gl::types::Program program;
        gl::types::BufferArray bufferArray;
        gl::types::BufferElement bufferElement;
        // NOTE: cache counts as data would be passed to buffer object and cleared
        Size indicesCount = 0;
#elif APP_TIMELINE == APP_TIMELINE_OBJECT
        static constexpr GLuint Location_Pos = 0;
        static constexpr GLuint Location_Color = 1;
        gl::types::VertexArray vertexArray;
        gl::types::Program program;
        gl::types::BufferArray bufferArray;
        gl::types::BufferElement bufferElement;
        // NOTE: cache counts as data would be passed to buffer object and cleared
        Size indicesCount = 0;
#else
#warning "no data for timeline"
#endif
    };

} // app implementation

// model
app::Model::Model( ) : data_{ new Data } { }
app::Model::~Model( ) { delete data_; }
// timelines
#if APP_TIMELINE == APP_TIMELINE_IMMEDIATE
bool app::Model::setup( Ref< RawData > raw ) try {
    data_->vertices.reserve( raw.indices( ).size( ));

    for ( auto index : raw.indices( )) {
        data_->vertices.emplace_back( raw.xy( index ), raw.rgb( index ));
    }

    return true;
} catch ( ... ) { return false; }
void app::Model::bind( ) { }
void app::Model::draw( ) {
    gl::begin( gl::enums::Draw::Triangles );
    for ( auto vertex : data_->vertices ) {
        gl::color( vertex.rgba );
        gl::vertex( vertex.xy );
    }
    gl::end( );
}
void app::Model::free( ) { }
void app::Model::clean( ) { }
#elif APP_TIMELINE == APP_TIMELINE_LIST
bool app::Model::setup( Ref< RawData > raw ) try {
    data_->list = gl::gen_lists( 1 );
    gl::new_list( data_->list, GL_COMPILE );
    gl::begin( gl::enums::Draw::Triangles );
    for ( auto index : raw.indices( )) {
        gl::color( raw.rgb( index ));
        gl::vertex( raw.xy( index ));
    }
    gl::end( );
    gl::end_list( );
    APP_ASSERT( gl::get( ) == gl::enums::Error::No );
    return true;
} catch ( ... ) { return false; }

void app::Model::bind( ) { }

void app::Model::draw( ) {
    gl::call_list( data_->list );
    APP_ASSERT( gl::get( ) == gl::enums::Error::No );
}

void app::Model::free( ) { }

void app::Model::clean( ) { }
#elif APP_TIMELINE == APP_TIMELINE_ARRAY
bool app::Model::setup( Ref< RawData > raw ) try {
    Size size = raw.indices( ).size( );
    data_->colors.reserve( size );
    data_->positions.reserve( size );
    for ( auto index : raw.indices( )) {
        data_->colors.emplace_back( raw.rgb( index ));
        data_->positions.emplace_back( raw.xy( index ));
    }

    return true;
} catch ( ... ) { return false; }

void app::Model::bind( ) {
    gl::enable( gl::enums::ClientState::Vertex );
    gl::enable( gl::enums::ClientState::Color );
    gl::vertex_pointer( Vertex::Position_Size, Vertex::Position_Type_GL, Zero, data_->positions.data( ));
    gl::color_pointer( Vertex::Color_Size, Vertex::Color_Type_GL, Zero, data_->colors.data( ));
    APP_ASSERT( gl::get( ) == gl::enums::Error::No );
}

void app::Model::draw( ) {
    gl::draw_arrays( APP_CAST( GLenum, gl::enums::Draw::Triangles ), Zero, data_->positions.size( ));
    APP_ASSERT( gl::get( ) == gl::enums::Error::No );
}

void app::Model::free( ) {
    gl::disable( gl::enums::ClientState::Vertex );
    gl::disable( gl::enums::ClientState::Color );
    APP_ASSERT( gl::get( ) == gl::enums::Error::No );
}

void app::Model::clean( ) { }
#elif APP_TIMELINE == APP_TIMELINE_INTERLEAVE
bool app::Model::setup( Ref< RawData > raw ) try {
    data_->vertices.reserve( raw.indices( ).size( ));

    for ( auto index : raw.indices( )) {
        data_->vertices.emplace_back( raw.xy( index ), raw.rgb( index ));
    }

    return true;
} catch ( ... ) { return false; }

void app::Model::bind( ) {
    gl::enable( gl::enums::ClientState::Vertex );
    gl::enable( gl::enums::ClientState::Color );
    static constexpr auto stride = sizeof( Vertex );
    gl::vertex_pointer( Vertex::Position_Size, Vertex::Position_Type_GL, stride, &data_->vertices[Zero].xy );
    gl::color_pointer( Vertex::Color_Size, Vertex::Color_Type_GL, stride, &data_->vertices[Zero].rgba );
    APP_ASSERT( gl::get( ) == gl::enums::Error::No );
}

void app::Model::draw( ) {
    gl::draw_arrays( GL_TRIANGLES, Zero, data_->vertices.size( ));
    APP_ASSERT( gl::get( ) == gl::enums::Error::No );
}

void app::Model::free( ) {
    gl::disable( gl::enums::ClientState::Vertex );
    gl::disable( gl::enums::ClientState::Color );
    APP_ASSERT( gl::get( ) == gl::enums::Error::No );
}

void app::Model::clean( ) { }
#elif APP_TIMELINE == APP_TIMELINE_ELEMENT
bool app::Model::setup( Ref< RawData > raw ) try {
    data_->vertices.reserve( raw.vertices( ).size( ));
    for ( Ref< Vertex > vertex : raw.vertices( )) {
        data_->vertices.emplace_back( vertex );
    }
    data_->indices.resize( raw.indices( ).size( ));
    cxx::copy( raw.indices( ).begin( ), raw.indices( ).end( ), data_->indices.begin( ));

    return true;
} catch ( ... ) { return false; }

void app::Model::bind( ) {
    gl::enable( gl::enums::ClientState::Vertex );
    gl::enable( gl::enums::ClientState::Color );
    static constexpr auto stride = sizeof( Vertex );
    glVertexPointer( Vertex::Position_Size, Vertex::Position_Type_GL, stride, &data_->vertices[0].xy );
    glColorPointer( Vertex::Color_Size, Vertex::Color_Type_GL, stride, &data_->vertices[0].rgba );
    APP_ASSERT( gl::get( ) == gl::enums::Error::No );
}

void app::Model::draw( ) {
    gl::draw_elements( GL_TRIANGLES, data_->indices.size( ), GL_UNSIGNED_BYTE, data_->indices.data( ));
    APP_ASSERT( gl::get( ) == gl::enums::Error::No );
}

void app::Model::free( ) {
    gl::disable( gl::enums::ClientState::Vertex );
    gl::disable( gl::enums::ClientState::Color );
    APP_ASSERT( gl::get( ) == gl::enums::Error::No );
}
void app::Model::clean( ) { }
#elif APP_TIMELINE == APP_TIMELINE_BUFFER
bool app::Model::setup( Ref< RawData > raw ) try {
    Size verticesCount = raw.vertices( ).size( );
    data_->indicesCount = raw.indices( ).size( );
    Data::Vertices vertices( verticesCount );
    Data::Indices indices( data_->indicesCount );
    cxx::copy( raw.vertices( ).begin( ), raw.vertices( ).end( ), vertices.begin( ));
    cxx::copy( raw.indices( ).begin( ), raw.indices( ).end( ), indices.begin( ));
    gl::gen( data_->vbo );
    gl::gen( data_->ebo );
    gl::bind( data_->vbo );
    gl::buffer_data( GL_ARRAY_BUFFER, vertices.size( ) * sizeof( Vertex ), vertices.data( ), GL_STATIC_DRAW );
    gl::bind( data_->ebo );
    gl::buffer_data( GL_ELEMENT_ARRAY_BUFFER, indices.size( ) * sizeof( Index ), indices.data( ), GL_STATIC_DRAW );
    vertices.clear( );
    indices.clear( );
    APP_ASSERT( gl::get( ) == gl::enums::Error::No );

    return true;
} catch ( ... ) { return false; }

void app::Model::bind( ) {
    gl::enable( gl::enums::ClientState::Vertex );
    gl::enable( gl::enums::ClientState::Color );
    gl::bind( data_->vbo );
    gl::bind( data_->ebo );
    gl::vertex_pointer( Vertex::Position_Size, Vertex::Position_Type_GL, sizeof( Vertex), (void *)offsetof( Vertex, xy ));
    gl::color_pointer( Vertex::Color_Size, Vertex::Color_Type_GL, sizeof( Vertex), (void *)offsetof( Vertex, rgba ));
    APP_ASSERT( gl::get( ) == gl::enums::Error::No );
}

void app::Model::draw( ) {
    gl::draw_elements( GL_TRIANGLES, data_->indicesCount, GL_UNSIGNED_BYTE, (void *)Zero );
    APP_ASSERT( gl::get( ) == gl::enums::Error::No );
}

void app::Model::free( ) {
    gl::disable( gl::enums::ClientState::Vertex );
    gl::disable( gl::enums::ClientState::Color );
    gl::bind_buffer( GL_ARRAY_BUFFER, Zero );
    gl::bind_buffer( GL_ELEMENT_ARRAY_BUFFER, Zero );
    APP_ASSERT( gl::get( ) == gl::enums::Error::No );
}

void app::Model::clean( ) {
    gl::del( data_->vbo );
    gl::del( data_->ebo );
    APP_ASSERT( gl::get( ) == gl::enums::Error::No );
}
#elif APP_TIMELINE == APP_TIMELINE_SHADER
bool app::Model::setup( Ref< RawData > raw ) try {
    // shaders
    // -- code
    View codeShared = R"GLSL(#version 120
    varying vec4 vColor;
)GLSL"sv;

    View codeVertex = R"GLSL(
    attribute vec2 aPos;
    attribute vec4 aColor;

void main( ) {
    gl_Position = vec4( aPos, 0.0, 1.0 );
    vColor = aColor;
}
    )GLSL"sv;

    View codeFragment = R"GLSL(#version 120
    varying vec4 vColor;
void main( ) {
    gl_FragColor = vColor;
}
    )GLSL"sv;
    // -- loading
    gl::gen( data_->program );
    gl::types::Program program = data_->program;
    gl::types::Shader vertex, fragment;
    gl::gen( vertex, gl::enums::Shader::Vertex );
    gl::gen( fragment, gl::enums::Shader::Fragment );
    gl::attach( program, vertex );
    gl::attach( program, fragment );
    gl::types::Sources< 2 > sourcesVertex{ codeShared, codeVertex };
    gl::types::Source sourcesFragment{ codeFragment };

    gl::sources( vertex, sourcesVertex );
    gl::source( fragment, sourcesFragment );

    gl::compile( vertex );
    gl::compile( fragment );

    gl::bind_attrib_location( program, Data::Attrib_Pos, "aPos" );
    gl::bind_attrib_location( program, Data::Attrib_Color, "aColor" );
    gl::link( program );
    GLint statusProgram = GL_FALSE;
    gl::get_program( program, GL_LINK_STATUS, &statusProgram );
    auto info = [&]{
        APP_ASSERT( not statusProgram );
        gl::types::Info log;
        gl::info( log, program, "=== program ===\n" );
        gl::info( log, fragment, "=== fragment ===\n" );
        gl::info( log, vertex, "=== vertex ===\n" );
        std::println( stderr, "size/capacity: {}/{},\ninfo log: \n{}", log.size( ), log.capacity( ), log.view( ));
    };

    if ( not statusProgram ) { info( ); return false; }

    gl::detach( program, vertex );
    gl::detach( program, fragment );
    gl::del( vertex );
    gl::del( fragment );
    // Buffers
    Size verticesCount = raw.vertices( ).size( );
    // -- NOTE: cache for draw calls
    data_->indicesCount = raw.indices( ).size( );
    Grow< Vertex > vertices( verticesCount );
    Grow< Index > indices( data_->indicesCount );
    cxx::copy( raw.vertices( ).begin( ), raw.vertices( ).end( ), vertices.begin( ));
    cxx::copy( raw.indices( ).begin( ), raw.indices( ).end( ), indices.begin( ));
    gl::gen( data_->bufferArray );
    gl::gen( data_->bufferElement );
    gl::bind( data_->bufferArray );
    gl::buffer_data( GL_ARRAY_BUFFER, vertices.size( ) * sizeof( Vertex ), vertices.data( ), GL_STATIC_DRAW );
    gl::bind( data_->bufferElement );
    gl::buffer_data( GL_ELEMENT_ARRAY_BUFFER, indices.size( ) * sizeof( Index ), indices.data( ), GL_STATIC_DRAW );
    vertices.clear( );
    indices.clear( );
    APP_ASSERT( gl::get( ) == gl::enums::Error::No );

    return true;
} catch ( ... ) { return false; }

void app::Model::bind( ) {
    gl::bind( data_->program );
    gl::bind( data_->bufferArray );
    gl::bind( data_->bufferElement );
    gl::enable_vertex_attrib_array( Data::Attrib_Pos );
    gl::enable_vertex_attrib_array( Data::Attrib_Color );
    gl::vertex_attrib_pointer( Data::Attrib_Pos, Vertex::Position_Size, Vertex::Position_Type_GL, GL_FALSE, sizeof( Vertex), (void *)offsetof( Vertex, xy ));
    gl::vertex_attrib_pointer( Data::Attrib_Color, Vertex::Color_Size, Vertex::Color_Type_GL, GL_TRUE, sizeof( Vertex), (void *)offsetof( Vertex, rgba ));
    APP_ASSERT( gl::get( ) == gl::enums::Error::No );
}

void app::Model::draw( ) {
    gl::draw_elements( GL_TRIANGLES, data_->indicesCount, GL_UNSIGNED_BYTE, (void *)Zero );
    APP_ASSERT( gl::get( ) == gl::enums::Error::No );
}

void app::Model::free( ) {
    gl::disable_vertex_attrib_array( Data::Attrib_Color );
    gl::disable_vertex_attrib_array( Data::Attrib_Pos );
    gl::free( gl::types::BufferArray{ });
    gl::free( gl::types::BufferElement{ });
    gl::free( gl::types::Program{ });
    APP_ASSERT( gl::get( ) == gl::enums::Error::No );
}

void app::Model::clean( ) {
    gl::del( data_->program );
    gl::del( data_->bufferArray );
    gl::del( data_->bufferElement );
    APP_ASSERT( gl::get( ) == gl::enums::Error::No );
}
#elif APP_TIMELINE == APP_TIMELINE_OBJECT
bool app::Model::setup( Ref< RawData > raw ) try {
    // shaders
    // -- code
    View codeShared = R"GLSL(#version 330 core
)GLSL"sv;

    View codeVertex = R"GLSL(
    layout( location = 0 ) in vec2 aPos;
    layout( location = 1 ) in vec4 aColor;

    out vec4 vColor;

void main( ) {
    gl_Position = vec4( aPos, 0.0, 1.0 );
    vColor = aColor;
}
)GLSL"sv;

    View codeFragment = R"GLSL(
    in vec4 vColor;
    out vec4 FragColor;
void main( ) {
    FragColor = vColor;
}
)GLSL"sv;
    // -- loading
    gl::gen( data_->program );
    gl::types::Program program = data_->program;
    gl::types::Shader vertex, fragment;
    gl::gen( vertex, gl::enums::Shader::Vertex );
    gl::gen( fragment, gl::enums::Shader::Fragment );
    APP_ASSERT( program and vertex and fragment );
    gl::attach( program, vertex );
    gl::attach( program, fragment );
    gl::types::Sources< 2 > sourcesVertex{ codeShared, codeVertex };
    gl::types::Sources< 2 > sourcesFragment{ codeShared, codeFragment };

    gl::sources( vertex, sourcesVertex );
    gl::sources( fragment, sourcesFragment );

    gl::compile( vertex );
    gl::compile( fragment );

    gl::link( program );
    GLint statusProgram = GL_FALSE;
    gl::get_program( program, GL_LINK_STATUS, &statusProgram );
    auto info = [&]{
        APP_ASSERT( not statusProgram );
        gl::types::Info log;
        gl::info( log, program, "=== program ===\n" );
        gl::info( log, fragment, "=== fragment ===\n" );
        gl::info( log, vertex, "=== vertex ===\n" );
        std::println( stderr, "size/capacity: {}/{},\ninfo log: \n{}", log.size( ), log.capacity( ), log.view( ));
    };

    if ( not statusProgram ) { info( ); return false; }

    gl::detach( program, vertex );
    gl::detach( program, fragment );
    gl::del( vertex );
    gl::del( fragment );
    // Buffers
    // -- NOTE: cache for draw calls
    data_->indicesCount = raw.indices( ).size( );
    Size verticesCount = raw.vertices( ).size( );
    Grow< Vertex > vertices( verticesCount );
    Grow< Index > indices( data_->indicesCount );
    cxx::copy( raw.vertices( ).begin( ), raw.vertices( ).end( ), vertices.begin( ));
    cxx::copy( raw.indices( ).begin( ), raw.indices( ).end( ), indices.begin( ));
    gl::gen( data_->vertexArray );
    gl::gen( data_->bufferArray );
    gl::gen( data_->bufferElement );
    gl::bind( data_->vertexArray );
    gl::bind( data_->bufferArray );
    gl::buffer_data( GL_ARRAY_BUFFER, verticesCount * sizeof( Vertex ), vertices.data( ), GL_STATIC_DRAW );
    gl::bind( data_->bufferElement );
    gl::buffer_data( GL_ELEMENT_ARRAY_BUFFER, indices.size( ) * sizeof( Index ), indices.data( ), GL_STATIC_DRAW );
    gl::enable_vertex_attrib_array( Data::Location_Pos );
    gl::enable_vertex_attrib_array( Data::Location_Color );
    gl::vertex_attrib_pointer( Data::Location_Pos, Vertex::Position_Size, Vertex::Position_Type_GL, GL_FALSE, sizeof( Vertex ), (void *)offsetof( Vertex, xy ));
    gl::vertex_attrib_pointer( Data::Location_Color, Vertex::Color_Size, Vertex::Color_Type_GL, GL_TRUE, sizeof( Vertex ), (void *)offsetof( Vertex, rgba ));
    gl::free( gl::types::VertexArray{ });
    vertices.clear( );
    indices.clear( );
    APP_ASSERT( gl::get( ) == gl::enums::Error::No );

    return true;
} catch ( ... ) { return false; }

void app::Model::bind( ) {
    gl::bind( data_->program );
    gl::bind( data_->vertexArray );
    APP_ASSERT( gl::get( ) == gl::enums::Error::No );
}

void app::Model::draw( ) {
    gl::draw_elements( GL_TRIANGLES, data_->indicesCount, GL_UNSIGNED_BYTE, (void *)Zero );
    APP_ASSERT( gl::get( ) == gl::enums::Error::No );
}

void app::Model::free( ) {
    gl::free( gl::types::Program{ });
    gl::free( gl::types::VertexArray{ });
    APP_ASSERT( gl::get( ) == gl::enums::Error::No );
}

void app::Model::clean( ) {
    gl::del( data_->program );
    gl::del( data_->bufferElement );
    gl::del( data_->bufferArray );
    gl::del( data_->vertexArray );
    APP_ASSERT( gl::get( ) == gl::enums::Error::No );
}
#else
#warning "timeline model interface not implemented"
bool app::Model::setup( Ref< RawData > raw ) try { return true; } catch ( ... ) { return false; }

void app::Model::bind( ) { }

void app::Model::draw( ) { }

void app::Model::free( ) { }

void app::Model::clean( ) { }
#endif // app timelines
