#include "loco.h"

#include <stdio.h>

locoBase_c::locoBase_c( serverBase_c *parent, const locoAddress_t &address ) :
  _address( address ),
  _parent( parent )
{
  for ( int index = 0; index <= LOCO_MAX_FUNCTION; index++ )
  {
    _functions[ index ] = false;
  }
}

bool locoBase_c::function( uint8_t index ) const
{
  return _functions[ index ];
}

uint8_t locoBase_c::codedSpeed() const
{
  uint8_t result = 0;

  if ( _emergencyStop )
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

void locoBase_c::setCodedSpeed( uint8_t value )
{
  uint8_t speedValue = value & 0x7f;

  _speed = 0;
  _direction = value & 0x80 ? DIRECTION_FORWARD : DIRECTION_BACKWARD;
  _emergencyStop = ( speedValue == 0x01 ) || ( _speedMode == SPEED_MODE_28 && speedValue == 0x11 );

  if ( !( _emergencyStop || speedValue == 0 || ( _speedMode == SPEED_MODE_28 && speedValue == 0x10 ) ) )
  {
    if ( _speedMode == SPEED_MODE_28 )
    {
      _speed = ( ( speedValue & 0xf ) - 1 ) * 2;

      if ( speedValue & 0x10 == 0 )
      {
        _speed -= 1;
      }
    }
    else
    {
      _speed = speedValue - 1;
    }
  }
}

void locoBase_c::setFunction( uint8_t index, bool flag )
{
  if ( index <= LOCO_MAX_FUNCTION )
  {
    _functions[ index ] = flag;
  }
}

bool locoBase_c::toggleFunction( uint8_t index )
{
  if ( index <= LOCO_MAX_FUNCTION )
  {
    _functions[ index ] = !_functions[ index ];
    return _functions[ index ];
  }

  return false;
}
