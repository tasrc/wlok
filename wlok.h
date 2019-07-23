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
  void getSerialNumber( char *, quint16 & ) const;
  void getSystemState( char *, quint16 & ) const;
  void logoff( char *, quint16 & );
  void parseMsg( const char *, quint16, char *, quint16 & );
  void processX( const char *, char *, quint16 &  );
  void replyHeader( char *, quint16, quint16 ) const;
  void setBroadcastFlags( const char *, char *, quint16 & );
  void unknownCommand( char *, quint16 & ) const;

  QUdpSocket *_udpSocket;
};

#endif
