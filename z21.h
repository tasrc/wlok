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
    xFirmwareVersionMessage_t    firmwareVersion;
    xGetFirmwareVersionMessage_t getFirmwareVersion;
    xStatusChangedMessage_t      statusChanged;
    xXorHelper_t                 xorHelper;
  };

  uint8_t xor0() const
  {
    return xHeader ^ db0;
  }

  uint8_t xor1() const
  {
    return xHeader ^ db0 ^ xorHelper.db1;
  }

  uint8_t xor2() const
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
  z21Message_t
*/
struct z21Message_t
{
  uword_t length;
  uword_t header;

  union
  {
    hwInfoMessage_t                 hwInfo;
    serialNumberMessage_t           serialNumber;
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
  enum centralState_t
  {
    CS_NOT_SET                 = 0x00,
    CS_EMERGENCY_STOP          = 0x01,
    CS_TRACK_VOLTAGE_OF        = 0x02,
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

  void parseMsg( const char *, uint16_t &, char *, uint16_t & );

private:
  void processGetHwInfo( z21Message_t & );
  void processGetSerialNumber( z21Message_t & );
  void processLogoff( z21Message_t & );
  void processSystemStateGetData( z21Message_t & ) const;
  void processX( const z21Message_t &, z21Message_t & );
  void processSetBroadcastFlags( const z21Message_t &, z21Message_t & );
  void replyUnknownCommand( z21Message_t & ) const;

  const uint8_t _firmwareVersionMain = 0x01;
  const uint8_t _firmwareVersionSub  = 0x28;
  uint8_t       _centralState        = CS_NOT_SET;
};
