#include "App.h"
#include "MainWindow.h"

// 'Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit() {
   if( !wxApp::OnInit() )
      return false;

   MainFrame *frame = new MainFrame("StoreFrontPro");
   frame->Show(true);

   return true;
}