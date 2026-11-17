#include <app/debug.hxx>
// son8
#include <glad/son8.hxx>
#include <son8/c/exec.hxx>
#include <son8/core/alias/pour.hxx>
#include <son8/core/alias/text.hxx>
#include <son8/core/alias/data.hxx>
#include <print>
#include <stacktrace>
// NOTE: only private class functions implemented here
// \ as interface should just call private wrappers which
// \ would use heavy headers inclusion
// \ leaving debug interface lightweight
namespace app {
    using namespace son8;
    using namespace son8::core;
    using std::println;

    namespace {
        void GLAPIENTRY gl_Callback( GLenum, GLenum, GLuint id, GLenum severity, GLsizei, GLchar const *message, void const * ) {
            println( stderr, "GL DEBUG CALLBACK [{}]: {}", id, message );
            if ( severity == GL_DEBUG_SEVERITY_HIGH ) { assert( false and "high severity error" ); }
        }

        GLint gMaxDebugMessageLength{ };
    } // anonymous namespace

    // private

    bool Debug::gl_Legacy( Out< bool > isFatalError ) {
        switch ( glGetError( )) {
        case GL_NO_ERROR: return false;
            default: println( stderr, "GL DEBUG [legacy]: error happened" ); isFatalError = true; break;
        }
        return true;
    }

    bool Debug::gl_Modern( Out< bool > isFatalError, bool drain ) {
        GLint remaining = 0;
        glGetIntegerv( GL_DEBUG_LOGGED_MESSAGES, &remaining );

        if ( remaining <= 0 ) { return false; }
        else if ( drain ) {
            glGetDebugMessageLog( static_cast< GLuint >( remaining ), 0, NULL, NULL, NULL, NULL, NULL, NULL );

            return false;
        }

        static constexpr GLint Num = 8;
        Flat< GLuint, Num > ids;
        Flat< GLenum, Num > sevs;
        Flat< GLsizei, Num > lens;
        GLint maxMsgLen{ gMaxDebugMessageLength };
        assert( maxMsgLen >= 1 and "app: max debug message length is too short" );

        if ( maxMsgLen <= 0 ) { return false; }

        auto count = static_cast< GLuint >( std::min( remaining, Num ));
        Grow< GLchar > msg;
        GLsizei msgSize = maxMsgLen * static_cast< GLint >( count );
        msg.resize( static_cast< Size >( msgSize ));

        GLuint read = glGetDebugMessageLog( count, msgSize, NULL, NULL, ids.data( ), sevs.data( ), lens.data( ), msg.data( ));
        auto const *curr = msg.data( );

        for ( GLuint i = 0; i < read; ++i ) {

            if ( sevs[i] == GL_DEBUG_SEVERITY_HIGH ) { isFatalError = true; }

            println( stderr, "GL DEBUG LOG [{}] {}", ids[i], curr );
            curr += lens[i];
        }

        return read > 0u;
    }

    bool Debug::is_Init( ) const { return flags_ & cast_Flag( Flags_::Init_Bit ); }
    bool Debug::is_Modern( ) const { return flags_ & cast_Flag( Flags_::Modern_Bit ); }
    bool Debug::is_Callback( ) const { return flags_ & cast_Flag( Flags_::Callback_Bit ); }
    void Debug::set_Flag( Flags_ flag ) { flags_ |= cast_Flag( flag ); }
    void Debug::clear_Flag( Flags_ flag ) { flags_ &= ~cast_Flag( flag ); }
    void Debug::init_Required( ) const {
    #ifndef NDEBUG
        if ( is_Init( )) return;
        println( stderr, "app: initializatin required to perform operation\n{}", std::stacktrace::current( 1 ));
        c::abort( );
    #endif // debug build
    }


    void Debug::use_callback( ) {
        init_Required( );

        if ( not is_Modern( )) {
            println( stderr, "app: debug could not use callback in legacy system\n" );

            return;
        }

        glEnable( GL_DEBUG_OUTPUT_SYNCHRONOUS );
        glDebugMessageCallback( gl_Callback, nullptr );
        set_Flag( Flags_::Callback_Bit );

    }
    void Debug::nil_callback( ) {
        init_Required( );

        if ( not is_Modern( )) {
            println( stderr, "app: debug could not use callback in legacy system" );

            return;
        }

        if ( not is_Callback( )) {
            println( stderr, "app: debug logic error, should not nil already not used callback" );

            return;
        }

        glDisable( GL_DEBUG_OUTPUT_SYNCHRONOUS );
        glDebugMessageCallback( nullptr, nullptr );
        clear_Flag( Flags_::Callback_Bit );
    }

    void Debug::init( char const *cversion ) {
        if ( is_Init( )) {
            println( stderr, "app: debug logic error, already initialized" );
            return;
        }

        init_Impl( cversion );

    }

    void Debug::init_Impl( char const *cversion ) {
        set_Flag( Flags_::Init_Bit );

        View version{ cversion };
        char major = version[0];
        char minor = version[2];
        GLint flagsContext{ };

        if ( version.size( ) < 3u ) {
            println( stderr, "app: gl version is too short, using legacy debug" );
            return;
        }

        if ( major != '4' ) {
            println( stderr, "app: gl major version is too low, using legacy debug" );
            return;
        }


        if ( '6' < minor or minor < '3' ) {
            println( stderr, "app: gl minor version is not supported, using legacy debug" );
            return;
        }

        glGetIntegerv( GL_CONTEXT_FLAGS, &flagsContext );

        if ( flagsContext & GL_CONTEXT_FLAG_DEBUG_BIT ) {
            glGetIntegerv( GL_MAX_DEBUG_MESSAGE_LENGTH, &gMaxDebugMessageLength );
            glEnable( GL_DEBUG_OUTPUT );
            glDebugMessageControl( GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE );
            set_Flag( Flags_::Modern_Bit );
            assert( check( ) and "app: self check for debug initilization failed" );
        } else {
            println( stderr, "app: debug context flag is not set" );
        }

    }

} // namespace `app`
