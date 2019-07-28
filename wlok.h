#pragma once

#include <QCoreApplication>

#include "z21.h"

class QUdpSocket;

class wLok_c : public QCoreApplication
{
public:
  wLok_c( int argc, char *argv[] ) :
    QCoreApplication( argc, argv )
  {
  }

  void initSocket();

public slots:
  void readData();

private:
  QUdpSocket *_udpSocket;
  z21Base_c  _z21;
};
