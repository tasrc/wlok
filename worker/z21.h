#pragma once

#include "types.h"

#define SERVER_PORT 21105

class serverBase_c;

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
  xGetLocoInfoMessage_t

  LAN_X_GET_LOCO_INFO
*/
struct xGetLocoInfoMessage_t
{
  uint8_t adrMsb;
  uint8_t adrLsb;
  uint8_t xorByte;
};

/*!
  xLocoInfoMessage_t

  LAN_X_LOCO_INFO
*/
struct xLocoInfoMessage_t
{
  uint8_t adrLsb;
  uint8_t speedMode;
  uint8_t speed;
  uint8_t function0;
  uint8_t function1;
  uint8_t function2;
  uint8_t function3;
  uint8_t xorByte;
};

/*!
  xSetLocoDriveMessage_t

  LAN_X_SET_LOCO_DRIVE
*/
struct xSetLocoDriveMessage_t
{
  uint8_t adrMsb;
  uint8_t adrLsb;
  uint8_t speed;
  uint8_t xorByte;
};

/*!
  xSetLocoFunctionMessage_t

  LAN_X_SET_LOCO_FUNCTION
*/
struct xSetLocoFunctionMessage_t
{
  uint8_t adrMsb;
  uint8_t adrLsb;
  uint8_t function;
  uint8_t xorByte;
};

/*!
  xXorHelper_t
*/
struct xXorHelper_t
{
  uint8_t db1;
  uint8_t db2;
  uint8_t db3;
  uint8_t db4;
  uint8_t db5;
  uint8_t db6;
  uint8_t db7;
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
    xGetLocoInfoMessage_t         getLocoInfo;
    xGetFirmwareVersionMessage_t  getFirmwareVersion;
    xLocoInfoMessage_t            locoInfo;
    xSetLocoDriveMessage_t        setLocoDrive;
    xSetLocoFunctionMessage_t     setLocoFunction;
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

  uint8_t xor4() const
  {
    return xHeader ^ db0 ^ xorHelper.db1 ^ xorHelper.db2 ^ xorHelper.db3;
  }

  uint8_t xor5() const
  {
    return xHeader ^ db0 ^ xorHelper.db1 ^ xorHelper.db2 ^ xorHelper.db3 ^ xorHelper.db4;
  }

  uint8_t xor6() const
  {
    return xHeader ^ db0 ^ xorHelper.db1 ^ xorHelper.db2 ^ xorHelper.db3 ^ xorHelper.db4 ^ xorHelper.db5;
  }

  uint8_t xor7() const
  {
    return xHeader ^ db0 ^ xorHelper.db1 ^ xorHelper.db2 ^ xorHelper.db3 ^ xorHelper.db4 ^ xorHelper.db5 ^
           xorHelper.db6;
  }

  uint8_t xor8() const
  {
    return xHeader ^ db0 ^ xorHelper.db1 ^ xorHelper.db2 ^ xorHelper.db3 ^ xorHelper.db4 ^ xorHelper.db5 ^
           xorHelper.db6 ^ xorHelper.db7;
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
  getLocoModeMessage_t

  LAN_GET_LOCOMODE
*/
struct getLocoModeMessage_t
{
  udword_t address;
};

/*!
  locoModeMessage_t
*/
struct locoModeMessage_t
{
  udword_t address;
  uint8_t  mode;
};

/*!
  setLocoModeMessage_t

  LAN_SET_LOCOMODE
*/
struct setLocoModeMessage_t
{
  udword_t address;
  uint8_t  mode;
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
    getLocoModeMessage_t            getLocoMode;
    hwInfoMessage_t                 hwInfo;
    locoModeMessage_t               locoMode;
    serialNumberMessage_t           serialNumber;
    setBroadcastFlagsMessage_t      setBroadcastFlags;
    setLocoModeMessage_t            setLocoMode;
    systemStateDataChangedMessage_t systemStateDataChanged;
    xMessage_t                      x;
  };
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

  inline udword_t   broadcastFlags() const { return _broadcastFlags; }
  void              getSendBcProgrammingMode( char *, uint16_t & ) const;
  void              getSendBcStopped( char *, uint16_t & ) const;
  void              getSendBcTrackPowerOff( char *, uint16_t & ) const;
  void              getSendBcTrackPowerOn( char *, uint16_t & ) const;
  void              getSendBcTrackShortCircuit( char *, uint16_t & ) const;
  void              getSendLocoInfo( const locoAddress_t &, char *, uint16_t & ) const;
  inline clientId_t id() const { return _id; }
  inline bool       isLoggedOff() const { return _isLoggedOff; }
  void              parseMsg( const char *, uint16_t &, char *, uint16_t & );

private:
  z21Base_c() = delete;

  void createBcProgrammingMode( z21Message_t & ) const;
  void createBcStopped( z21Message_t & ) const;
  void createBcTrackPowerOff( z21Message_t & ) const;
  void createBcTrackPowerOn( z21Message_t & ) const;
  void createBcTrackShortCircuit( z21Message_t & ) const;
  void createLocoInfo( const locoAddress_t &, z21Message_t & ) const;
  void createSystemStateDataChanged( z21Message_t & ) const;
  void createUnknownCommand( z21Message_t & ) const;
  void logMsg( const char *, const z21Message_t & ) const;
  void processGetCode( z21Message_t & );
  void processGetHwInfo( z21Message_t & );
  void processGetLocoMode( const z21Message_t &, z21Message_t & );
  void processGetSerialNumber( z21Message_t & );
  void processLogoff();
  void processGetBroadcastFlags( z21Message_t & );
  void processSetBroadcastFlags( const z21Message_t & );
  void processSetLocoDrive( const z21Message_t & );
  void processSetLocoFunction( const z21Message_t & );
  void processSetLocoMode( const z21Message_t & );
  void processSystemStateGetData( z21Message_t & ) const;
  void processX( const z21Message_t &, z21Message_t & );

  udword_t           _broadcastFlags = 0;
  clientId_t         _id             = 0;
  bool               _isLoggedOff    = false;
  static clientId_t  _nextId;
  serverBase_c      *_parent         = nullptr;
};
