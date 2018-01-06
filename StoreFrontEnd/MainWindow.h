#pragma once

// Fixed WS_EX_LAYOUTRTL not defined by updating the preprocessor definition; WINVER=0x0500

// When a wxWindow is destroyed, it automatically deletes 
// all its children.These children are all the objects that
// received the window as the parent - argument in their constructors.

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
#include "StoreFront.h"
#include <wx/aui/framemanager.h>

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

// IDs for the controls and the menu commands. Taken from minimal sample.
enum
{
   Menu_Quit = wxID_EXIT,
   Menu_About = wxID_ABOUT,
   Menu_Import = 0x4
};

class MainFrame : public wxFrame
{
public:
   MainFrame(const wxString& title);
   ~MainFrame();

   void OnQuit(wxCommandEvent& event);
   void OnAbout(wxCommandEvent& event);

   void OnViewCollection(wxCommandEvent& event);
   void OnImportSource(wxCommandEvent& awxEvt);

private:
   wxDECLARE_EVENT_TABLE();

   wxPanel* m_CurrentPanel;

   void buildMenus();

   void viewCollectionsOverview();
   void viewCollection(const wxString& aszColName);

   void setView(wxPanel* awxNewPanel);

   void appendMenuOptions(unsigned int aiMenuID, const wxString& aszName);
};
