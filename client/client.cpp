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

static const int PORT = 5000;

Client::Client(  ClientView * pClientView ) : pcv( pClientView ), MUReceiveSocket( NULL ), MSocketNotifier( NULL ) {
}

Client::~Client(){
  if( MUReceiveSocket )
    delete MUReceiveSocket;
  if( MSocketNotifier )
    delete MSocketNotifier;     
}



void Client::OnMReceive() {           
  if( NULL == MUReceiveSocket )
    return;
    
  int ByteCount, ReadCount;
                      
  ByteCount = MUReceiveSocket->bytesAvailable();
  
  //new a buffer

  char* buf = new char[ ByteCount ];  
  
  ReadCount = MUReceiveSocket->readDatagram(buf, ByteCount );
  
  QByteArray ba;
  ba.setRawData( buf, ByteCount ); 
  
  QDataStream so(  &ba, QIODevice::ReadOnly );

  int width;
  int height;
  int top;
  QPixmap newMap;

  so >> width >> height >> top >> newMap;    
  //update the contents                 
  pcv->redrawPixmap( width, height, top, newMap );     

  //free the buffer
  ba.clear();
  delete [] buf;                 
}

/*
  *bind or rebind to another address
*/
bool Client::bind( const QString FakeAddress ) {   
  if( MUReceiveSocket ) {
      delete MUReceiveSocket;
      MUReceiveSocket = NULL;
  }
  if( MSocketNotifier ) {
      delete MSocketNotifier;
      MSocketNotifier = NULL;
  }
  
  MUReceiveSocket = new QUdpSocket;
  
  QHostAddress MyAddress;   
  MyAddress.setAddress( FakeAddress );                               
  
  bool bindResult =  MUReceiveSocket->bind( MyAddress, PORT );
  //add to the group
  bool setResult = setSocketOpt( MUReceiveSocket, FakeAddress );

  MSocketNotifier = new QSocketNotifier( MUReceiveSocket->socketDescriptor(), QSocketNotifier::Read);

  QObject::connect( MSocketNotifier, SIGNAL( activated( int ) ), this, SLOT( OnMReceive() ) );

  return bindResult & setResult;
}

/*
  *add the socketdevice to a group
*/
bool Client::setSocketOpt( const QUdpSocket * MUReceiveSocket, const QString address ) {
  int s = MUReceiveSocket->socketDescriptor();

  struct ip_mreq mreq;
  if( inet_aton( reinterpret_cast<const char *>(address.data()), &mreq.imr_multiaddr ) == 0 ) {
    QMessageBox::warning( pcv, "Error", "can't construct a multicast address" );
    return false;
  }
  mreq.imr_interface.s_addr = htonl( INADDR_ANY );
  if( setsockopt( s, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof( mreq ) ) < 0 ) {
    QMessageBox::warning( pcv, "Error", "can't add to the group" + address );
    return false;
  }

  return true;
}
