#include <QCoreApplication>
#include <QNetworkDatagram>
#include <QUdpSocket>

#include "wlok.h"

namespace
{
  void fromLittleEndian( const char *data, quint16 &result )
  {
#if 1
    ( ( char * ) &result )[ 0 ] = *data;
    ( ( char * ) &result )[ 1 ] = *( data + 1 );
#else
    ( ( char * ) &result )[ 1 ] = *data;
    ( ( char * ) &result )[ 0 ] = *( data + 1 );
#endif
  }

  void toLittleEndian( const quint16 value, char *dest )
  {
#if 1
    *dest = ( ( char * ) &value )[ 0 ];
    *( dest + 1 ) = ( ( char * ) &value )[ 1 ];
#else
    *dest = ( ( char * ) &value )[ 1 ];
    *( dest + 1 ) = ( ( char * ) &value )[ 0 ];
#endif
  }

}

int main(int argc, char *argv[])
{
  wLok_c a (argc, argv );

  a.initSocket();

  return a.exec();
}

void wLok_c::initSocket()
{
  _udpSocket = new QUdpSocket(this);
  _udpSocket->bind( QHostAddress::Any, 21105 );

  connect( _udpSocket, &QUdpSocket::readyRead, this, &wLok_c::readData );
}

void wLok_c::readData()
{
  while ( _udpSocket->hasPendingDatagrams() )
  {
    QNetworkDatagram datagram = _udpSocket->receiveDatagram();
    QByteArray data = datagram.data();

//    fprintf( stderr, "datagram %03d\n", data.size() );

    quint16 msgLen = 0;
    for ( quint16 pos = 0; pos < data.size(); pos += msgLen )
    {
      const char *msgData = data.constData() + pos;
      char replyData[ 2000 ];
      quint16 replyLen;

      fromLittleEndian( msgData, msgLen );

      parseMsg( msgData, msgLen, replyData, replyLen );

      if ( replyLen > 0 )
      {
        _udpSocket->writeDatagram( datagram.makeReply( QByteArray::fromRawData( replyData, replyLen ) ) );
      }
    }
  }
}

void wLok_c::parseMsg( const char *msgData, quint16 msgLen, char *replyData, quint16 &replyLen )
{
  quint16 header;
  fromLittleEndian( msgData + 2, header );

  fprintf( stderr, "\nmessage %02x %02x (", msgLen, header );
  for ( int xx = 0; xx < msgLen; xx++ )
  {
    fprintf( stderr, " %02x", ( unsigned char ) msgData[ xx ] );
  }
  fprintf( stderr, " )\n" );

  const char *payload = msgData + 4;

  switch ( header )
  {
  case 0x10:
    getSerialNumber( replyData, replyLen );
    break;

  case 0x30:
    logoff( replyData, replyLen );
    break;

  case 0x40:
    processX( payload, replyData, replyLen );
    break;

  case 0x50:
    setBroadcastFlags( payload, replyData, replyLen );
    break;

  case 0x85:
    getSystemState( replyData, replyLen );
    break;

  default:
    unknownCommand( replyData, replyLen );
  }

  fprintf( stderr, "reply %02x (", replyLen );
  for ( int xx = 0; xx < replyLen; xx++ )
  {
    fprintf( stderr, " %02x", ( unsigned char ) replyData[ xx ] );
  }
  fprintf( stderr, " )\n" );
}

void wLok_c::getSerialNumber( char *replyData, quint16 &replyLen ) const
{
  fprintf( stderr, "-- LAN_GET_SERIAL_NUMBER\n" );

  replyLen = 0x8;

  replyHeader( replyData, replyLen, 0x10 );

  static const quint32 serialNumber = 12345;
  memcpy( replyData + 4, &serialNumber, 4 );
}

void wLok_c::logoff( char *, quint16 &replyLen )
{
  fprintf( stderr, "-- LAN_LOGOFF\n" );

  replyLen = 0;
}

void wLok_c::processX( const char *msgData, char *replyData, quint16 &replyLen )
{
  quint8 xHeader = msgData[ 0 ];
  quint8 dbo = msgData[ 1 ];

  if ( xHeader == 0xf1 && dbo == 0xa )
  {
    fprintf( stderr, "-- LAN_X_GET_FIRMWARE_VERSION\n" );

    replyLen = 0x9;
    replyHeader( replyData, replyLen, 0x40 );
    replyData[ 4 ] = 0xf3;
    replyData[ 5 ] = 0xa;
    replyData[ 6 ] = 0x1;
    replyData[ 7 ] = 0x30;
    replyData[ 8 ] = 0xdb;
  }
  else
  {
    fprintf( stderr, "-- ERROR: Unknown X-Message\n" );
    replyLen = 0;
  }
}

void wLok_c::setBroadcastFlags( const char *msgData, char *replyData, quint16 &replyLen )
{
  fprintf( stderr, "-- LAN_SET_BROADCASTFLAGS\n" );

  replyLen = 0;
}

void wLok_c::getSystemState( char *replyData, quint16 &replyLen ) const
{
  fprintf( stderr, "-- LAN_SYSTEMSTATE_GETDATA\n" );

  replyLen = 0x14;

  replyHeader( replyData, replyLen, 0x84 );

  qint16 mainCurrent = 0;
  qint16 progCurrent = 0;
  qint16 filteredMainCurrent = 0;
  qint16 temperature = 40;
  quint16 supplyVoltage = 0;
  quint16 vccVoltage = 0;
  quint8 centralState = 0x3;
  quint8 centralStateEx = 0;

  memcpy( replyData + 4, &mainCurrent, 2 );
  memcpy( replyData + 6, &progCurrent, 2 );
  memcpy( replyData + 8, &filteredMainCurrent, 2 );
  memcpy( replyData + 10, &temperature, 2 );
  memcpy( replyData + 12, &supplyVoltage, 2 );
  memcpy( replyData + 14, &vccVoltage, 2 );
  replyData[ 16 ] = centralState;
  replyData[ 17 ] = centralStateEx;
  replyData[ 18 ] = 0;
  replyData[ 19 ] = 0;
}

void wLok_c::replyHeader( char *replyData, quint16 len, quint16 header ) const
{
  toLittleEndian( len, replyData );
  toLittleEndian( header, replyData + 2 );
}

void wLok_c::unknownCommand( char *replyData, quint16 &replyLen ) const
{
  fprintf( stderr, "-- ERROR: Unknown Command\n" );

  replyLen = 0x7;

  replyHeader( replyData, replyLen, 0x40 );
  replyData[ 4 ] = 0x61;
  replyData[ 5 ] = 0x82;
}
