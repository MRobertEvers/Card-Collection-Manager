#pragma once

// Fixed WS_EX_LAYOUTRTL not defined by updating the preprocessor definition; WINVER=0x0500

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
#include <wx/aui/framemanager.h>

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

// IDs for the controls and the menu commands. Taken from minimal sample.
enum
{
   Menu_Quit = wxID_EXIT,
   Menu_About = wxID_ABOUT
};

class MainFrame : public wxFrame
{
public:
   MainFrame(const wxString& title);

   void OnQuit(wxCommandEvent& event);
   void OnAbout(wxCommandEvent& event);

private:
   wxDECLARE_EVENT_TABLE();

   wxAuiManager m_UIMgr;
};
