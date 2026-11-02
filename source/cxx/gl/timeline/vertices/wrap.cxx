#define APP_TYPE_WRAP
//#define APP_TIMELINE APP_TIMELINE_BUFFER
#include "vertices.hxx"
// son8
#include <son8/core/alias/data.hxx>
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
    #include <son8/overglad/v1_1.hxx>
#elif APP_TIMELINE == APP_TIMELINE_BUFFER
    #include <son8/overglad/v1_5.hxx>
#else
    #warning "include proper overglad wrapper for specific opengl version"
    #include <son8/overglad/comp.hxx>
#endif // overglad include
/**
 * Vertices Wrap Timeline Examples
 */
namespace app {

    namespace gl = overglad;

    using Error = cxx::runtime_error;
    using Window = windowed::Window;
    using WindowConfig = windowed::Config;

    struct App {
        Model model;
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

    if ( not appOut.model.setup( )) { throw Error{ "setup model failed" }; }

    window.free_opengl( );
    window.run( [&appOut]{
            gl::clear_color( .125f, .125f, .125f );
            gl::clear( gl::enums::Clearbit::Color );
            appOut.model.bind( );
            appOut.model.draw( );
            appOut.model.free( );
    });
    appOut.model.clean( );
}

namespace app {

    struct Vertex {
        static constexpr GLenum Position_Type_GL = GL_FLOAT;
        static constexpr GLenum Color_Type_GL = GL_UNSIGNED_BYTE;
        static constexpr auto Position_Size = 2;
        static constexpr auto Color_Size = 4;
        static constexpr auto Pad_Size = 4;
        using Position_Type = GLfloat;
        using Color_Type = GLubyte;
        using Position = Flat< Position_Type, Position_Size >;
        using Color = Flat< Color_Type, Color_Size >;
        Position xy;
        Color rgba;
        GLubyte pad[Pad_Size];
    };
    static_assert( sizeof(Vertex) == 16
        , "Vertex must be exactly 16 bytes for cache line alignment" );
    static_assert( offsetof( Vertex, xy ) == 0
        , "xy offset must be 0" );
    static_assert( offsetof( Vertex, rgba ) == 8
        , "rgba offset must be 8" );
    static_assert( offsetof( Vertex, pad ) == 12
        , "pad offset must be 12");

    class Data final {
        static constexpr auto Vertices_Count_ = 14;
        static constexpr auto Indices_Count_ = 36;
        using Vertices_ = Flat< Vertex, Vertices_Count_ >;
        using Index_ = GLubyte;
        using Indices_ = Flat< Index_, Indices_Count_ >;
        Vertices_ vertices_ = {{
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
        }};
        Indices_ indices_ = {
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
    public:
        using Index = Index_;
        auto raw_vertices( ) const -> Ref< Vertices_ > { return vertices_; }
        auto raw_indices( ) const -> Ref< Indices_ > { return indices_; }
        auto xy( Index index ) const -> Ref< Vertex::Position > { return vertices_[index].xy; }
        auto xy_v( Index index ) const -> Ptr< Vertex::Position_Type const > { return xy( index ).data( ); }
        auto rgb( Index index ) const -> Ref< Vertex::Color > { return vertices_[index].rgba; }
        auto rgb_v( Index index ) const -> Ptr< Vertex::Color_Type const > { return rgb( index ).data( ); }
#if APP_TIMELINE == APP_TIMELINE_LIST
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
        using Buffer = GLuint;
        static constexpr auto Index_VBO = 0;
        static constexpr auto Index_EBO = 1;
        static constexpr auto Buffer_Count = 2;
        Flat< Buffer, Buffer_Count > buffers;
        gl::types::BufferArray vbo;
        gl::types::BufferElement ebo;
        Grow< Vertex > vertices;
        Grow< Index > indices;
        // NOTE: cache counts as data would be passed to buffer object and cleared
        Size indicesCount = 0;
        Size verticesCount = 0;
#endif
    };

} // app implementation

// model
app::Model::Model( ) : data_{ new Data } { }
app::Model::~Model( ) { delete data_; }
// timelines
#if APP_TIMELINE == APP_TIMELINE_IMMEDIATE
bool app::Model::setup( ) try { return true; } catch ( ... ) { return false; }
void app::Model::bind( ) { }
void app::Model::draw( ) {
    gl::begin( gl::enums::Draw::Triangles );
    for ( auto index : data_->raw_indices( )) {
        gl::color( data_->rgb( index ));
        gl::vertex( data_->xy( index ));
    }
    gl::end( );
}
void app::Model::free( ) { }
void app::Model::clean( ) { }
#elif APP_TIMELINE == APP_TIMELINE_LIST
bool app::Model::setup( ) try {
    data_->list = gl::gen_lists( 1 );
    gl::new_list( data_->list, GL_COMPILE );
    gl::begin( gl::enums::Draw::Triangles );
    for ( auto index : data_->raw_indices( )) {
        gl::color( data_->rgb( index ));
        gl::vertex( data_->xy( index ));
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
bool app::Model::setup( ) try {
    data_->vertices.reserve( data_->raw_indices( ).size( ));

    for ( auto index : data_->raw_indices( )) {
        data_->vertices.emplace_back( Vertex{ data_->xy( index ), data_->rgb( index )});
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
bool app::Model::setup( ) try {
    data_->verticesCount = data_->raw_vertices( ).size( );
    data_->indicesCount = data_->raw_indices( ).size( );
    data_->vertices.resize( data_->verticesCount );
    data_->indices.resize( data_->indicesCount );
    cxx::copy( data_->raw_vertices( ).begin( ), data_->raw_vertices( ).end( ), data_->vertices.begin( ));
    cxx::copy( data_->raw_indices( ).begin( ), data_->raw_indices( ).end( ), data_->indices.begin( ));
    gl::gen( data_->vbo );
    gl::gen( data_->ebo );
    gl::bind( data_->vbo );
    gl::buffer_data( GL_ARRAY_BUFFER, data_->vertices.size( ) * sizeof( Vertex ), data_->vertices.data( ), GL_STATIC_DRAW );
    gl::bind( data_->ebo );
    gl::buffer_data( GL_ELEMENT_ARRAY_BUFFER, data_->indices.size() * sizeof( Data::Index ), data_->indices.data( ), GL_STATIC_DRAW );
    data_->vertices.clear( );
    data_->indices.clear( );
    APP_ASSERT( gl::get( ) == gl::enums::Error::No );
    return true;

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
#else
bool app::Model::setup( ) try { return true; } catch ( ... ) { return false; }

void app::Model::bind( ) { }

void app::Model::draw( ) { }

void app::Model::free( ) { }

void app::Model::clean( ) { }
#endif // app timelines
