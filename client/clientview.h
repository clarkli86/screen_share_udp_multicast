#ifndef CLIENTVIEW_H
#define CLIENTVIEW_H

#include <QWidget>
#include <QScrollArea>
#include <qpixmap.h>
/** This is the main widget of the client
  */
class QMenuBar;
class QMenu;
class Client;
class Sensor;
class QTimer;
class ClientView : public QScrollArea  {
   Q_OBJECT
public: 
	ClientView(QWidget *parent=0, const char *name=0);
	~ClientView();

  void redrawPixmap( const int width, const int height, const int top, const QPixmap& newMap );  
  void start();
  QMenu* getServerList();
	
protected:
  void contentsMouseDoubleClickEvent( QMouseEvent * e );
  void contentsMouseReleaseEvent( QMouseEvent * e );
  void drawContents( QPainter * p, int clipx, int clipy, int clipw, int cliph );

private slots:
	void timeout();
public slots:
 //menu slots
  void quit();
  void changeServer( int );
  void showMenu();
  void refreshMenu();
  void showAbout();
  
  signals:
  void quitSignal(); 
  
public:
  QPixmap map;
private:
  //if the widget is fullscreen
  bool bFullScreen;
  //if the menu is visible
  bool bShowMenu;
  QMenuBar * pMenuBar;
  QMenu * pServerList;
  QMenu * pMenu;
  Client * pClient;
  Sensor * pSensor;
  QTimer * pTimer;
};

#endif
