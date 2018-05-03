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

   // The app needs to be made HIGH-DPI aware. Do this properties->manifest tool
   // Contrary to the wiki... WINVER=0x0500 must be added to pre-processor definitions.
   MainFrame *frame = new MainFrame( "StoreFrontPro" );

   return true;
}