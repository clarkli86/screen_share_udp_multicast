#ifndef CLIENT_H
#define CLIENT_H

#include <qobject.h>


/** This class is used to receive the udp packet from the server and convert it to a pixmap
  */
class QUdpSocket;
class QSocketNotifier;
class ClientView;

class Client : public QObject  {
    Q_OBJECT
public: 
	Client( ClientView *  );
	~Client();
  //bind the multicast address
  bool bind( const QString );

private:
  bool setSocketOpt( const QUdpSocket *, const QString );
   
public slots:
  //receive udp packet
  void OnMReceive();    
  
private:
  ClientView * pcv;     
  QUdpSocket * MUReceiveSocket;
  QSocketNotifier * MSocketNotifier;
};

#endif
