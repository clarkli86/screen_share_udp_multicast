#ifndef CLIENTVIEW_H
#define CLIENTVIEW_H

#include <QWidget>
#include <QPixmap>
#include <QMenuBar>
#include <QMenu>
#include <QTimer>
#include <memory>
#include <client.h>
#include <sensor.h>

/** This is the main widget of the client
  */
class ClientView : public QWidget  {
   Q_OBJECT
public: 
	ClientView(QWidget *parent=0, const char *name=0);
	~ClientView();

  void redrawPixmap( const int width, const int height, const int top, const QPixmap& newMap );  
  void start();
  QMenu* getServerList();
	
protected:
  void contextMenuEvent(QContextMenuEvent * event) Q_DECL_OVERRIDE;
  void contentsMouseDoubleClickEvent( QMouseEvent * e );
  void contentsMouseReleaseEvent( QMouseEvent * e );
  void paintEvent(QPaintEvent * e);

private slots:
	void timeout();

public slots:
 //menu slots
  void quit();
  void changeServer(QAction*);
  void refreshMenu();
  void fullScreen();
  
signals:
  void quitSignal(); 
  
public:
  QPixmap map_;
private:
  //if the widget is fullscreen
  bool fullScreen_ = false;

  std::unique_ptr<QMenu> serverList_;
  std::unique_ptr<QMenu> servers_;
  std::unique_ptr<QMenu> menu_;
  Client client_;
  Sensor sensor_;
  QTimer timer_;
};

#endif
