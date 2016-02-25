#ifndef SENSOR_H
#define SENSOR_H

#include <QObject>
#include <set>
#include <QString>
#include <QUdpSocket>
#include <QSocketNotifier>
#include <QTimer>
#include <memory>
#include <mutex>
class ClientView;

class Sensor : public QObject  {
   Q_OBJECT    
public: 
    Sensor( ClientView * pCientView );
	~Sensor();
  void refresh();
private:
  //bind the multicast address
  bool bind( const QString &);
  
public slots:
  //receive udp packet
  void OnMReceive();
private slots:
  //time out
  void timerDone();

private:
  std::unique_ptr<QUdpSocket> receiveSocket_;
  std::unique_ptr<QSocketNotifier> socketNotifier_;
  ClientView * pcv_;
  QTimer timer_;
  std::set< QString > newServers_;
  std::set< QString > oldServers_;

  /// Protect the modification of servers
  std::mutex serversMutex_;
};

#endif
