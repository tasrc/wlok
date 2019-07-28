#pragma once

#include <QCoreApplication>
#include <QMap>
#include <QString>

#include "z21.h"

class QUdpSocket;

class wLok_c : public QCoreApplication, public serverBase_c
{
public:
  wLok_c( int argc, char *argv[] ) :
    QCoreApplication( argc, argv )
  {
  }

  void initSocket();
  void stop( bool );

public slots:
  void readData();

private:
  QMap< QString, z21Base_c >  _clientSessions;
  QUdpSocket                 *_udpSocket;
};
