#pragma once
#include <wx/wxprec.h>

#include <wx/aui/framemanager.h>

class CMainWindow;

class VMainWindow : public wxFrame
{
public:
   VMainWindow(const wxString& title);
   ~VMainWindow();

   void SetController( CMainWindow* aptController );
   wxMenuBar* GetMenu();

   void OnQuit(wxCommandEvent& event);
   void OnAbout(wxCommandEvent& event);

   void OnViewCollection(wxCommandEvent& event);
   void OnViewCollectionOverview(wxCommandEvent& awxEvt);
   void OnImportSource(wxCommandEvent& awxEvt);
   void OnViewCollectionAs(wxCommandEvent& awxEvt);

   void SetView( wxPanel* aptEVTHandler );

private:
   wxDECLARE_EVENT_TABLE();

   wxMenuBar* m_wxMenuBar;
   CMainWindow* m_Controller;
   wxPanel* m_CurrentPanel;
   wxAuiManager m_mgr;

   void uiBuildDefaultMenus();
};

