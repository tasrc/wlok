#include <QCoreApplication>
#include <QNetworkDatagram>
#include <QUdpSocket>

#include "wlok.h"

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
fprintf( stderr, "tas01\n" );
//    processTheDatagram(datagram);
  }
}
