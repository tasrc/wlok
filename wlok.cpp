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

    const QString clientAddress = datagram.senderAddress().toString();
    auto clientIt = _clientSessions.find( clientAddress );
    if ( clientIt == _clientSessions.end() )
    {
      fprintf( stderr, "LOGIN %s\n", qPrintable( clientAddress ) );
      clientIt = _clientSessions.insert( clientAddress, z21Base_c( this ) );
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
      fprintf( stderr, "LOGOFF %s\n", qPrintable( clientAddress ) );
      _clientSessions.remove( clientAddress );
    }
  }
}

void wLok_c::stop( bool all )
{
  for ( auto it = _clientSessions.begin(), itEnd = _clientSessions.end(); it != itEnd; ++it )
  {
    if ( all || uint32_t( it.value().broadcastFlags() ) & uint32_t( z21Base_c::BROADCAST_AUTOMATIC_MESSAGES ) )
    {
      // TODO
    }
  }
}
