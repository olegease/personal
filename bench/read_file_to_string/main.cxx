#include <benchmark/benchmark.h>
// std
#include <cstdio>
#include <fstream>
#include <filesystem>
#include <iterator>
#include <stdexcept>
#include <string>
#include <string_view>

#define BENCH_STATE bm::State &state

template< typename Type > using Ptr = Type *;
template< typename Type > using Out = Type &;

namespace bm = benchmark;
namespace fs = std::filesystem;

using namespace std::string_view_literals;
using FileC = std::FILE;
using InputFile = std::ifstream;
using OutputFile = std::ofstream;
using Error = std::runtime_error;
using Text = std::string;
using View = std::string_view;
using Size = std::size_t;

static constexpr auto File_Name = "test.out"sv;
static constexpr auto Zero = 0;
static auto File_Size = fs::file_size( File_Name );

static void restore_file( InputFile &file ) {
    file.clear( );
    file.seekg( Zero, std::ios::beg );
}

static auto open_file( View fileName ) -> InputFile {
    InputFile file{ Text{ fileName } };

    if ( not file.is_open( ) ) throw Error{ "open file" };

    return file ;
}

static void BM_istream_iterator( BENCH_STATE ) {
    using It = std::istream_iterator< InputFile::char_type >;
    InputFile file = open_file( File_Name );

    auto do_work = [&]( Out< Text > text ) -> void {
        file >> std::noskipws; // Do not skip whitespace
        text.reserve( File_Size );
        text.assign( It{ file }, It{ } );
    };

    { // test
        Text text;
        do_work( text );
        OutputFile of{ "BM_istream_iterator.out" };
        of << text;
    }

    for ( auto _ : state ) {
        restore_file( file );
        Text text;
        do_work( text );
        bm::DoNotOptimize( text.data( ) );
        bm::ClobberMemory( );
    }

    state.SetBytesProcessed( state.iterations( ) * File_Size );
}

static void BM_streambuf_iterator( BENCH_STATE ) {
    using It = std::istreambuf_iterator< InputFile::char_type >;
    InputFile file = open_file( File_Name );
    auto do_work = [&]( Out< Text > text ) {
        text.reserve( File_Size );
        text.assign( It{ file }, It{ } );
    };

    { // test
        Text text;
        do_work( text );
        OutputFile of{ "BM_streambuf_iterator.out" };
        of << text;
    }

    for ( auto _ : state ) {
        restore_file( file );
        Text text;
        do_work( text );
        bm::DoNotOptimize( text.data( ) );
        bm::ClobberMemory( );
    }

    state.SetBytesProcessed( state.iterations( ) * File_Size );
}

static void BM_stringstream_rdbuf( BENCH_STATE ) {
    InputFile file = open_file( File_Name );

    auto do_work = [&]( Out< Text > text ) -> void {
        std::ostringstream ss;
        ss << file.rdbuf( );
        text = ss.str( );
    };

    { // test
        Text text;
        do_work( text );
        OutputFile of{ "BM_stringstream_rdbuf.out" };
        of << text;
    }

    for ( auto _ : state ) {
        restore_file( file );
        Text text;
        do_work( text );
        bm::DoNotOptimize( text.data( ));
        bm::ClobberMemory( );
    }

    state.SetBytesProcessed( state.iterations( ) * File_Size );
}

static void BM_cbuffer_fread( BENCH_STATE ) {
    unsigned const Buffer_Size = state.range( Zero );
    Ptr< FileC > file = std::fopen( File_Name.data( ), "r" );
    if ( file == nullptr ) throw Error{ "fopen" };

    auto buff = std::make_unique< unsigned char[] >( Buffer_Size );

    auto do_work = [&]( Out< Text > text ) {
        text.reserve( File_Size );
        Size readSize;
        while ( ( readSize = std::fread( buff.get( ), sizeof buff.get( )[Zero], Buffer_Size, file ))) {
            text.append( buff.get( ), buff.get( ) + readSize );
        }
    };

    { // test
        Text text;
        do_work( text );
        OutputFile of{ "BM_cbuffer_fread.out" };
        of << text;
    }

    for ( auto _ : state ) {
        std::clearerr( file );
        std::rewind( file );
        Text text;
        do_work( text );
        bm::DoNotOptimize( text.data( ) );
        bm::ClobberMemory( );
    }
    std::fclose( file );
    state.SetBytesProcessed( state.iterations( ) * File_Size );
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
        OutputFile of{ "BM_stream_read.out" };
        of << text;
    }

    for ( auto _ : state ) {
        restore_file( file );
        Text text;
        do_work( text );
        bm::DoNotOptimize( text.data( ) );
        bm::ClobberMemory( );
    }

    state.SetBytesProcessed( state.iterations( ) * File_Size );
}

static void BM_cfull_fread( BENCH_STATE ) {
    Ptr< FileC > file = std::fopen( File_Name.data( ), "rb" );
    if ( file == nullptr ) throw Error{ "fopen" };

    auto do_work = [&]( Out< Text > text ) {
        text.resize( File_Size );
        std::fread( text.data( ), sizeof text[0], File_Size, file );
    };

    { // test
        Text text;
        do_work( text );
        OutputFile of{ "BM_cfull_fread.out" };
        of << text;
    }

    for ( auto _ : state ) {
        std::clearerr( file );
        std::rewind( file );
        Text text;
        do_work( text );
        bm::DoNotOptimize( text.data( ) );
        bm::ClobberMemory( );
    }

    std::fclose( file );
    state.SetBytesProcessed( state.iterations( ) * File_Size );
}

static void BM_resize_and_overwrite( BENCH_STATE ) {
    InputFile file = open_file( File_Name );
    auto do_work = [&]( Out< Text > text ) {
        text.resize_and_overwrite( File_Size, [&file]( Ptr< char > buf, Size count ) {
            file.read( buf, count );

            return file.gcount( );
        });
    };

    { // test
        Text text;
        OutputFile of{ "BM_resize_and_overwrite.out" };
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

    state.SetBytesProcessed( state.iterations( ) * File_Size );
}

BENCHMARK( BM_istream_iterator );
BENCHMARK( BM_streambuf_iterator );
BENCHMARK( BM_stringstream_rdbuf );
BENCHMARK( BM_cbuffer_fread )->RangeMultiplier( 2u )->Range( 1 << 6, 1u << 13u );
BENCHMARK( BM_stream_read );
BENCHMARK( BM_cfull_fread );
BENCHMARK( BM_resize_and_overwrite );

BENCHMARK_MAIN( );
