#include <QUdpSocket>
#include <qsocketnotifier.h>
#include <qhostaddress.h>
#include <qmessagebox.h>
#include <QMenu>
#include <qtimer.h>
#include <unistd.h>
#include "clientview.h"
#include "sensor.h"
using namespace std;

static const int PORT = 5000;
/// Receive server address from this multicat address
static const char * BROADCAST = "224.0.0.1";

Sensor::Sensor( ClientView * pClientView ) : pcv_( pClientView )  {
  //bind to the broadcast address  
  if( ! bind( BROADCAST ) ) {
    QMessageBox::warning( pcv_, "Error", "can't bind to the broadcast address, the application will quit!" );
    exit( 0 );
  }

  connect( &timer_, SIGNAL( timeout() ), this, SLOT( timerDone() ));
  timer_.start( 5000 );
}

Sensor::~Sensor(){  
}


void Sensor::OnMReceive() {           
  const auto ByteCount = receiveSocket_->bytesAvailable();

  //new a buffer   
  char buf[ByteCount];

  const auto ReadCount = receiveSocket_->readDatagram( buf, ByteCount );
  
  QString address(reinterpret_cast<QChar*>(buf), ReadCount);
  
  lock_guard<mutex> guard(serversMutex_);
  newServers_.insert( address );
}
/*
  *bind to the specified multicast address
*/
bool Sensor::bind( const QString & FakeAddress ) {
  receiveSocket_.reset(new QUdpSocket());

  QHostAddress MyAddress;
  MyAddress.setAddress( FakeAddress );

  bool result =  receiveSocket_->bind( MyAddress, PORT );

  socketNotifier_.reset(new QSocketNotifier( receiveSocket_->socketDescriptor(), QSocketNotifier::Read));

  QObject::connect( socketNotifier_.get(), SIGNAL( activated( int ) ), this, SLOT( OnMReceive() ) );

  return result;
}

void Sensor::timerDone() {   
    lock_guard<mutex> guard(serversMutex_);

    oldServers_ = newServers_;
    newServers_.clear();
}

void Sensor::refresh() {   
    QMenu* serverList = pcv_->getServerList();
    serverList->clear();

    lock_guard<mutex> guard(serversMutex_);
    for(auto & server : oldServers_) {
        serverList->addAction(server);
    }
}
