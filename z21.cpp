#include "z21.h"

#include <stdio.h>

/*!
  Konstruktor
*/
z21Base_c::z21Base_c( serverBase_c *parent ) :
  _parent( parent )
{
}

/*!
  Empfangene Message parsen
*/
void z21Base_c::parseMsg( const char *msgData, uint16_t &msgLen, char *replyData, uint16_t &replyLen )
{
  const z21Message_t &message = ( const z21Message_t & ) *msgData;
  z21Message_t &reply = ( z21Message_t & ) *replyData;
  reply.length = 0x0;
  reply.header = 0x0;

  msgLen = message.length;

  logMsg( "\nmessage", message );

  switch ( uint16_t( message.header ) )
  {
  case 0x10:
    processGetSerialNumber( reply );
    break;

  case 0x12:
  case 0x16:
    // undokumentierte Kommandos
    fprintf( stderr, "-- ERROR: Undocumented message\n" );
    break;

  case 0x18:
    processGetCode( reply );
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

  case 0x51:
    processGetBroadcastFlags( reply );
    break;

  case 0x85:
    processSystemStateGetData( reply );
    break;

  default:
    createUnknownCommand( reply );
  }

  replyLen = reply.length;

  logMsg( "reply", reply );
}

/*!
  Abfrage der Seriennummer bearbeiten
*/
void z21Base_c::processGetSerialNumber( z21Message_t &reply )
{
  fprintf( stderr, "-- LAN_GET_SERIAL_NUMBER\n" );

  reply.length                    = 0x08;
  reply.header                    = 0x10;
  reply.serialNumber.serialNumber = 12345; // TODO
}

/*!
  Abfrage der Hardwareinformationen bearbeiten
*/
void z21Base_c::processGetHwInfo( z21Message_t &reply )
{
  fprintf( stderr, "-- LAN_GET_HWINFO\n" );

  reply.length                       = 0x0c;
  reply.header                       = 0x1a;
  reply.hwInfo.hwType                = serverBase_c::HWT_SMARTRAIL;
  reply.hwInfo.firmwareVersionSub    = _parent->firmwareVersionSub;
  reply.hwInfo.firmwareVersionMain   = _parent->firmwareVersionMain;
  reply.hwInfo.firmwareDummy1 = 0;
  reply.hwInfo.firmwareDummy2 = 0;
}

/*!
  Abfrage des Softwareumfang bearbeiten
*/
void z21Base_c::processGetCode( z21Message_t &reply )
{
  fprintf( stderr, "-- LAN_GET_CODE\n" );

  reply.length    = 0x0c;
  reply.header    = 0x18;
  reply.code.code = 0x00;
}

/*!
  Logoff bearbeiten
*/
void z21Base_c::processLogoff( z21Message_t &reply )
{
  fprintf( stderr, "-- LAN_LOGOFF\n" );

  _isLoggedOff = true;

  reply.length = 0x0;
  reply.header = 0x0;
}

/*!
  X-Bus Messages bearbeiten
*/
void z21Base_c::processX( const z21Message_t &msg, z21Message_t &reply )
{
  reply.length = 0x00;
  reply.header = 0x00;

  if ( msg.x.xHeader == 0x21 && msg.x.db0 == 0x21 )
  {
    fprintf( stderr, "-- LAN_X_GET_VERSION\n" );

    reply.length            = 0x09;
    reply.header            = 0x40;
    reply.x.xHeader         = 0x63;
    reply.x.db0             = 0x21;
    reply.x.version.db1     = 0x30;
    reply.x.version.db2     = 0x12;
    reply.x.version.xorByte = reply.x.xor3();
  }
  if ( msg.x.xHeader == 0x21 && msg.x.db0 == 0x24 )
  {
    fprintf( stderr, "-- LAN_X_GET_STATUS\n" );

    reply.length                  = 0x09;
    reply.header                  = 0x40;
    reply.x.xHeader               = 0x62;
    reply.x.db0                   = 0x22;
    reply.x.statusChanged.status  = _parent->centralState();
    reply.x.statusChanged.xorByte = reply.x.xor2();
  }
  else if ( msg.x.xHeader == 0x21 && msg.x.db0 == 0x80 )
  {
    fprintf( stderr, "-- LAN_X_SET_TRACK_POWER_OFF\n" );

    _parent->trackPowerOff( true );
  }
  else if ( msg.x.xHeader == 0x21 && msg.x.db0 == 0x81 )
  {
    fprintf( stderr, "-- LAN_X_SET_TRACK_POWER_ON\n" );

    _parent->trackPowerOn( true );
  }
  else if ( msg.x.xHeader == 0x80 )
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
    reply.x.firmwareVersion.xorByte     = reply.x.xor3();
  }
  else
  {
    fprintf( stderr, "-- ERROR: Unknown X-Message\n" );
    createUnknownCommand( reply );
  }
}

/*!
  Broadcast Flags setzen
*/
void z21Base_c::processSetBroadcastFlags( const z21Message_t &msg, z21Message_t &reply )
{
  fprintf( stderr, "-- LAN_SET_BROADCASTFLAGS\n" );

  _broadcastFlags = msg.setBroadcastFlags.broadcastFlags;

  reply.length = 0x00;
  reply.header = 0x00;
}

/*!
  Broadcast Flags abfragen
*/
void z21Base_c::processGetBroadcastFlags( z21Message_t &reply )
{
  fprintf( stderr, "-- LAN_GET_BROADCASTFLAGS\n" );

  reply.length = 0x08;
  reply.header = 0x051;
  reply.getBroadcastFlags.broadcastFlags = _broadcastFlags;
}

/*!
  Abfrage des Systemstatus bearbeiten
*/
void z21Base_c::processSystemStateGetData( z21Message_t &reply ) const
{
  fprintf( stderr, "-- LAN_SYSTEMSTATE_GETDATA\n" );

  createSystemStateDataChanged( reply );
}

/*!
  Message über Änderung des Systemstatus erzeugen
*/
void z21Base_c::createSystemStateDataChanged( z21Message_t &msg ) const
{
  fprintf( stderr, "-- LAN_SYSTEMSTATE_DATACHANGED\n" );

  msg.length                                     = 0x14;
  msg.header                                     = 0x84;
  msg.systemStateDataChanged.mainCurrent         = 0;
  msg.systemStateDataChanged.progCurrent         = 0;
  msg.systemStateDataChanged.filteredMainCurrent = 0;
  msg.systemStateDataChanged.temperature         = 0;
  msg.systemStateDataChanged.supplyVoltage       = 0;
  msg.systemStateDataChanged.vccVoltage          = 0;
  msg.systemStateDataChanged.centralState        = _parent->centralState();
  msg.systemStateDataChanged.centralStateEx      = 0;
  msg.systemStateDataChanged.reserved1           = 0;
  msg.systemStateDataChanged.reserved2           = 0;
}

/*!
  Antowrt auf eine unbekannte Anfrage erzeugen
*/
void z21Base_c::createUnknownCommand( z21Message_t &msg ) const
{
  fprintf( stderr, "-- ERROR: Unknown Command\n" );

  msg.length    = 0x07;
  msg.header    = 0x40;
  msg.x.xHeader = 0x61;
  msg.x.db0     = 0x82;
}

/*!
  Message über Stop alles Loks erzeugen
*/
void z21Base_c::createBcStopped( z21Message_t &msg ) const
{
  fprintf( stderr, "-- LAN_X_BC_STOPPED\n" );

  msg.length              = 0x07;
  msg.header              = 0x40;
  msg.x.xHeader           = 0x81;
  msg.x.db0               = 0x00;
  msg.x.bcStopped.xorByte = msg.x.xor1();

  logMsg( "send", msg );
}

/*!
  Message über Stop alles Loks erzeugen
*/
void z21Base_c::getSendBcStopped( char *buffer, uint16_t &length ) const
{
  z21Message_t &msg = ( z21Message_t & ) *buffer;

  createBcStopped( msg );

  length = msg.length;
}

/*!
  Message über Abschalten der Gleisspannung erzeugen
*/
void z21Base_c::createBcTrackPowerOff( z21Message_t &msg ) const
{
  fprintf( stderr, "-- LAN_X_BC_TRACK_POWER_OFF\n" );

  msg.length                   = 0x07;
  msg.header                   = 0x40;
  msg.x.xHeader                = 0x61;
  msg.x.db0                    = 0x00;
  msg.x.bcTrackPowerOn.xorByte = msg.x.xor1();

  logMsg( "send", msg );
}

/*!
  Message über Abschalten der Gleisspannung erzeugen
*/
void z21Base_c::getSendBcTrackPowerOff( char *buffer, uint16_t &length ) const
{
  z21Message_t &msg = ( z21Message_t & ) *buffer;

  createBcTrackPowerOff( msg );

  length = msg.length;
}

/*!
  Message über Einschalten der Gleisspannung erzeugen
*/
void z21Base_c::createBcTrackPowerOn( z21Message_t &msg ) const
{
  fprintf( stderr, "-- LAN_X_BC_TRACK_POWER_ON\n" );

  msg.length                   = 0x07;
  msg.header                   = 0x40;
  msg.x.xHeader                = 0x61;
  msg.x.db0                    = 0x01;
  msg.x.bcTrackPowerOn.xorByte = msg.x.xor1();

  logMsg( "send", msg );
}

/*!
  Message über Einschalten der Gleisspannung erzeugen
*/
void z21Base_c::getSendBcTrackPowerOn( char *buffer, uint16_t &length ) const
{
  z21Message_t &msg = ( z21Message_t & ) *buffer;

  createBcTrackPowerOn( msg );

  length = msg.length;
}

/*!
  Message über Kurzschluss erzeugen
*/
void z21Base_c::createBcTrackShortCircuit( z21Message_t &msg ) const
{
  fprintf( stderr, "-- LAN_X_BC_TRACK_SHORT_CIRCUIT\n" );

  msg.length                        = 0x07;
  msg.header                        = 0x40;
  msg.x.xHeader                     = 0x61;
  msg.x.db0                         = 0x08;
  msg.x.bcTrackShortCircuit.xorByte = msg.x.xor1();

  logMsg( "send", msg );
}

/*!
  Message über Kurzschluss erzeugen
*/
void z21Base_c::getSendBcTrackShortCircuit( char *buffer, uint16_t &length ) const
{
  z21Message_t &msg = ( z21Message_t & ) *buffer;

  createBcTrackShortCircuit( msg );

  length = msg.length;
}

/*!
  Message über Programmiermodus erzeugen
*/
void z21Base_c::createBcProgrammingMode( z21Message_t &msg ) const
{
  fprintf( stderr, "-- LAN_X_BC_PROGRAMMING_MODE\n" );

  msg.length                      = 0x07;
  msg.header                      = 0x40;
  msg.x.xHeader                   = 0x61;
  msg.x.db0                       = 0x02;
  msg.x.bcProgrammingMode.xorByte = msg.x.xor1();

  logMsg( "send", msg );
}

/*!
  Message über Programmiermodus erzeugen
*/
void z21Base_c::getSendBcProgrammingMode( char *buffer, uint16_t &length ) const
{
  z21Message_t &msg = ( z21Message_t & ) *buffer;

  createBcProgrammingMode( msg );

  length = msg.length;
}

/*!
  Eine empfangene/gesendete Message ausgeben
*/
void z21Base_c::logMsg( const char *label, const z21Message_t &data ) const
{
  const unsigned char *dataPtr = ( unsigned char * ) &data;
  if ( data.length > 0 )
  {
    fprintf( stderr, "%s", label );
    for ( int xx = 0; xx < data.length; xx++ )
    {
      fprintf( stderr, " %02x", dataPtr[ xx ] );
    }
    fprintf( stderr, "\n" );
  }
}
