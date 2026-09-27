/*
    tab (or `spacetab`): command-line tool
    \ to convert spaces into tabs but only
    \ for multiple spaces from line start.
*/
// includes
// -- std
#include <cassert>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace fs = std::filesystem;

namespace app {
    template< typename Type > using Out = Type &;
    template< typename Type > using Ref = Type const &;
    using Exception = std::exception;
    using Error = std::runtime_error;
    using InputFile = std::ifstream;
    using Text = std::string;
    using View = std::string_view;
    using GrowViews = std::vector< View >;
    using Path = fs::path;
    using Size = std::size_t;
    using fs::file_size;
    using std::distance;
    using std::cout, std::cerr, std::endl;

    static constexpr char New_Line = '\n';
    static constexpr char Space = ' ';

    struct TextState {
        struct Line {
            View original;
            View spacetab;
        };
        using GrowLines = std::vector< Line >;
        void read_text( Out< InputFile > file, Size fileSize );
        void split_lines( );
        void print_lines( ) const;
    private:
        Text text_;
        GrowLines lines_;
        void push_Line( View line );
    };

    struct State {
        TextState textState;
    };
} // app interface namespace

int main( int argc, char *argv[] ) try {
    using namespace app;
    cout << argv[argc - argc] << endl;

    if ( argc != 2 ) { throw Error{ "one input file expected" }; }

    State appOut;

    /* close file block */ {
        Path path{ argv[1] };
        auto fileSize = file_size( path );

        if ( not fileSize ) { return EXIT_SUCCESS; }

        InputFile file{ path };

        if ( not file.is_open( )) { throw Error{ "could not open input file" }; }
        appOut.textState.read_text( file, fileSize );
    }

    appOut.textState.split_lines( );
    Ref< State > app = appOut; // read only alias
    app.textState.print_lines( );

    return EXIT_SUCCESS;
} catch ( app::Ref< app::Exception > error ) {
    using namespace app;
    cerr << "std::exception: " << error.what( ) << endl;
    return EXIT_FAILURE;
} catch ( ... ) {
    using namespace app;
    cerr << "..." << endl;
    return EXIT_FAILURE;
}
// implementation
namespace app {
    // return partial view of input parameter, excluding spaces from the start (end end?)
    static View
    remove_Spaces( View line ) {
        assert( not line.empty( ) and line.back( ) == New_Line and "require line to end with new line character" );
        line.remove_prefix( line.find_first_not_of( Space ));
        return line;
    }

    void
    TextState::push_Line( View line ) {
        lines_.push_back({ line, remove_Spaces( line )});
    }

    void
    TextState::read_text( Out< InputFile > file, Size fileSize ) {
        lines_.clear( );
        text_.clear( );
        text_.reserve( fileSize + 1 );
        text_.resize( fileSize );
        file.read( text_.data( ), fileSize );

        if ( text_.back( ) != New_Line ) { text_.push_back( New_Line ); }
    }

    void
    TextState::split_lines( ) {
        assert( lines_.empty( ) and "split_lines called on already filled lines_" );
        lines_.reserve( text_.size( ) >> 5u );
        View full{ text_ };

        while ( not full.empty( ) ) {
            Size length = full.find( New_Line ) + 1u;
            push_Line( full.substr( 0, length ));
            full.remove_prefix( length );
        }
    }

    void
    TextState::print_lines( ) const {
        for ( auto line : lines_ ) {
            cout << "removed spaces: " << distance( line.original.data( ), line.spacetab.data( )) << ", line: " << line.spacetab;
        }
    }

} // app implementation namespace
