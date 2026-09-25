#include <benchmark/benchmark.h>
// std
#include <fstream>
#include <filesystem>
#include <stdexcept>
#include <string>
#include <string_view>

#define BENCH_STATE bm::State &state

template< typename Type > using Out = Type &;

namespace bm = benchmark;
namespace fs = std::filesystem;

using namespace std::string_view_literals;
using InputFile = std::ifstream;
using OutputFile = std::ofstream;
using Error = std::runtime_error;
using Text = std::string;
using View = std::string_view;

static constexpr auto File_Name = "test.out"sv;
static auto File_Size = fs::file_size( File_Name );

static void restore_file( InputFile &file ) {
    file.clear( );
    file.seekg( 0, std::ios::beg );
}

static auto open_file( View fileName ) -> InputFile {
    InputFile file{ Text{ fileName } };

    if ( not file.is_open( ) ) throw Error{ "open file" };

    return file ;
}

static void BM_stream_read( BENCH_STATE ) {
    InputFile file = open_file( File_Name );

    auto do_work = [&]( Out< Text > text ) {
        text.resize( File_Size );
        file.read( text.data( ), File_Size );
    };

    { // test
        Text text;
        do_work( text );
        OutputFile of{ "srd.out" };
        of << text;
    }

    for ( auto _ : state ) {
        restore_file( file );
        Text text;
        do_work( text );
        bm::DoNotOptimize( text.data( ) );
        bm::ClobberMemory( );
    }

    state.SetBytesProcessed( int64_t( state.iterations() ) * int64_t( File_Size ) );
}

static void BM_streambuf_iterator( BENCH_STATE ) {
    using It = std::istreambuf_iterator< typename InputFile::char_type >;
    InputFile file = open_file( File_Name );
    auto do_work = [&]( Out< Text > text ) {
        text.reserve( File_Size );
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
        bm::DoNotOptimize( text.data( ) );
        bm::ClobberMemory( );
    }

    state.SetBytesProcessed( int64_t( state.iterations() ) * int64_t( File_Size ) );
}

static void BM_resize_and_overwrite( BENCH_STATE ) {
    InputFile file = open_file( File_Name );
    auto do_work = [&]( Out< Text > text ) {
        text.resize_and_overwrite( File_Size, [&file]( char *buf, std::size_t count ) {
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
        bm::DoNotOptimize( text.data( ) );
        bm::ClobberMemory( );
    }

    state.SetBytesProcessed( int64_t( state.iterations() ) * int64_t( File_Size ) );
}

BENCHMARK( BM_streambuf_iterator );
BENCHMARK( BM_stream_read );
BENCHMARK( BM_resize_and_overwrite );

BENCHMARK_MAIN( );
