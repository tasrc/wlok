#include "wlok.h"

#include <QCoreApplication>
#include <QNetworkDatagram>
#include <QUdpSocket>

int main(int argc, char *argv[])
{
  wLok_c a (argc, argv );

  a.initSocket();

  return a.exec();
}

void wLok_c::initSocket()
{
  _udpSocket = new QUdpSocket( this );
  _udpSocket->bind( QHostAddress::Any, 21105 );

  connect( _udpSocket, &QUdpSocket::readyRead, this, &wLok_c::readData );
}

void wLok_c::readData()
{
  while ( _udpSocket->hasPendingDatagrams() )
  {
    QNetworkDatagram datagram = _udpSocket->receiveDatagram();
    QByteArray data = datagram.data();

    auto clientIt = _clientSessions.find( datagram.senderAddress() );
    if ( clientIt == _clientSessions.end() )
    {
      fprintf( stderr, "LOGIN %s\n", qPrintable( datagram.senderAddress().toString() ) );
      clientIt = _clientSessions.insert( datagram.senderAddress(), z21Base_c( this ) );
    }

    auto client = clientIt.value();

    quint16 msgLen = 0;
    for ( quint16 pos = 0; pos < data.size(); pos += msgLen )
    {
      const char *msgData = data.constData() + pos;
      char replyData[ 2000 ];
      quint16 replyLen;

      client.parseMsg( msgData, msgLen, replyData, replyLen );

      if ( replyLen > 0 )
      {
        _udpSocket->writeDatagram( datagram.makeReply( QByteArray::fromRawData( replyData, replyLen ) ) );
      }
    }

    if ( client.isLoggedOff() )
    {
      fprintf( stderr, "LOGOFF %s\n", qPrintable( datagram.senderAddress().toString() ) );
      _clientSessions.remove( datagram.senderAddress() );
    }
  }
}

void wLok_c::stop( bool all )
{
  _centralState |= CS_EMERGENCY_STOP;

  for ( auto it = _clientSessions.begin(), itEnd = _clientSessions.end(); it != itEnd; ++it )
  {
    if ( all || uint32_t( it.value().broadcastFlags() ) & uint32_t( z21Base_c::BROADCAST_AUTOMATIC_MESSAGES ) )
    {
      quint16 sendLen;
      it.value().getSendBcStopped( _sendBuffer, sendLen );
      QNetworkDatagram datagram( QByteArray::fromRawData( _sendBuffer, sendLen ), it.key() );
      _udpSocket->writeDatagram( datagram );
    }
  }
}

void wLok_c::trackPowerOff( bool all )
{
  _centralState |= CS_TRACK_VOLTAGE_OFF;
  _centralState |= CS_EMERGENCY_STOP;

  for ( auto it = _clientSessions.begin(), itEnd = _clientSessions.end(); it != itEnd; ++it )
  {
    if ( all || uint32_t( it.value().broadcastFlags() ) & uint32_t( z21Base_c::BROADCAST_AUTOMATIC_MESSAGES ) )
    {
      quint16 sendLen;
      it.value().getSendBcTrackPowerOff( _sendBuffer, sendLen );
      QNetworkDatagram datagram( QByteArray::fromRawData( _sendBuffer, sendLen ), it.key() );
      _udpSocket->writeDatagram( datagram );
    }
  }
}

void wLok_c::trackPowerOn( bool all )
{
  _centralState &= ~CS_TRACK_VOLTAGE_OFF;
  _centralState &= ~CS_EMERGENCY_STOP;
  _centralState &= ~CS_SHORT_CIRCUIT;

  for ( auto it = _clientSessions.begin(), itEnd = _clientSessions.end(); it != itEnd; ++it )
  {
    if ( all || uint32_t( it.value().broadcastFlags() ) & uint32_t( z21Base_c::BROADCAST_AUTOMATIC_MESSAGES ) )
    {
      quint16 sendLen;
      it.value().getSendBcTrackPowerOn( _sendBuffer, sendLen );
      QNetworkDatagram datagram( QByteArray::fromRawData( _sendBuffer, sendLen ), it.key() );
      _udpSocket->writeDatagram( datagram );
    }
  }
}

void wLok_c::trackShortCircuit()
{
  _centralState &= ~CS_TRACK_VOLTAGE_OFF;
  _centralState &= ~CS_EMERGENCY_STOP;
  _centralState &= ~CS_SHORT_CIRCUIT;

  for ( auto it = _clientSessions.begin(), itEnd = _clientSessions.end(); it != itEnd; ++it )
  {
    if ( uint32_t( it.value().broadcastFlags() ) & uint32_t( z21Base_c::BROADCAST_AUTOMATIC_MESSAGES ) )
    {
      quint16 sendLen;
      it.value().getSendBcTrackShortCircuit( _sendBuffer, sendLen );
      QNetworkDatagram datagram( QByteArray::fromRawData( _sendBuffer, sendLen ), it.key() );
      _udpSocket->writeDatagram( datagram );
    }
  }
}

void wLok_c::programmingMode()
{
  _centralState &= ~CS_PROGRAMMING_MODE_ACTIVE;

  for ( auto it = _clientSessions.begin(), itEnd = _clientSessions.end(); it != itEnd; ++it )
  {
    if ( uint32_t( it.value().broadcastFlags() ) & uint32_t( z21Base_c::BROADCAST_AUTOMATIC_MESSAGES ) )
    {
      quint16 sendLen;
      it.value().getSendBcProgrammingMode( _sendBuffer, sendLen );
      QNetworkDatagram datagram( QByteArray::fromRawData( _sendBuffer, sendLen ), it.key() );
      _udpSocket->writeDatagram( datagram );
    }
  }
}
