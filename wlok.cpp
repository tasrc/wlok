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

//    fprintf( stderr, "datagram %03d\n", data.size() );

    quint16 msgLen = 0;
    for ( quint16 pos = 0; pos < data.size(); pos += msgLen )
    {
      const char *msgData = data.constData() + pos;
      char replyData[ 2000 ];
      quint16 replyLen;

      _z21.parseMsg( msgData, msgLen, replyData, replyLen );

      if ( replyLen > 0 )
      {
        _udpSocket->writeDatagram( datagram.makeReply( QByteArray::fromRawData( replyData, replyLen ) ) );
      }
    }
  }
}
