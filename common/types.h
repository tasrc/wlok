#pragma once

#include <stdint.h>

// TODO
#define LITTLE_INDIAN_SYSTEM

typedef uint16_t clientId_t;
typedef uint16_t locoAddress_t;

/*!
  uword_t
*/
struct uword_t
{
  uword_t( uint16_t value )
  {
#ifdef LITTLE_INDIAN_SYSTEM
    low  = ( ( unsigned char * ) &value )[ 0 ];
    high = ( ( unsigned char * ) &value )[ 1 ];
#else
    high = ( ( unsigned char * ) &value )[ 0 ];
    low  = ( ( unsigned char * ) &value )[ 1 ];
#endif
  }

  operator uint16_t() const
  {
    uint16_t result;

#ifdef LITTLE_INDIAN_SYSTEM
    ( ( unsigned char * ) &result )[ 0 ] = low;
    ( ( unsigned char * ) &result )[ 1 ] = high;
#else
    ( ( unsigned char * ) &result )[ 0 ] = high;
    ( ( unsigned char * ) &result )[ 1 ] = low;
#endif

    return result;
  }

private:
  unsigned char low;
  unsigned char high;
};

/*!
  word_t
*/
struct word_t
{
  word_t( int16_t value )
  {
#ifdef LITTLE_INDIAN_SYSTEM
    low  = ( ( unsigned char * ) &value )[ 0 ];
    high = ( ( unsigned char * ) &value )[ 1 ];
#else
    high = ( ( unsigned char * ) &value )[ 0 ];
    low  = ( ( unsigned char * ) &value )[ 1 ];
#endif
  }

private:
  unsigned char low;
  unsigned char high;
};

/*!
  udwort_t
*/
struct udword_t
{
  udword_t( uint32_t value )
  {
#ifdef LITTLE_INDIAN_SYSTEM
    byte0  = ( ( unsigned char * ) &value )[ 0 ];
    byte1  = ( ( unsigned char * ) &value )[ 1 ];
    byte2  = ( ( unsigned char * ) &value )[ 2 ];
    byte3  = ( ( unsigned char * ) &value )[ 3 ];
#else
    byte3  = ( ( unsigned char * ) &value )[ 0 ];
    byte2  = ( ( unsigned char * ) &value )[ 1 ];
    byte1  = ( ( unsigned char * ) &value )[ 2 ];
    byte0  = ( ( unsigned char * ) &value )[ 3 ];
#endif
  }

  operator uint32_t() const
  {
    uint32_t result;

#ifdef LITTLE_INDIAN_SYSTEM
    ( ( unsigned char * ) &result )[ 0 ] = byte0;
    ( ( unsigned char * ) &result )[ 1 ] = byte1;
    ( ( unsigned char * ) &result )[ 2 ] = byte2;
    ( ( unsigned char * ) &result )[ 3 ] = byte3;
#else
    ( ( unsigned char * ) &result )[ 0 ] = byte3;
    ( ( unsigned char * ) &result )[ 1 ] = byte2;
    ( ( unsigned char * ) &result )[ 2 ] = byte1;
    ( ( unsigned char * ) &result )[ 3 ] = byte0;
#endif

    return result;
  }

private:
  unsigned char byte0;
  unsigned char byte1;
  unsigned char byte2;
  unsigned char byte3;
};
