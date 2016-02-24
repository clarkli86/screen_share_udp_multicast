#include <qapplication.h>
#include <qfont.h>
#include <qstring.h>
#include <qtextcodec.h>
#include <qtranslator.h>
#include <qpushbutton.h>    
#include "clientview.h"

int main(int argc, char *argv[])
{      
  QApplication a(argc, argv);

  ClientView cv( NULL, "Lan Screen Capture" );

  QObject::connect( &cv, SIGNAL( quitSignal() ), &a, SLOT( quit() ) );

  cv.start();
  cv.resize( 400, 400 );
  a.setActiveWindow(&cv );
  cv.show();  
  
  return a.exec();
}


