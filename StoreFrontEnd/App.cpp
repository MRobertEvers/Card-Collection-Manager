#include "App.h"
#include "MainWindow.h"

// 'Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit() {
   if( !wxApp::OnInit() )
      return false;

   MyFrame *frame = new MyFrame("StoreFrontPro");
   frame->Show(true);

   return true;
}