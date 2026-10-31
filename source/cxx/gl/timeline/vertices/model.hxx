#ifndef APP_MODEL_HXX
#define APP_MODEL_HXX
#error "subject to be removed due to being renamed to vertices.hxx"
#include <cassert>
#define APP_ASSERT( check ) assert(( check ))

#include <son8/core/alias/pour.hxx>

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
        void setup( );
        void bind( );
        void draw( );
        void free( );
        void clean( );
    };
}

#endif // INCLUDE GUARD
