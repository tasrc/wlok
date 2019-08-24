#include "master/wlok.h"

#include <QCoreApplication>
#include <QNetworkDatagram>
#include <QUdpSocket>

int main(int argc, char *argv[])
{
  wLok_c a (argc, argv );

  a.initSocket();

  return a.exec();
}

/*********************************************************************************************************************/

loco_c::loco_c( wLok_c *parent, const locoAddress_t &address ) :
  locoBase_c( parent, address )
{
}

void loco_c::remove( const clientId_t &id )
{
  if ( _controller == id )
  {
    _controller = 0;
  }

  _subscriptions.remove( id );
}

void loco_c::setController( const clientId_t &id )
{
  _controller = id;
}

void loco_c::subscribe( const clientId_t &id )
{
  _subscriptions.insert( id );
}

bool loco_c::isSubscriber( const clientId_t &id ) const
{
  return _subscriptions.contains( id );
}

/*********************************************************************************************************************/

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

    auto &client = clientIt.value();

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
      logoff( datagram.senderAddress() );
    }
  }
}

void wLok_c::sendStop( bool all )
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

void wLok_c::sendTrackPowerOff( bool all )
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

void wLok_c::sendTrackPowerOn( bool all )
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

void wLok_c::sendTrackShortCircuit()
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

void wLok_c::sendProgrammingMode()
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

void wLok_c::sendLocoInfo( const locoAddress_t &address )
{
  const loco_c l = loco( address );

  for ( auto it = _clientSessions.begin(), itEnd = _clientSessions.end(); it != itEnd; ++it )
  {
    if ( l.isSubscriber( it.value().id() ) )
    {
      quint16 sendLen;
      it.value().getSendLocoInfo( address, _sendBuffer, sendLen );
      QNetworkDatagram datagram( QByteArray::fromRawData( _sendBuffer, sendLen ), it.key() );
      _udpSocket->writeDatagram( datagram );
    }
  }
}

loco_c & wLok_c::loco( const locoAddress_t &address )
{
  auto it = _locos.find( address );

  if ( it == _locos.end() )
  {
    it = _locos.insert( address, loco_c( this, address ) );
  }

  return it.value();
}

void wLok_c::logoff( const clientAddress_c &address )
{
  fprintf( stderr, "LOGOFF %s\n", qPrintable( address.toString() ) );

  auto clientIt = _clientSessions.find( address );
  if ( clientIt != _clientSessions.end() )
  {
    auto &client = clientIt.value();

    for ( auto &loco : _locos )
    {
      loco.remove( client.id() );
    }
  }

  _clientSessions.remove( address );
}

void wLok_c::setLocoController( const locoAddress_t &address, const clientId_t &id )
{
  auto &l = loco( address );
  if ( l.controller() == 0 )
  {
    l.setController( id );
  }
}

void wLok_c::subscribeLoco( const locoAddress_t &address, const clientId_t &id )
{
  for ( auto it = _clientSessions.begin(), itEnd = _clientSessions.end(); it != itEnd; ++it )
  {
    if ( it.value().id() == id &&
         ( uint32_t( it.value().broadcastFlags() ) & uint32_t( z21Base_c::BROADCAST_AUTOMATIC_MESSAGES ) ) )
    {
      loco( address ).subscribe( id );
      return;
    }
  }
}

clientId_t wLok_c::locoController( const locoAddress_t &address )
{
  loco( address ).controller();
}
