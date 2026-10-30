#include <son8/main.hxx>
#include <son8/overglad.hxx>
#include <son8/windowed.hxx>
#include <son8/cxx/flow.hxx>

namespace app {
    using namespace son8;

    namespace gl = overglad;

    using Error = cxx::runtime_error;
    using Window = windowed::Window;
}

void son8::main( Args args ) {
    using namespace app;

    Window window{ SON8_OVERGLAD_VERSION };

    if ( window.is_error( window.bind_opengl( ))) { throw Error{ "cannot bind opengl" }; }
    // TODO `opengl` initialization
    window.free_opengl( );
    window.run( []{
            gl::clear_color( .125f, .25f, .5f );
            gl::clear( gl::enums::Clearbit::Color );
    });
}
