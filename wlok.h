#ifndef _WLOK_H
#define _WLOK_H

#include <QCoreApplication>

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
};

#endif
