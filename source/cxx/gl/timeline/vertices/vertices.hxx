#ifndef APP_VERTICES_HXX
#define APP_VERTICES_HXX

#include <son8/core/alias/pour.hxx>
#include <glad/son8/header.h>
#include <glad/son8/define/values.h>

#include <cassert>

// TODO: wrap the model into special class that would allow to
// \ choose `OpenGL` version for vertices processing for future timelines

#define APP_TIMELINE_IMMEDIATE  0x0100'1u // Vertex Calls
#define APP_TIMELINE_LIST       0x0100'2u // Display Lists
#define APP_TIMELINE_ARRAY      0x0101'1u // Draw Arrays
#define APP_TIMELINE_INTERLEAVE 0x0101'2u // Draw Arrays Interleaved
#define APP_TIMELINE_ELEMENT    0x0101'3u // Draw Elements
#define APP_TIMELINE_BUFFER     0x0105'0u // Vertex Buffer Objects
#define APP_TIMELINE_SHADER     0x0201'0u // Vertex and Fragment Shaders, `glsl 120`
#define APP_TIMELINE_OBJECT     0x0303'0u // Vertex Array Object `glsl 330`
// TODO
//`#define APP_TIMELINE_BINARY 0x0403'0u` // Binary Shaders and Program Pipelines `glsl 430`
//`#define APP_TIMELINE_NAME 0x0406'0u` // Named Objects Specification `glsl 460`

// TODO remove later
#define APP_TIMELINE APP_TIMELINE_BUFFER

#define APP_STRINGIFY( x ) #x
#define APP_STRINGIFY_EXPAND( x ) APP_STRINGIFY( x )

#define APP_ASSERT( check ) assert(( check ))
#define APP_CAST( to, from ) static_cast< to >(( from ))


namespace app {

    using namespace son8;
    using namespace son8::core;

    inline constexpr Ptr< Char const > Window_Title = "timeline version: " APP_STRINGIFY_EXPAND( APP_TIMELINE );
    inline constexpr GLuint Window_W = 1280u;
    inline constexpr GLuint Window_H = 720u;
    inline constexpr GLdouble Window_Aspect = 1.0 * Window_W / ( 1.0 * Window_H );

    static constexpr signed Zero = 0;
    static constexpr unsigned Zero_U = 0u;

    using Index = GLubyte;
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

    class RawData {
        static constexpr auto Vertices_Count_ = 14;
        static constexpr auto Indices_Count_ = 36;
        using Vertices_ = Flat< Vertex, Vertices_Count_ >;
        using Indices_ = Flat< Index, Indices_Count_ >;
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
        auto vertices_count( ) const { return Vertices_Count_; }
        auto indices_count( ) const { return Indices_Count_; }
        auto vertices( ) const -> Ref< Vertices_ > { return vertices_; }
        auto indices( ) const -> Ref< Indices_ > { return indices_; }
        auto xy( Index index ) const -> Ref< Vertex::Position > { return vertices_[index].xy; }
        auto xy_v( Index index ) const -> Ptr< Vertex::Position_Type const > { return xy( index ).data( ); }
        auto rgb( Index index ) const -> Ref< Vertex::Color > { return vertices_[index].rgba; }
        auto rgb_v( Index index ) const -> Ptr< Vertex::Color_Type const > { return rgb( index ).data( ); }
        auto indirect_vertex_v( Size i ) const -> Ptr< Vertex::Position_Type const > {
            auto index = indices_[i];
            return xy_v( index );
        }
        auto indirect_color_v( Size i ) const -> Ptr< Vertex::Color_Type const > {
            auto index = indices_[i];
            return rgb_v( index );
        }
    };

    // NOTE: specific implementation for timelines
    struct Data;

    class Model final {
        using DataPtr_ = Ptr< Data >;
        DataPtr_ data_;
      public:
        Model( );
       ~Model( );
        [[nodiscard]] Bool setup( Ref< RawData > raw );
        Void bind( );
        Void draw( );
        Void free( );
        Void clean( );
    };

}

#endif // INCLUDE GUARD
