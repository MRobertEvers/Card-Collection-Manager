#pragma once

// Fixed WS_EX_LAYOUTRTL not defined by updating the preprocessor definition; WINVER=0x0500

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
#include <wx/aui/framemanager.h>

// Include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

// IDs for the controls and the menu commands. Taken from minimal sample.
enum
{
   // menu items
   Minimal_Quit = wxID_EXIT,

   // it is important for the id corresponding to the "About" command to have
   // this standard value as otherwise it won't be handled properly under Mac
   // (where it is special and put into the "Apple" menu)
   Minimal_About = wxID_ABOUT
};

//#include "App.h"

// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
public:
   // ctor(s)
   MyFrame(const wxString& title);

   // event handlers (these functions should _not_ be virtual)
   void OnQuit(wxCommandEvent& event);
   void OnAbout(wxCommandEvent& event);

private:
   // any class wishing to process wxWidgets events must use this macro
   wxDECLARE_EVENT_TABLE();

   wxAuiManager m_UIMgr;
};
