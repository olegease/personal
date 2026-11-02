#ifndef APP_VERTICES_HXX
#define APP_VERTICES_HXX

#include <cassert>
#include <glad/son8/header.h>

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

// forward declare namespaces
namespace son8 { namespace core { }}

namespace app {

    using namespace son8;
    using namespace son8::core;

    inline constexpr char const *Window_Title = "timeline version: " APP_STRINGIFY_EXPAND( APP_TIMELINE );
    inline constexpr GLuint Window_W = 640u;
    inline constexpr GLuint Window_H = 360u;
    inline constexpr GLdouble Window_Aspect = 1.0 * Window_W / ( 1.0 * Window_H );

    static constexpr signed Zero = 0;
    static constexpr unsigned Zero_U = 0u;

    struct Data;

    class Model final {
        using DataPtr_ = Data *;
        DataPtr_ data_;
      public:
        Model( );
       ~Model( );
        [[nodiscard]] bool setup( );
        void bind( );
        void draw( );
        void free( );
        void clean( );
    };
}

#endif // INCLUDE GUARD
