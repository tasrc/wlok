#pragma once

#include <QCoreApplication>
#include <QHostAddress>
#include <QMap>

#include "worker/loco.h"
#include "worker/server_base.h"
#include "worker/z21.h"

class QUdpSocket;

class masterServer_c;

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

class masterLoco_c : public locoBase_c
{
public:
  masterLoco_c( masterServer_c *, const locoAddress_t & );

  bool              isSubscriber( const clientId_t & ) const;
  inline clientId_t controller() const { return _controller; }
  void              remove( const clientId_t & );
  void              setController( const clientId_t & );
  void              subscribe( const clientId_t & );

private:
  masterLoco_c() = delete;

  QSet< clientId_t > _subscriptions;
  clientId_t         _controller = 0;
};

class masterServer_c : public QCoreApplication, public serverBase_c
{
public:
  masterServer_c( int argc, char *argv[] ) :
    QCoreApplication( argc, argv )
  {
  }

  void           initSocket();
  masterLoco_c & loco( const locoAddress_t & );
  clientId_t     locoController( const locoAddress_t & );
  void           sendLocoInfo( const locoAddress_t & );
  void           sendProgrammingMode();
  void           sendStop( bool );
  void           sendTrackPowerOff( bool );
  void           sendTrackPowerOn( bool );
  void           sendTrackShortCircuit();
  void           setLocoController( const locoAddress_t &, const clientId_t & );
  void           subscribeLoco( const locoAddress_t &, const clientId_t & );

public slots:
  void readData();

private:
  void logoff( const clientAddress_c & );

  QMap< clientAddress_c, z21Base_c >  _clientSessions;
  QMap< locoAddress_t, masterLoco_c > _locos;
  char                                _sendBuffer[ 2000 ];
  QUdpSocket                         *_udpSocket;
};
