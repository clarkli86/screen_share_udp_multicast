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
ClientView::ClientView(QWidget *parent, const char *name ) : QWidget(parent), client_(this), sensor_(this)  {
    setWindowTitle( "Lan Screen Capture" );
    //set the timer
    connect(&timer_, SIGNAL( timeout() ), this, SLOT( timeout() ) );
    timer_.start( 500 );

    menu_.reset(new QMenu( this ));

    //insert into the option
    menu_->addAction( "&Refresh server list", this, SLOT( refreshMenu() ) );
    menu_->addAction("&Fullscreen", this, SLOT(fullScreen()));

    serverList_.reset(new QMenu("Select a server", this));
    menu_->addMenu(serverList_.get());
    menu_->addAction( "&Quit", this, SLOT( quit() ) );
}

ClientView::~ClientView(){     
}

void ClientView::fullScreen()
{
    fullScreen_ = !fullScreen_;
    setWindowState(fullScreen_ ? Qt::WindowFullScreen : Qt::WindowNoState);
}

void ClientView::drawContents( QPainter * p, int clipx, int clipy, int clipw, int cliph ) {   
    if( true == fullScreen_ ) {
    //hide the scroll bar
    //this->resizeContents( 0, 0 );
    }
    else
    //if( contentsWidth() != map.width() || contentsHeight() != map.height() )
    //    resizes( map.width(), map.height() );

    p->drawPixmap( clipx, clipy, map_, clipx, clipy, clipw, cliph );
  
}        

void ClientView::contextMenuEvent(QContextMenuEvent * event)
{
    menu_->exec(event->globalPos());
}

void ClientView::redrawPixmap( const int width, const int height, const int top, const QPixmap& newMap ) {
  //resize the map
    if( map_.width() != width || map_.height() != height ) {
      //map.resize( width, height );
    }
  //bitBit the new Map to the specified to the map       
    QPainter painter(&map_);
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

void ClientView::refreshMenu() {  
  sensor_.refresh();
}

QMenu* ClientView::getServerList() {
    return serverList_.get();
}
