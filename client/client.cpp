#include "client.h"
#include <qhostaddress.h>
#include <qstring.h>
#include <qpixmap.h>
#include <qdatastream.h>
#include <qmessagebox.h>
#include <QUdpSocket>
#include <qsocketnotifier.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "clientview.h"

using namespace std;

static const int PORT = 5000;

Client::Client(  ClientView * pClientView ) : pcv_( pClientView ) {
}

Client::~Client(){  
}

void Client::OnMReceive() {
  const auto ByteCount = receiveSocket_->bytesAvailable();
  
  //new a buffer

  char buf[ByteCount];
  
  const auto ReadCount = receiveSocket_->readDatagram(buf, ByteCount );
  
  QByteArray ba;
  ba.setRawData( buf, ReadCount );
  
  QDataStream so(&ba, QIODevice::ReadOnly);

  int width;
  int height;
  int top;
  QPixmap newMap;

  so >> width >> height >> top >> newMap;    
  //update the contents                 
  pcv_->redrawPixmap( width, height, top, newMap );
}

/*
  *bind or rebind to another address
*/
bool Client::bind( const QString FakeAddress ) {
  receiveSocket_.reset(new QUdpSocket);
  
  QHostAddress MyAddress( FakeAddress );
  
  bool bindResult =  receiveSocket_->bind( MyAddress, PORT );
  //add to the group
  bool setResult = setSocketOpt( receiveSocket_, FakeAddress );

  socketNotifier_.reset(new QSocketNotifier( receiveSocket_->socketDescriptor(), QSocketNotifier::Read));

  QObject::connect( socketNotifier_.get(), SIGNAL( activated( int ) ), this, SLOT( OnMReceive() ) );

  return bindResult & setResult;
}

/*
  *add the socketdevice to a group
*/
bool Client::setSocketOpt(unique_ptr<QUdpSocket> & MUReceiveSocket, const QString address ) {
  int s = MUReceiveSocket->socketDescriptor();

  struct ip_mreq mreq;
  if( inet_aton( reinterpret_cast<const char *>(address.data()), &mreq.imr_multiaddr ) == 0 ) {
    QMessageBox::warning( pcv_.get(), "Error", "can't construct a multicast address" );
    return false;
  }
  mreq.imr_interface.s_addr = htonl( INADDR_ANY );
  if( setsockopt( s, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof( mreq ) ) < 0 ) {
    QMessageBox::warning( pcv_.get(), "Error", "can't add to the group" + address );
    return false;
  }

  return true;
}
