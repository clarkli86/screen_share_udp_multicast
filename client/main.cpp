#include <QApplication>
#include <QScrollArea>
#include "clientview.h"

namespace {
constexpr int INITIAL_WIDTH = 400;
constexpr int INITIAL_HEIGHT = 400;
}
int main(int argc, char *argv[])
{      
  QApplication a(argc, argv);

  ClientView cv( NULL, "Lan Screen Capture" );

  QObject::connect( &cv, SIGNAL( quitSignal() ), &a, SLOT( quit() ) );

  QScrollArea scroll;
  scroll.setWidget(&cv);

  cv.resize( INITIAL_WIDTH, INITIAL_HEIGHT );
  a.setActiveWindow(&scroll );
  scroll.show();
  
  return a.exec();
}


