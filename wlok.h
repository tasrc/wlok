#pragma once

#include <QCoreApplication>
#include <QHostAddress>
#include <QMap>

#include "loco.h"
#include "server_base.h"
#include "z21.h"

class QUdpSocket;

class wLok_c;

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

class loco_c : public locoBase_c
{
public:
  loco_c( wLok_c *, const locoAddress_t & );

  inline clientId_t master() const { return _master; }
  void              remove( const clientId_t & );
  void              setMaster( const clientId_t & );
  void              subscribe( const clientId_t & );

private:
  loco_c() = delete;

  QSet< clientId_t > _subscriptions;
  clientId_t         _master = 0;
};

class wLok_c : public QCoreApplication, public serverBase_c
{
public:
  wLok_c( int argc, char *argv[] ) :
    QCoreApplication( argc, argv )
  {
  }

  void       initSocket();
  clientId_t locoMaster( const locoAddress_t & );
  loco_c &   loco( const locoAddress_t & );
  void       programmingMode();
  void       setLocoMaster( const locoAddress_t &, const clientId_t & );
  void       stop( bool );
  void       subscribeLoco( const locoAddress_t &, const clientId_t & );
  void       trackPowerOff( bool );
  void       trackPowerOn( bool );
  void       trackShortCircuit();

public slots:
  void readData();

private:
  void logoff( const clientAddress_c & );

  QMap< clientAddress_c, z21Base_c >  _clientSessions;
  QMap< locoAddress_t, loco_c >       _locos;
  char                                _sendBuffer[ 2000 ];
  QUdpSocket                         *_udpSocket;
};
