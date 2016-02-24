#include "clientview.h"
#include <qpainter.h>           
#include <qmenubar.h>
#include <QMenu>
#include <qmessagebox.h>
#include <qtimer.h>
#include <unistd.h>
#include "client.h"
#include "sensor.h"
#include <qevent.h>
ClientView::ClientView(QWidget *parent, const char *name ) : QScrollArea(parent)  {
   bFullScreen = false;
   bShowMenu = true;
   setWindowTitle( "Lan Screen Capture" );
   //set the timer
   pTimer = new QTimer( this );
   connect( pTimer, SIGNAL( timeout() ), this, SLOT( timeout() ) );
   pTimer->start( 500 );
}


ClientView::~ClientView(){
       delete pTimer;	
}


void ClientView::start() {
  pClient = new Client( this );
   pSensor = new Sensor( this );
   //initial the menubar
   pMenuBar = new QMenuBar( this );

   pMenu = new QMenu( this );

   pServerList = new QMenu("Options", this);
   //insert into the option
   pMenu->addAction( "&Refresh server list", this, SLOT( refreshMenu() ) );
   QMenu * servers = new QMenu("Select a server", this);
   pMenu->addMenu(servers );
   pMenu->addAction( "Show&Menu", this, SLOT( showMenu() ) );
   pMenu->addAction( "&Quit", this, SLOT( quit() ) );

   pMenuBar->addMenu(pMenu );

   QMenu * pAbout = new QMenu("Help", this );
   pAbout->addAction( "Abort", this, SLOT( showAbout() ) );
   pMenuBar->addMenu(pAbout );
}

void ClientView::drawContents( QPainter * p, int clipx, int clipy, int clipw, int cliph ) {   
  if( true == bFullScreen ) {
  //hide the scroll bar
    //this->resizeContents( 0, 0 );
    }
  else
    //if( contentsWidth() != map.width() || contentsHeight() != map.height() )
    //    resizes( map.width(), map.height() );

  p->drawPixmap( clipx, clipy, map, clipx, clipy, clipw, cliph );
  
}        

void ClientView::contentsMouseDoubleClickEvent( QMouseEvent * e ) {
    if( Qt::LeftButton != e->button() )
      return;
      
    if( false == bFullScreen ) {  
      showFullScreen();
      pMenuBar->hide();
      bFullScreen = true;
    }
    else {
      showNormal();
      pMenuBar->show();
      bFullScreen = false;
    }
}       

void ClientView::contentsMouseReleaseEvent ( QMouseEvent * e ) {
  if( Qt::RightButton != e->button() )
    return;                
 
  pMenu->popup( e->globalPos() );
}

void ClientView::redrawPixmap( const int width, const int height, const int top, const QPixmap& newMap ) {
  //resize the map
    if( map.width() != width || map.height() != height ) {
      //map.resize( width, height );
    }
  //bitBit the new Map to the specified to the map       
    QPainter painter(&map);
    painter.drawPixmap(0, top, newMap, 0, 0, newMap.width(), newMap.height());
//    updateContents();
}

//it's called after every 0.5s
void ClientView::timeout() {
	//redraw the content
    repaint();
}

void ClientView::quit() {
  //tell the qapplication to quit
  emit quitSignal();
}

void ClientView::changeServer( int nId ) {
  // Get address on the server and connect to it
  /*
  if( ! pClient->bind( address ) ) {
    QMessageBox::warning( this, "Error", "Can't bind to the address " + address );
    pServerList->setItemChecked( nId, false );
  } 
  */
}

void ClientView::showMenu() {
  bShowMenu = ! bShowMenu;

  if( bShowMenu == true )
    pMenuBar->show();
  else
    pMenuBar->hide();
}

void ClientView::refreshMenu() {  
  pSensor->refresh(); 
}

void ClientView::showAbout() {
  QMessageBox::about( this, "About Lan Screen Capture", "Lan Screen Capture Release 1.0 for Linux Red Hat\nProgram:\n\tHongLiang Li\n\tQian Zhu\n\tQi Xie\nSpecial thanks for Robert Luo\nBug Report: thenshesaid2003@yahoo.com.cn" );
}

QMenu* ClientView::getServerList() {
	return pServerList;
}
