#pragma once

#include <QCoreApplication>
#include <QHostAddress>
#include <QMap>

#include "z21.h"

class QUdpSocket;

class clientAddress_c : public QHostAddress
{
public:
  clientAddress_c( const QHostAddress host ) :
    QHostAddress( host )
  {
  }

  bool operator<( const clientAddress_c &other ) const
  {
    return toString() < other.toString();
  }
};

class wLok_c : public QCoreApplication, public serverBase_c
{
public:
  wLok_c( int argc, char *argv[] ) :
    QCoreApplication( argc, argv )
  {
  }

  void initSocket();
  void programmingMode();
  void stop( bool );
  void trackPowerOff( bool );
  void trackPowerOn( bool );
  void trackShortCircuit();

public slots:
  void readData();

private:
  QMap< clientAddress_c, z21Base_c >  _clientSessions;
  char                                _sendBuffer[ 2000 ];
  QUdpSocket                         *_udpSocket;
};
