#ifndef SENSOR_H
#define SENSOR_H


#include <qobject.h>
#include <set>
using namespace std;

class QUdpSocket;
class QSocketNotifier;
class ClientView;
class QTimer;

class Sensor : public QObject  {
   Q_OBJECT    
public: 
	Sensor( ClientView * pCientView );
	~Sensor();
  void refresh();
private:
  //bind the multicast address
  bool bind( const QString );
  
public slots:
  //receive udp packet
  void OnMReceive();
private slots:
  //time out
  void timerDone();

private:
  QUdpSocket * MUReceiveSocket;
  QSocketNotifier * MSocketNotifier;
  ClientView * pcv;
  QTimer * pTimer;
  set< QString > newServers;
  set< QString > oldServers;
};

#endif
