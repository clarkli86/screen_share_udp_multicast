#include <QUdpSocket>
#include <qsocketnotifier.h>
#include <qhostaddress.h>
#include <qmessagebox.h>
#include <QMenu>
#include <qtimer.h>
#include <unistd.h>
#include "clientview.h"
#include "sensor.h"

static const int PORT = 5000;
#define BROADCAST "224.0.0.1"

Sensor::Sensor( ClientView * pClientView ) : pcv( pClientView )  {
  //bind to the broadcast address  
  if( ! bind( BROADCAST ) ) {
    QMessageBox::warning( pcv, "Error", "can't bind to the broadcast address, the application will quit!" );
//    pcv->quit();
    exit( 0 );
  }

  pTimer = new QTimer( this );
  connect( pTimer, SIGNAL( timeout() ), this, SLOT( timerDone() ));
  pTimer->start( 5000 );
               
}

Sensor::~Sensor(){
  if( pTimer ) {
    pTimer->stop();
    delete pTimer;
  }
  if( MUReceiveSocket )
    delete MUReceiveSocket;
  if( MSocketNotifier )
    delete MSocketNotifier;    
}


void Sensor::OnMReceive() {          
 
  int ByteCount, ReadCount;

  ByteCount = MUReceiveSocket->bytesAvailable();

  //new a buffer   
  char* buf = new char[ ByteCount ];

  ReadCount = MUReceiveSocket->readDatagram( buf, ByteCount );
  
  QString address( buf );    
  
  newServers.insert( address );
  
  delete [] buf;
}
/*
  *bind to the specified multicast address
*/
bool Sensor::bind( const QString FakeAddress ) {    
  if( MUReceiveSocket ) {
      delete MUReceiveSocket;
      MUReceiveSocket = NULL;
  }
  if( MSocketNotifier ) {
      delete MSocketNotifier;
      MSocketNotifier = NULL;
  }

  MUReceiveSocket = new QUdpSocket();

  QHostAddress MyAddress;
  MyAddress.setAddress( FakeAddress );

  bool result =  MUReceiveSocket->bind( MyAddress, PORT );

  MSocketNotifier = new QSocketNotifier( MUReceiveSocket->socketDescriptor(), QSocketNotifier::Read);

  QObject::connect( MSocketNotifier, SIGNAL( activated( int ) ), this, SLOT( OnMReceive() ) );

  return result;
}

void Sensor::timerDone() {   
  oldServers = newServers;
  newServers.clear();
}

void Sensor::refresh() {   
  QMenu* pServerList = pcv->getServerList();
  QString preAddress;
  // Refresh the list
}
