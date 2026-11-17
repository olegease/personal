#ifndef APP_DEBUG_HXX
#define APP_DEBUG_HXX

namespace app {

    class Debug {
        bool gl_Legacy( bool &isFatalError );
        bool gl_Modern( bool &isFatalError, bool drain = false );
        using FlagsType_ = unsigned;
        enum class Flags_ : FlagsType_ {
            Init_Bit        = 1u << 0u,
            Modern_Bit      = 1u << 1u,
            Callback_Bit    = 1u << 2u,
        };
        static constexpr auto cast_Flag( Flags_ flag ) -> FlagsType_ { return static_cast< FlagsType_ >( flag ); }
        FlagsType_ flags_{ };
        bool is_Init( ) const;
        bool is_Modern( ) const;
        bool is_Callback( ) const;
        void set_Flag( Flags_ flag );
        void clear_Flag( Flags_ flag );
        void init_Required( ) const;
        void init_Impl( char const *cversion );
    public:
        Debug( ) = default;
        Debug( char const *version ) { init_Impl( version ); }
        // initialization for default created debug objects
        void init( char const *version );
        void use_callback( );
        void nil_callback( );
        void drain( ) {
            init_Required( );

            if ( not is_Modern( ) or is_Callback( )) return;

            [[maybe_unused]] bool unused;

            while ( gl_Modern( unused, true )) { }
        }
        bool check( ) {
            init_Required( );
            if ( is_Callback( )) return true;
            bool isFatalError{ };
            if ( is_Modern( )) { while ( gl_Modern( isFatalError )) { } }
            else { while ( gl_Legacy( isFatalError )) { } }
            return not isFatalError;
        }
    };

} // namespace `app`

#endif // INCLUDE GUARD
