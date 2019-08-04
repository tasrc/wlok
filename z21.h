#pragma once

// TODO
#define LITTLE_INDIAN_SYSTEM

#include <stdint.h>

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

/*!
  xGetFirmwareVersionMessage_t

  LAN_X_GET_FIRMWARE_VERSION
*/
struct xGetFirmwareVersionMessage_t
{
  uint8_t xorByte;
};

/*!
  xFirmwareVersionMessage_t
*/
struct xFirmwareVersionMessage_t
{
  uint8_t mainVersion;
  uint8_t subVersion;
  uint8_t xorByte;
};

/*!
  xStatusChangedMessage_t

  LAN_X_STATUS_CHANGED
*/
struct xStatusChangedMessage_t
{
  uint8_t status;
  uint8_t xorByte;
};

/*!
  xBcStoppedMessage_t

  LAN_X_BC_STOPPED
*/
struct xBcStoppedMessage_t
{
  uint8_t xorByte;
};

/*!
  xBcTrackPowerOnMessage_t

  LAN_X_BC_TRACK_POWER_ON
*/
struct xBcTrackPowerOnMessage_t
{
  uint8_t xorByte;
};

/*!
  xBcTrackShortCircuitMessage_t

  LAN_X_BC_TRACK_SHORT_CIRCUIT
*/
struct xBcTrackShortCircuitMessage_t
{
  uint8_t xorByte;
};

/*!
  xBcProgrammingModeMessage_t

  LAN_X_BC_PROGRAMMING_MODE
*/
struct xBcProgrammingModeMessage_t
{
  uint8_t xorByte;
};

/*!
  xVersionMessage_t

  LAN_X_GET_VERSION
*/
struct xVersionMessage_t
{
  uint8_t db1;
  uint8_t db2;
  uint8_t xorByte;
};

/*!
  xXorHelper_t
*/
struct xXorHelper_t
{
  uint8_t db1;
  uint8_t db2;
};

/*!
  xMessage_t
*/
struct xMessage_t
{
  uint8_t xHeader;
  uint8_t db0;

  union
  {
    xBcProgrammingModeMessage_t   bcProgrammingMode;
    xBcStoppedMessage_t           bcStopped;
    xBcTrackPowerOnMessage_t      bcTrackPowerOn;
    xBcTrackShortCircuitMessage_t bcTrackShortCircuit;
    xFirmwareVersionMessage_t     firmwareVersion;
    xGetFirmwareVersionMessage_t  getFirmwareVersion;
    xStatusChangedMessage_t       statusChanged;
    xVersionMessage_t             version;
    xXorHelper_t                  xorHelper;
  };

  uint8_t xor0() const
  {
    return xHeader;
  }

  uint8_t xor1() const
  {
    return xHeader ^ db0;
  }

  uint8_t xor2() const
  {
    return xHeader ^ db0 ^ xorHelper.db1;
  }

  uint8_t xor3() const
  {
    return xHeader ^ db0 ^ xorHelper.db1 ^ xorHelper.db2;
  }
};

/*!
  serialNumber_t
*/
struct serialNumberMessage_t
{
  udword_t serialNumber;
};

/*!
  systemStateDataChangedMessage_t

  LAN_SYSTEMSTATE_DATACHANGED
*/
struct systemStateDataChangedMessage_t
{
  word_t mainCurrent;
  word_t progCurrent;
  word_t filteredMainCurrent;
  word_t temperature;
  uword_t supplyVoltage;
  uword_t vccVoltage;
  uint8_t centralState;
  uint8_t centralStateEx;
  uint8_t reserved1;
  uint8_t reserved2;
};

/*!
  setBroadcastFlagsMessage_t

  LAN_SET_BROADCASTFLAGS
*/
struct setBroadcastFlagsMessage_t
{
  udword_t broadcastFlags;
};

/*!
  getBroadcastFlagsMessage_t

  LAN_GET_BROADCASTFLAGS
*/
struct getBroadcastFlagsMessage_t
{
  udword_t broadcastFlags;
};

/*!
  hwInfoMessage_t
*/
struct hwInfoMessage_t
{
  udword_t hwType;
  uint8_t  firmwareVersionSub;
  uint8_t  firmwareVersionMain;
  uint8_t  firmwareDummy1;
  uint8_t  firmwareDummy2;
};

/*!
  codeMessage_t
*/
struct codeMessage_t
{
  uint8_t  code;
};

/*!
  z21Message_t
*/
struct z21Message_t
{
  uword_t length;
  uword_t header;

  union
  {
    codeMessage_t                   code;
    getBroadcastFlagsMessage_t      getBroadcastFlags;
    hwInfoMessage_t                 hwInfo;
    serialNumberMessage_t           serialNumber;
    setBroadcastFlagsMessage_t      setBroadcastFlags;
    systemStateDataChangedMessage_t systemStateDataChanged;
    xMessage_t                      x;
  };
};

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

  inline uint8_t centralState() const { return _centralState; }
  virtual void   programmingMode() = 0;
  virtual void   stop( bool ) = 0;
  virtual void   trackPowerOff( bool ) = 0;
  virtual void   trackPowerOn( bool ) = 0;
  virtual void   trackShortCircuit() = 0;

protected:
  uint8_t _centralState = CS_NOT_SET;
};

/*!
  z21Base_c
*/
class z21Base_c
{
public:
  enum broadcastFlag_t
  {
    BROADCAST_AUTOMATIC_MESSAGES      = 0x00000001,
    BROADCAST_RMBUS_CHANGED           = 0x00000002,
    BROADCAST_RAILCOM_CHANGED         = 0x00000004,
    BROADCAST_SYSTEMSTATE_CHANGED     = 0x00000100,
    BROADCAST_ALL_LOCO_INFO           = 0x00010000,
    BROADCAST_RAILCOM_CHANGED_ALL     = 0x00040000,
    BROADCAST_FORWARD_CAN_TRACK       = 0x00080000,
    BROADCAST_FORWARD_LOCONET_GENERAL = 0x01000000,
    BROADCAST_FORWARD_LOCONET_LOCO    = 0x02000000,
    BROADCAST_FORWARD_LOCONET_SWITCH  = 0x04000000,
    BROADCAST_FORWARD_LOCONET_TRACK   = 0x08000000,
  };

  z21Base_c( serverBase_c * );

  inline udword_t broadcastFlags() const { return _broadcastFlags; }
  void            getSendBcProgrammingMode( char *, uint16_t & ) const;
  void            getSendBcStopped( char *, uint16_t & ) const;
  void            getSendBcTrackPowerOff( char *, uint16_t & ) const;
  void            getSendBcTrackPowerOn( char *, uint16_t & ) const;
  void            getSendBcTrackShortCircuit( char *, uint16_t & ) const;
  inline bool     isLoggedOff() const { return _isLoggedOff; }
  void            parseMsg( const char *, uint16_t &, char *, uint16_t & );

private:
  void createBcProgrammingMode( z21Message_t & ) const;
  void createBcStopped( z21Message_t & ) const;
  void createBcTrackPowerOff( z21Message_t & ) const;
  void createBcTrackPowerOn( z21Message_t & ) const;
  void createBcTrackShortCircuit( z21Message_t & ) const;
  void createSystemStateDataChanged( z21Message_t & ) const;
  void createUnknownCommand( z21Message_t & ) const;
  void logMsg( const char *, const z21Message_t & ) const;
  void processGetCode( z21Message_t & );
  void processGetHwInfo( z21Message_t & );
  void processGetSerialNumber( z21Message_t & );
  void processLogoff( z21Message_t & );
  void processGetBroadcastFlags( z21Message_t & );
  void processSetBroadcastFlags( const z21Message_t &, z21Message_t & );
  void processSystemStateGetData( z21Message_t & ) const;
  void processX( const z21Message_t &, z21Message_t & );

  udword_t      _broadcastFlags = 0;
  bool          _isLoggedOff    = false;
  serverBase_c *_parent         = nullptr;
};
