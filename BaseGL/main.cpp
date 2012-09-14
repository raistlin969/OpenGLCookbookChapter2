#include "GLWidgetText.h"
#include <QtGui/QApplication>

#include <Windows.h>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <boost/log/trivial.hpp>
#include "Logger.h"


void createConsole()
{
    AllocConsole();
    SetConsoleTitle(L"Debug console");

    int hConHandle;
    long lStdHandle;

    FILE *fp;   // redirect unbuffered STDOUT to the console
    lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
    hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
    fp = _fdopen( hConHandle, "w" );
    *stdout = *fp;
    setvbuf( stdout, NULL, _IONBF, 0 ); 

    // redirect unbuffered STDIN to the console
    lStdHandle = (long)GetStdHandle(STD_INPUT_HANDLE);
    hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
    fp = _fdopen( hConHandle, "r" );
    *stdin = *fp;
    setvbuf( stdin, NULL, _IONBF, 0 );  

    // redirect unbuffered STDERR to the console
    lStdHandle = (long)GetStdHandle(STD_ERROR_HANDLE);
    hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
    fp = _fdopen( hConHandle, "w" );
    *stderr = *fp;
    setvbuf( stderr, NULL, _IONBF, 0 );
}

int main(int argc, char *argv[])
{
  createConsole();
  boost::log::sources::severity_logger<severity_level>& lg = get_global_log();  
  BOOST_LOG_SEV(lg, normal) << "Console Initiated";
  QApplication a(argc, argv);
  QGLFormat glFormat;
  glFormat.setVersion( 4, 0 );
  glFormat.setProfile( QGLFormat::CoreProfile ); // Requires >=Qt-4.8.0
  glFormat.setSampleBuffers( true );
  glFormat.setDoubleBuffer(true);
  glFormat.setRgba(true);
  glFormat.setAlpha(true);
  glFormat.setDepth(true);
 
    // Create a GLWidget requesting our format
  GLWidgetText w( glFormat );    
  w.setMinimumWidth(800);
  w.setMinimumHeight(600);
  w.show();
  return a.exec();
}
