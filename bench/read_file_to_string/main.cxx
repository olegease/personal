#include <benchmark/benchmark.h>
// std
#include <fstream>
#include <filesystem>
#include <stdexcept>
#include <string>
#include <string_view>


#define BENCH_STATE benchmark::State &state

template< typename Type > using Out = Type &;

namespace fs = std::filesystem;

using InputFile = std::ifstream;
using OutputFile = std::ofstream;
using Error = std::runtime_error;
using Text = std::string;
using View = std::string_view;
using namespace std::string_view_literals;

static constexpr auto File_Name = "main.cxx"sv;

static void restore_file( InputFile &file ) {
    file.clear( );
    file.seekg( 0, std::ios::beg );
}

static auto open_file( View fileName ) -> InputFile {
    InputFile file{ Text{ fileName } };
    if ( not file.is_open( ) ) throw Error{ "open file" };
    return file ;
}

static void BM_streambuf_iterator( BENCH_STATE ) {
    using It = std::istreambuf_iterator< typename InputFile::char_type >;
    InputFile file = open_file( File_Name );
    auto fileSize = fs::file_size( File_Name );
    auto do_work = [&]( Out< Text > text ) {
        text.reserve( fileSize );
        text.assign( It{ file }, It{ } );
    };
    { // test
        Text text;
        do_work( text );
        OutputFile of{ "sit.out" };
        of << text;
    }

    for ( auto _ : state ) {
        restore_file( file );
        Text text;
        do_work( text );
        benchmark::DoNotOptimize( text.data( ) );
        benchmark::ClobberMemory( );
    }
    state.SetBytesProcessed( int64_t( state.iterations() ) * int64_t( fileSize ) );
}

static void BM_resize_and_overwrite( BENCH_STATE ) {
    InputFile file = open_file( File_Name );
    auto fileSize = fs::file_size( File_Name );
    auto do_work = [&]( Out< Text > text ) {
        text.resize_and_overwrite( fileSize, [&file]( char *buf, std::size_t count ) {
            file.read( buf, count );
            return file.gcount( );
        });

    };
    { // test
        Text text;
        OutputFile of{ "rao.out" };
        do_work( text );
        of << text;
    }
    for ( auto _ : state ) {
        restore_file( file );
        Text text;
        do_work( text );
        benchmark::DoNotOptimize( text.data( ) );
        benchmark::ClobberMemory( );
    }
    state.SetBytesProcessed( int64_t( state.iterations() ) * int64_t( fileSize ) );
}

BENCHMARK( BM_streambuf_iterator );
BENCHMARK( BM_resize_and_overwrite );

BENCHMARK_MAIN( );
