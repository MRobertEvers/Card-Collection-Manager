#pragma once

// Fixed WS_EX_LAYOUTRTL not defined by updating the preprocessor definition; WINVER=0x0500

// When a wxWindow is destroyed, it automatically deletes 
// all its children.These children are all the objects that
// received the window as the parent - argument in their constructors.

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
#include <wx/aui/framemanager.h>
#include <vector>

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

class wxMenu;
class IMenuEventHandler;

// IDs for the controls and the menu commands. Taken from minimal sample.


class MainFrame : public wxFrame
{
public:
   enum
   {
      Menu_Quit = wxID_EXIT,
      Menu_About = wxID_ABOUT,
      Menu_Main = 0x71,
      Menu_Import = 0x4,
      Menu_View_As = 0x4500,
      Menu_View_As_Deck = 0x4503,
      Menu_View_As_Cube = 0x4502
   };

   MainFrame(const wxString& title);
   ~MainFrame();

   void RegisterMenu(wxMenu* fileMenu, wxString aszTitle);

   void OnQuit(wxCommandEvent& event);
   void OnAbout(wxCommandEvent& event);

   void OnViewCollection(wxCommandEvent& event);
   void OnViewCollectionOverview(wxCommandEvent& awxEvt);
   void OnImportSource(wxCommandEvent& awxEvt);
   void OnGenericMenuEvent(wxCommandEvent& awxEvt);
   void OnViewCollectionAs( wxCommandEvent& awxEvt );

   void RegisterSendMenuEvents();
   void ReleaseMenuEventHandler();
   void BindMenuEventHandler();

private:
   wxDECLARE_EVENT_TABLE();

   IMenuEventHandler* m_CurrentPanel;
   wxMenuBar* m_wxMenuBar;
   std::vector<wxMenu*> m_vecViewMenus;
   bool m_bEvtHandlerView;
   bool m_bEvtHandlerViewFlag;

   void buildMenus();

   void viewCollectionsOverview();
   void viewCollection(const wxString& aszColName);

   void setView(IMenuEventHandler* awxNewPanel);

   void appendMenuOptions(unsigned int aiMenuID, const wxString& aszName);
};
