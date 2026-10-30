#ifndef APP_MODEL_HXX
#define APP_MODEL_HXX

#include <son8/core/alias/pour.hxx>

namespace app {
    using namespace son8::core;
    struct Data;

    class Model final {
        using DataPtr_ = Ptr< Data >;
        DataPtr_ data_;
    public:
        void setup( );
        void bind( );
        void draw( );
        void free( );
        void clean( );
    };
}

#endif//APP_MODEL_HXX
