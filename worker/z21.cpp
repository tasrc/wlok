#include "z21.h"

#include <stdio.h>

#include "loco.h"
#include "server_base.h"

clientId_t z21Base_c::_nextId = 1;

/*!
  Konstruktor
*/
z21Base_c::z21Base_c( serverBase_c *parent ) :
  _id( _nextId++ ),
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
  reply.length = 0x00;
  reply.header = 0x00;

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
    printf( "-- ERROR: Undocumented message\n" );
    break;

  case 0x18:
    processGetCode( reply );
    break;

  case 0x1a:
    processGetHwInfo( reply );
    break;

  case 0x30:
    processLogoff();
    break;

  case 0x40:
    processX( message, reply );
    break;

  case 0x50:
    processSetBroadcastFlags( message );
    break;

  case 0x51:
    processGetBroadcastFlags( reply );
    break;

  case 0x60:
    processGetLocoMode( message, reply );
    break;

  case 0x61:
    processSetLocoMode( message );
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
  printf( "-- LAN_GET_SERIAL_NUMBER\n" );

  reply.length                    = 0x08;
  reply.header                    = 0x10;
  reply.serialNumber.serialNumber = 12345; // TODO
}

/*!
  Abfrage der Hardwareinformationen bearbeiten
*/
void z21Base_c::processGetHwInfo( z21Message_t &reply )
{
  printf( "-- LAN_GET_HWINFO\n" );

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
  printf( "-- LAN_GET_CODE\n" );

  reply.length    = 0x0c;
  reply.header    = 0x18;
  reply.code.code = 0x00;
}

/*!
  Logoff bearbeiten
*/
void z21Base_c::processLogoff()
{
  printf( "-- LAN_LOGOFF\n" );

  _isLoggedOff = true;
}

/*!
  X-Bus Messages bearbeiten
*/
void z21Base_c::processX( const z21Message_t &msg, z21Message_t &reply )
{
  if ( msg.x.xHeader == 0x21 && msg.x.db0 == 0x21 )
  {
    printf( "-- LAN_X_GET_VERSION\n" );

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
    printf( "-- LAN_X_GET_STATUS\n" );

    reply.length                  = 0x09;
    reply.header                  = 0x40;
    reply.x.xHeader               = 0x62;
    reply.x.db0                   = 0x22;
    reply.x.statusChanged.status  = _parent->centralState();
    reply.x.statusChanged.xorByte = reply.x.xor2();
  }
  else if ( msg.x.xHeader == 0x21 && msg.x.db0 == 0x80 )
  {
    printf( "-- LAN_X_SET_TRACK_POWER_OFF\n" );

    _parent->sendTrackPowerOff( true );
  }
  else if ( msg.x.xHeader == 0x21 && msg.x.db0 == 0x81 )
  {
    printf( "-- LAN_X_SET_TRACK_POWER_ON\n" );

    _parent->sendTrackPowerOn( true );
  }
  else if ( msg.x.xHeader == 0x80 )
  {
    printf( "-- LAN_X_SET_STOP\n" );

    _parent->sendStop( true );
  }
  else if ( msg.x.xHeader == 0xe3 && msg.x.db0 == 0xf0 )
  {
    const locoAddress_t address = ( ( msg.x.getLocoInfo.adrMsb & 0x3f ) << 8 ) + msg.x.getLocoInfo.adrLsb;

    printf( "-- LAN_X_GET_LOCO_INFO %d\n", address );

    createLocoInfo( address, reply );
  }
  else if ( msg.x.xHeader == 0xe4 && ( msg.x.db0 == 0x10 || msg.x.db0 == 0x12 || msg.x.db0 == 0x13 ) )
  {
    processSetLocoDrive( msg );
  }
  else if ( msg.x.xHeader == 0xe4 && msg.x.db0 == 0xf8 )
  {
    processSetLocoFunction( msg );
  }
  else if ( msg.x.xHeader == 0xf1 && msg.x.db0 == 0x0a )
  {
    printf( "-- LAN_X_GET_FIRMWARE_VERSION\n" );

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
    printf( "-- ERROR: Unknown X-Message\n" );
    createUnknownCommand( reply );
  }
}

/*!
  Broadcast Flags setzen
*/
void z21Base_c::processSetBroadcastFlags( const z21Message_t &msg )
{
  printf( "-- LAN_SET_BROADCASTFLAGS\n" );

  _broadcastFlags = msg.setBroadcastFlags.broadcastFlags;
}

/*!
  Broadcast Flags abfragen
*/
void z21Base_c::processGetBroadcastFlags( z21Message_t &reply )
{
  printf( "-- LAN_GET_BROADCASTFLAGS\n" );

  reply.length = 0x08;
  reply.header = 0x051;
  reply.getBroadcastFlags.broadcastFlags = _broadcastFlags;
}

/*!
  Abfrage des Systemstatus bearbeiten
*/
void z21Base_c::processSystemStateGetData( z21Message_t &reply ) const
{
  printf( "-- LAN_SYSTEMSTATE_GETDATA\n" );

  createSystemStateDataChanged( reply );
}

/*!
  Abfrage des Lokmodues bearbeiten
*/
void z21Base_c::processGetLocoMode( const z21Message_t &msg, z21Message_t &reply )
{
  printf( "-- LAN_GET_LOCOMODE\n" );

  locoAddress_t address = msg.getLocoMode.address;
  auto &loco = _parent->loco( address );

  reply.length = 0x07;
  reply.header = 0x60;
  reply.locoMode.address = address;
  reply.locoMode.mode    = loco.addressFormat();
}

/*!
  Setzen des Lokmodus bearbeiten
*/
void z21Base_c::processSetLocoMode( const z21Message_t &msg )
{
  locoAddress_t address = msg.setLocoMode.address;
  auto &loco = _parent->loco( address );

  printf( "-- LAN_GET_LOCOMODE %d\n", address );

  loco.setAddressFormat( locoBase_c::addressFormat_t( msg.setLocoMode.mode ) );
}

/*!
  Setzen des Fahrstufe bearbeiten
*/
void z21Base_c::processSetLocoDrive( const z21Message_t &msg )
{
  const locoAddress_t address = ( ( msg.x.setLocoDrive.adrMsb & 0x3f ) << 8 ) + msg.x.setLocoDrive.adrLsb;
  auto &loco = _parent->loco( address );

  printf( "-- LAN_X_SET_LOCO_DRIVE %d\n", address );

  locoBase_c::speedMode_t speedMode = loco.speedMode();

  switch ( msg.x.db0 )
  {
  case 0x10: speedMode = locoBase_c::SPEED_MODE_14;  break;
  case 0x12: speedMode = locoBase_c::SPEED_MODE_28;  break;
  case 0x13: speedMode = locoBase_c::SPEED_MODE_128; break;
  }

  loco.setSpeedMode( speedMode );
  loco.setCodedSpeed( msg.x.setLocoDrive.speed );

  _parent->sendLocoInfo( address );
}

/*!
  Setzen einer Funktion bearbeiten
*/
void z21Base_c::processSetLocoFunction( const z21Message_t &msg )
{
  const locoAddress_t address = ( ( msg.x.setLocoFunction.adrMsb & 0x3f ) << 8 ) + msg.x.setLocoFunction.adrLsb;
  auto &loco = _parent->loco( address );

  printf( "-- LAN_X_SET_LOCO_FUNCTION %d\n", address );

  uint8_t operation = ( msg.x.setLocoFunction.function & 0xc0 ) >> 6;
  uint8_t function = msg.x.setLocoFunction.function & 0x3f;

  switch ( operation )
  {
  case 1:
    loco.setFunction( function, true );
    break;

  case 2:
    loco.toggleFunction( function );
    break;

  default:
    loco.setFunction( function, false );
    break;
  }

  _parent->sendLocoInfo( address );
}

/*!
  Message über Änderung des Systemstatus erzeugen
*/
void z21Base_c::createSystemStateDataChanged( z21Message_t &msg ) const
{
  printf( "-- LAN_SYSTEMSTATE_DATACHANGED\n" );

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
  printf( "-- ERROR: Unknown Command\n" );

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
  printf( "-- LAN_X_BC_STOPPED\n" );

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
  printf( "-- LAN_X_BC_TRACK_POWER_OFF\n" );

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
  printf( "-- LAN_X_BC_TRACK_POWER_ON\n" );

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
  printf( "-- LAN_X_BC_TRACK_SHORT_CIRCUIT\n" );

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
  printf( "-- LAN_X_BC_PROGRAMMING_MODE\n" );

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
  Message mit Lokinformationen erzeugen
*/
void z21Base_c::createLocoInfo( const locoAddress_t &address, z21Message_t &msg ) const
{
  printf( "-- LAN_X_LOCO_INFO %d\n", address );

  _parent->subscribeLoco( address, _id );

  auto &loco = _parent->loco( address );

  uint8_t adrMsb = address >> 8;
  if ( address >= 128 )
  {
    adrMsb |= 0xc0;
  }

  const uint8_t adrLsb = address & 0xff;

  const clientId_t locoController = _parent->locoController( address );
  const bool hasOtherController = locoController != 0 && locoController != _id;
  uint8_t speedMode = loco.speedMode();
  if ( locoController != 0 && locoController != _id )
  {
    speedMode |= 0x08;
  }

  uint8_t function0 = 0x00;
  uint8_t function1 = 0x00;
  uint8_t function2 = 0x00;
  uint8_t function3 = 0x00;
  int functionIndex = 0;

  if ( loco.multipleUnit() )
  {
    function0 |= 0x40;
  }
  if ( loco.function( functionIndex++ ) )
  {
    function0 |= 0x10;
  }

  for ( int bit = 0; bit < 4; bit++ )
  {
    if ( loco.function( functionIndex++ ) )
    {
      function0 |= ( 1 << bit );
    }
  }

  for ( int bit = 0; bit < 8; bit++ )
  {
    if ( loco.function( functionIndex++ ) )
    {
      function1 |= ( 1 << bit );
    }
  }

  for ( int bit = 0; bit < 8; bit++ )
  {
    if ( loco.function( functionIndex++ ) )
    {
      function2 |= ( 1 << bit );
    }
  }

  for ( int bit = 0; bit < 8; bit++ )
  {
    if ( loco.function( functionIndex++ ) )
    {
      function3 |= ( 1 << bit );
    }
  }

  msg.length               = 0x0d;
  msg.header               = 0x40;
  msg.x.xHeader            = 0xef;
  msg.x.db0                = adrMsb;
  msg.x.locoInfo.adrLsb    = adrLsb;
  msg.x.locoInfo.speedMode = speedMode;
  msg.x.locoInfo.speed     = loco.codedSpeed();
  msg.x.locoInfo.function0 = function0;
  msg.x.locoInfo.function1 = function1;
  msg.x.locoInfo.function2 = function2;
  msg.x.locoInfo.function3 = function3;
  msg.x.locoInfo.xorByte   = msg.x.xor8();

  logMsg( "send", msg );
}

/*!
  Message mit Lokinformationen erzeugen
*/
void z21Base_c::getSendLocoInfo( const locoAddress_t &address, char *buffer, uint16_t &length ) const
{
  z21Message_t &msg = ( z21Message_t & ) *buffer;

  createLocoInfo( address, msg );

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
    printf( "%s", label );
    for ( int xx = 0; xx < int( data.length ); xx++ )
    {
      printf( " %02x", dataPtr[ xx ] );
    }
    printf( "\n" );
  }
}
