#include <son8/main.hxx>
#include <son8/overglad.hxx>
#include <son8/windowed.hxx>

namespace gl = son8::overglad;

namespace app {
    using namespace son8;

    using Window = windowed::Window;
}

void son8::main( Args args ) {
    using namespace app;

    Window window{ SON8_OVERGLAD_VERSION };

    window.run( []{
            gl::clear_color( 1.f, 0.f, 0.f );
            gl::clear( gl::enums::Clearbit::Color );
    });
}
