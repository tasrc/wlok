#pragma once

#include "types.h"

#define LOCO_MAX_FUNCTION 28

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

  inline locoAddress_t   address() const { return _address; }
  inline addressFormat_t addressFormat() const { return _addressFormat; }
  uint8_t                codedSpeed() const;
  bool                   function( uint8_t ) const;
  inline bool            multipleUnit() const { return _multipleUnit; }
  inline void            setAddressFormat( addressFormat_t format ) { _addressFormat = format; }
  void                   setCodedSpeed( uint8_t );
  void                   setFunction( uint8_t, bool );
  inline void            setSpeedMode( speedMode_t mode ) { _speedMode = mode; }
  inline speedMode_t     speedMode() const { return _speedMode; }
  bool                   toggleFunction( uint8_t );

private:
  locoBase_c() = delete;

  locoAddress_t   _address = 0;
  addressFormat_t _addressFormat = ADDRESS_FORMAT_DCC;
  direction_t     _direction = DIRECTION_FORWARD;
  bool            _emergencyStop = false;
  bool            _functions[ LOCO_MAX_FUNCTION + 1 ];
  bool            _multipleUnit = false;
  serverBase_c   *_parent = nullptr;
  uint8_t         _speed = 0;
  speedMode_t     _speedMode = SPEED_MODE_128;
};
