#include "loco.h"

#include <stdio.h>

locoBase_c::locoBase_c( serverBase_c *parent, const locoAddress_t &address ) :
  _address( address ),
  _parent( parent )
{
  for ( int index = 0; index < LOCO_NUM_FUNCTION; index++ )
  {
    _functions[ index ] = false;
  }
}

bool locoBase_c::function( uint8_t index ) const
{
  return _functions[ index ];
}

uint8_t locoBase_c::codedSpeed( bool emergencyStop ) const
{
  uint8_t result = 0;

  if ( emergencyStop )
  {
    result = 0x01;
  }
  else if ( _speed > 0 )
  {
    if ( _speedMode == SPEED_MODE_28 )
    {
      result = ( ( _speed + 1 ) / 2 ) + 1;
      if ( ( _speed % 2 ) == 0 )
      {
        result |= 0x10;
      }
    }
    else
    {
      result = _speed + 1;
    }
  }

  if ( _direction == DIRECTION_FORWARD )
  {
    result |= 0x80;
  }

  return result;
}
