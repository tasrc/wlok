#include "z21.h"

#include <stdio.h>

z21Base_c::z21Base_c( serverBase_c *parent ) :
  _parent( parent )
{
}

void z21Base_c::parseMsg( const char *msgData, uint16_t &msgLen, char *replyData, uint16_t &replyLen )
{
  const z21Message_t &message = ( const z21Message_t & ) *msgData;
  z21Message_t &reply = ( z21Message_t & ) *replyData;

  msgLen = message.length;

  fprintf( stderr, "\nmessage %02x %02x (", uint16_t( message.length ), uint16_t( message.header ) );
  for ( int xx = 0; xx < msgLen; xx++ )
  {
    fprintf( stderr, " %02x", ( unsigned char ) msgData[ xx ] );
  }
  fprintf( stderr, " )\n" );

  switch ( uint16_t( message.header ) )
  {
  case 0x10:
    processGetSerialNumber( reply );
    break;

  case 0x1a:
    processGetHwInfo( reply );
    break;

  case 0x30:
    processLogoff( reply );
    break;

  case 0x40:
    processX( message, reply );
    break;

  case 0x50:
    processSetBroadcastFlags( message, reply );
    break;

  case 0x85:
    processSystemStateGetData( reply );
    break;

  default:
    replyUnknownCommand( reply );
  }

  replyLen = reply.length;

  fprintf( stderr, "reply %02x (", replyLen );
  for ( int xx = 0; xx < replyLen; xx++ )
  {
    fprintf( stderr, " %02x", ( unsigned char ) replyData[ xx ] );
  }
  fprintf( stderr, " )\n" );
}

void z21Base_c::processGetSerialNumber( z21Message_t &reply )
{
  fprintf( stderr, "-- LEN_GET_SERIAL_NUMBER\n" );

  reply.length                    = 0x08;
  reply.header                    = 0x10;
  reply.serialNumber.serialNumber = 12345; // TODO
}

void z21Base_c::processGetHwInfo( z21Message_t &reply )
{
  fprintf( stderr, "-- LEN_GET_HWINFO\n" );

  reply.length                       = 0x0c;
  reply.header                       = 0x10;
  reply.hwInfo.hwType                = serverBase_c::HWT_SMARTRAIL;
  reply.hwInfo.firmwareVersionSub    = _parent->firmwareVersionSub;
  reply.hwInfo.firmwareVersionMain   = _parent->firmwareVersionMain;
  reply.hwInfo.firmwareDummy1 = 0;
  reply.hwInfo.firmwareDummy2 = 0;
}

void z21Base_c::processLogoff( z21Message_t &reply )
{
  fprintf( stderr, "-- LAN_LOGOFF\n" );

  _isLoggedOff = true;

  reply.length = 0x0;
  reply.header = 0x0;
}

void z21Base_c::processX( const z21Message_t &msg, z21Message_t &reply )
{
  if ( msg.x.xHeader == 0x21 && msg.x.db0 == 0x24 )
  {
    fprintf( stderr, "-- LAN_X_GET_STATUS\n" );

    reply.length                    = 0x09;
    reply.header                    = 0x40;
    reply.x.xHeader                 = 0x62;
    reply.x.db0                     = 0x22;
    reply.x.statusChanged.status    = _parent->centralState();
    reply.x.firmwareVersion.xorByte = reply.x.xor1();
  }
  if ( msg.x.xHeader == 0x80 )
  {
    fprintf( stderr, "-- LAN_X_SET_STOP\n" );

    _parent->stop( true );
  }
  else if ( msg.x.xHeader == 0xf1 && msg.x.db0 == 0x0a )
  {
    fprintf( stderr, "-- LAN_X_GET_FIRMWARE_VERSION\n" );

    reply.length                        = 0x09;
    reply.header                        = 0x40;
    reply.x.xHeader                     = 0xf3;
    reply.x.db0                         = 0x0a;
    reply.x.firmwareVersion.mainVersion = _parent->firmwareVersionMain;
    reply.x.firmwareVersion.subVersion  = _parent->firmwareVersionSub;
    reply.x.firmwareVersion.xorByte     = reply.x.xor2();
  }
  else
  {
    fprintf( stderr, "-- ERROR: Unknown X-Message\n" );
    replyUnknownCommand( reply );
  }
}

void z21Base_c::processSetBroadcastFlags( const z21Message_t &msg, z21Message_t &reply )
{
  fprintf( stderr, "-- LAN_SET_BROADCASTFLAGS\n" );

  _broadcastFlags = msg.setBroadcastFlags.broadcastFlags;

  reply.length = 0x00;
  reply.header = 0x00;
}

void z21Base_c::processSystemStateGetData( z21Message_t &reply ) const
{
  fprintf( stderr, "-- LAN_SYSTEMSTATE_GETDATA\n" );

  reply.length                                     = 0x14;
  reply.header                                     = 0x84;
  reply.systemStateDataChanged.mainCurrent         = 0;
  reply.systemStateDataChanged.progCurrent         = 0;
  reply.systemStateDataChanged.filteredMainCurrent = 0;
  reply.systemStateDataChanged.temperature         = 0;
  reply.systemStateDataChanged.supplyVoltage       = 0;
  reply.systemStateDataChanged.vccVoltage          = 0;
  reply.systemStateDataChanged.centralState        = _parent->centralState();
  reply.systemStateDataChanged.centralStateEx      = 0;
  reply.systemStateDataChanged.reserved1           = 0;
  reply.systemStateDataChanged.reserved2           = 0;
}

void z21Base_c::replyUnknownCommand( z21Message_t &reply ) const
{
  fprintf( stderr, "-- ERROR: Unknown Command\n" );

  reply.length    = 0x07;
  reply.header    = 0x40;
  reply.x.xHeader = 0x61;
  reply.x.db0     = 0x82;
}
