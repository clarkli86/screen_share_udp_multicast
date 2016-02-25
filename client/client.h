#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QUdpSocket>
#include <QSocketNotifier>
#include <memory>

class ClientView;

class Client : public QObject  {
    Q_OBJECT
public: 
	Client( ClientView *  );
	~Client();
  //bind the multicast address
  bool bind( const QString );

private:
  bool setSocketOpt(std::unique_ptr<QUdpSocket> &, const QString );
   
public slots:
  //receive udp packet
  void OnMReceive();    
  
private:
  std::unique_ptr<ClientView> pcv_;
  std::unique_ptr<QUdpSocket> receiveSocket_;
  std::unique_ptr<QSocketNotifier> socketNotifier_;
};

#endif
