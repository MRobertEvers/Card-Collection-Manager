#include "App.h"
#include "StoreFrontEnd.h"
#include "../Views/MainWindow/MainWindow.h"

// 'Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
   if( !wxApp::OnInit() )
   {
      return false;
   }

   StoreFrontEnd::Server();
   wxInitAllImageHandlers();

   MainFrame *frame = new MainFrame( "StoreFrontPro" );

   return true;
}