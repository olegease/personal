#include <stdlib.h> /* malloc */
#include <stdio.h> /* printf */
#include <string.h> /* strlen */


/* sso - small string optimization */
#define A7_TEXT_SMALL_SIZE 16

typedef char A7Char;
typedef char *A7CPtr;
typedef unsigned long A7Size;

/* NOTE: size includes null character, cannot be passed as value type */
struct A7Text {
    union {
        struct {
            A7Char buff[A7_TEXT_SMALL_SIZE];
        } small;
        struct {
            A7CPtr heap;
            A7Size held;
        } large;
    } impl_;
    A7CPtr data_;
    A7Size size_;
};


typedef struct A7Text *A7TextPtr;

A7Size a7_text_error_size( void );
struct A7Text a7_text_init( void );
A7Size a7_text_create( A7TextPtr out, char const *cstr );
void a7_text_delete( A7TextPtr out );
A7Size a7_text_capacity( A7TextPtr const ref );
/* NOTE: never pass it as pointer or change its data */
static struct A7Text const A7_Text_Zeroed;

int main( void ) {

    struct A7Text textSmall, textLarge = a7_text_init( );

    char const *small = "small";
    char const *large = "large-large-large";
    A7Size textSmallSize = a7_text_create( &textSmall, small );
    A7Size textLargeSize = a7_text_create( &textLarge, large );

    printf( "%s\n", small );
    printf( "%s\n", large );

    printf( "%ld:%ld %s\n", textSmall.size_, textSmallSize, textSmall.data_ );
    printf( "%ld:%ld %s\n", textLarge.size_, textLargeSize, textLarge.data_ );

    a7_text_delete( &textLarge );

    return 0;
}


struct A7Text a7_text_init( void ) {
    return A7_Text_Zeroed;
}

A7Size a7_text_error_size( void ) {
    return -1u;
}

A7Size a7_text_create( A7TextPtr out, char const *cstr ) {
    A7Size capacity;
    A7TextPtr const ref = out; /* alias for reading only */

    if ( cstr == NULL ) return a7_text_error_size( );

    out->size_ = strlen( cstr ) + 1;

    if ( ref->size_ <= A7_TEXT_SMALL_SIZE ) {
        out->data_ = out->impl_.small.buff;
        capacity = A7_TEXT_SMALL_SIZE;
    } else { /* large string */
        capacity = ( ref->size_ >> 4u ) << 5u;
        out->impl_.large.held = capacity;
        out->impl_.large.heap = ( char * )malloc( capacity );

        if ( ref->impl_.large.heap == NULL ) return a7_text_error_size( );

        out->data_ = ref->impl_.large.heap;

    }

    memcpy( out->data_, cstr, ref->size_ );

    return capacity;
}

void a7_text_delete( A7TextPtr out ) {
    if ( A7_TEXT_SMALL_SIZE < out->size_ ) free( out->data_ );
    *out = a7_text_init( );
}

A7Size a7_text_capacity( A7TextPtr const ref ) {
    return ( A7_TEXT_SMALL_SIZE < ref->size_ ) ? ref->impl_.large.held : A7_TEXT_SMALL_SIZE;
}
