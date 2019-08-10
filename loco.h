#pragma once

#include "types.h"

#define LOCO_NUM_FUNCTION 28

class serverBase_c;

class locoBase_c
{
public:
  enum addressFormat_t
  {
    ADDRESS_FORMAT_DCC = 0,
    ADDRESS_FORMAT_MM  = 1,
  };

  enum speedMode_t
  {
    SPEED_MODE_14  = 0,
    SPEED_MODE_28  = 2,
    SPEED_MODE_128 = 4,
  };

  enum direction_t
  {
    DIRECTION_BACKWARD = 0,
    DIRECTION_FORWARD  = 1,
  };

  locoBase_c( serverBase_c *, const locoAddress_t & );

  inline addressFormat_t addressFormat() const { return _addressFormat; }
  uint8_t                codedSpeed( bool ) const;
  bool                   function( uint8_t ) const;
  inline bool            multipleUnit() const { return _multipleUnit; }
  inline void            setAddressFormat( addressFormat_t format ) { _addressFormat = format; }
  void                   setFunction( uint8_t, bool );
  inline speedMode_t     speedMode() const { return _speedMode; }
  bool                   toggleFunction( uint8_t );

private:
  locoBase_c() = delete;

  locoAddress_t   _address = 0;
  addressFormat_t _addressFormat = ADDRESS_FORMAT_DCC;
  direction_t     _direction = DIRECTION_FORWARD;
  bool            _functions[ LOCO_NUM_FUNCTION ];
  bool            _multipleUnit = false;
  serverBase_c   *_parent = nullptr;
  uint8_t         _speed = 0;
  speedMode_t     _speedMode = SPEED_MODE_128;
};
