#ifndef APP_VERTICES_HXX
#define APP_VERTICES_HXX

#include <cassert>

#include <son8/core/alias/pour.hxx>

// TODO: wrap the model into special class that would allow to
// \ choose `OpenGL` version for vertices processing for future timelines

#define APP_TIMELINE_IMMEDIATE  0x0100'1u // Vertex Calls
#define APP_TIMELINE_LIST       0x0100'2u // Display Lists
#define APP_TIMELINE_ARRAY      0x0101'1u // Draw Arrays
#define APP_TIMELINE_INTERLEAVE 0x0101'2u // Draw Arrays Interleaved
#define APP_TIMELINE_ELEMENT    0x0101'3u // Draw Elements
#define APP_TIMELINE_BUFFER     0x0105'0u // Vertex Buffer Objects
// TODO
//`#define APP_TIMELINE_SHADER 0x0201'0u`
//`#define APP_TIMELINE_OBJECT 0x0303'0u` // Vertex Array Object
//`#define APP_TIMELINE_NAME 0x0406'0u` // Named Objects Specification

#define APP_STRINGIFY( x ) #x
#define APP_STRINGIFY_EXPAND( x ) APP_STRINGIFY( x )

#define APP_ASSERT( check ) assert(( check ))


namespace app {
    using namespace son8;
    using namespace core;

    static constexpr signed Zero = 0;
    static constexpr unsigned Zero_U = 0u;

    struct Data;

    class Model final {
        using DataPtr_ = Ptr< Data >;
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
