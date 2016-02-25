#include <QApplication>
#include <QScrollArea>
#include "clientview.h"

int main(int argc, char *argv[])
{      
  QApplication a(argc, argv);

  ClientView cv( NULL, "Lan Screen Capture" );

  QObject::connect( &cv, SIGNAL( quitSignal() ), &a, SLOT( quit() ) );

  QScrollArea scroll;
  scroll.setWidget(&cv);

  cv.resize( 400, 400 );
  a.setActiveWindow(&cv );
  scroll.show();
  
  return a.exec();
}


