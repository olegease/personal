#define APP_TYPE_WRAP
#include "model.hxx"
// son8
#include <son8/main.hxx>
#include <son8/overglad.hxx>
#include <son8/windowed.hxx>
#include <son8/cxx/flow.hxx>

/**
 * Vertices Wrap Timeline Examples
 */


namespace app {

    namespace gl = overglad;

    using Error = cxx::runtime_error;
    using Window = windowed::Window;

    struct Data {

    };

    struct App {
        Model model;
    };
}

void son8::main( Args args ) {
    using namespace app;

    App appOut;
    Ref< App > app = appOut;

    Window window{ SON8_OVERGLAD_VERSION };

    if ( window.is_error( window.bind_opengl( ))) { throw Error{ "cannot bind opengl" }; }
    appOut.model.setup( );
    window.free_opengl( );
    window.run( [&appOut]{
            gl::clear_color( .125f, .25f, .5f );
            gl::clear( gl::enums::Clearbit::Color );
            appOut.model.bind( );
            appOut.model.draw( );
            appOut.model.free( );
    });
    appOut.model.clean( );
}
// model
app::Model::Model( ) : data_{ new Data } { }
app::Model::~Model( ) { delete data_; }
void app::Model::setup( ) { }
void app::Model::bind( ) { }
void app::Model::draw( ) { }
void app::Model::free( ) { }
void app::Model::clean( ) { }
