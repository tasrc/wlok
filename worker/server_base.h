#pragma once

#include "z21.h"

class locoBase_c;

/*!
  serverBase_c
*/
class serverBase_c
{
public:
  enum centralState_t
  {
    CS_NOT_SET                 = 0x00,
    CS_EMERGENCY_STOP          = 0x01,
    CS_TRACK_VOLTAGE_OFF       = 0x02,
    CS_SHORT_CIRCUIT           = 0x04,
    CS_PROGRAMMING_MODE_ACTIVE = 0x20,
  };

  enum hwInfo_t
  {
    HWT_Z21_OLD   = 0x00000200,
    HWT_Z21_NEW   = 0x00000201,
    HWT_SMARTRAIL = 0x00000202,
    HWT_Z21_SMALL = 0x00000203,
    HWT_Z21_START = 0x00000204,
  };

  static const uint8_t firmwareVersionMain = 0x01;
  static const uint8_t firmwareVersionSub  = 0x30;

  inline uint8_t       centralState() const { return _centralState; }
  inline bool          centralStateStopped() const { return _centralState != CS_NOT_SET; }
  virtual locoBase_c & loco( const locoAddress_t & ) = 0;
  virtual clientId_t   locoController( const locoAddress_t & ) = 0;
  virtual void         sendLocoInfo( const locoAddress_t & ) = 0;
  virtual void         sendProgrammingMode() = 0;
  virtual void         sendStop( bool ) = 0;
  virtual void         sendTrackPowerOff( bool ) = 0;
  virtual void         sendTrackPowerOn( bool ) = 0;
  virtual void         sendTrackShortCircuit() = 0;
  virtual void         setLocoController( const locoAddress_t &, const clientId_t & ) = 0;
  virtual void         subscribeLoco( const locoAddress_t &, const clientId_t & ) = 0;

protected:
  uint8_t _centralState = CS_NOT_SET;
};
