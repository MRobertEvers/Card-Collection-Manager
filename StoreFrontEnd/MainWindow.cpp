#include "MainWindow.h"
#include "StorefrontConfig.h"
#include "CollectionsOverview.h"


// Events can be tied at run-time. This is compile time.
wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
EVT_MENU(Minimal_Quit, MyFrame::OnQuit)
EVT_MENU(Minimal_About, MyFrame::OnAbout)
wxEND_EVENT_TABLE()


MyFrame::MyFrame(const wxString& title)
   : wxFrame(NULL, sfMAIN_WINDOW, title) {
   // set the frame icon
   SetIcon(wxICON(sample));

   // create a menu bar
   wxMenu *fileMenu = new wxMenu;
   fileMenu->Append(Minimal_Quit, "E&xit\tAlt-X", "Quit this program");

   // the "About" item should be in the help menu
   wxMenu *helpMenu = new wxMenu;
   helpMenu->Append(Minimal_About, "&About\tF1", "Show about dialog");

   // now append the freshly created menu to the menu bar...
   wxMenuBar *menuBar = new wxMenuBar();
   menuBar->Append(fileMenu, "&File");
   menuBar->Append(helpMenu, "&Help");

   // ... and attach this menu bar to the frame
   SetMenuBar(menuBar);

   // create a status bar just for fun (by default with 1 pane only)
   CreateStatusBar(2);
   SetStatusText("StoreFrontPro!");

   CollectionsOverview* ptColO = new CollectionsOverview(this, 8);
   wxBoxSizer* boxSizer = new wxBoxSizer(wxVERTICAL);
   boxSizer->Add(ptColO, 1, wxEXPAND);
}

void 
MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event)) {
   // true is to force the frame to close
   Close(true);
}

void 
MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event)) {
   wxMessageBox(wxString::Format
   (
      "Welcome to %s!\n"
      "\n"
      "This is the minimal wxWidgets sample\n"
      "running under %s.",
      wxVERSION_STRING,
      wxGetOsDescription()
   ),
      "About wxWidgets minimal sample",
      wxOK | wxICON_INFORMATION,
      this);
}
