#include "clientview.h"
#include "client.h"
#include "sensor.h"
#include <QMessageBox>
#include <QEvent>
#include <QContextMenuEvent>
#include <QPainter>
#include <unistd.h>


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
    connect(serverList_.get(), SIGNAL(triggered(QAction*)), this, SLOT(changeServer(QAction*)));
    menu_->addMenu(serverList_.get());
    menu_->addAction( "&Quit", this, SLOT( quit() ) );

    if(!map_.load(":/images/logo.png")) {
        QMessageBox::warning( this, "Error", "Can't load logo");
        exit(-1);
    }
}

ClientView::~ClientView(){     
}

void ClientView::fullScreen()
{
    fullScreen_ = !fullScreen_;
    setWindowState(fullScreen_ ? Qt::WindowFullScreen : Qt::WindowNoState);
}

void ClientView::paintEvent(QPaintEvent * e ) {
    if((width() != map_.width() || height() != map_.height()) && map_.height() != 0 && map_.width() != 0) {
        resize( map_.width(), map_.height() );
    }

    QPainter painter(this);
    painter.drawPixmap(0, 0, map_);
}        

void ClientView::contextMenuEvent(QContextMenuEvent * event)
{
    menu_->exec(event->globalPos());
}

void ClientView::redrawPixmap( const int width, const int height, const int top, const QPixmap& newMap ) {
    //resize the map
    if((map_.width() != width || map_.height() != height) && height != 0 && width != 0) {
        map_ = map_.scaled(width, height);
    }

    //bitBit the new Map to the specified to the map
    QPainter painter(&map_);
    painter.drawPixmap(0, top, newMap, 0, 0, newMap.width(), newMap.height());
    update();
}

//it's called after every 0.5s
void ClientView::timeout() {
	//redraw the content
    update();
}

void ClientView::quit() {
  //tell the qapplication to quit
  emit quitSignal();
}

void ClientView::changeServer(QAction* action) {
    const auto address = action->text();
    qDebug() << "Change to " << address;
    // Get address on the server and connect to it
    if( ! client_.bind(address) ) {
        QMessageBox::warning( this, "Error", "Can't bind to the address " + address );
    }
}

void ClientView::refreshMenu() {  
  sensor_.refresh();
}

QMenu* ClientView::getServerList() {
    return serverList_.get();
}
