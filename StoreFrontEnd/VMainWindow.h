#pragma once
#include "IMenuEventSource.h"
#include <wx/wxprec.h>

class CMainWindow;
class ivCollectionView;

class VMainWindow : public IMenuEventSource
{
public:
   VMainWindow(const wxString& title);
   ~VMainWindow();

   void SetController( CMainWindow* aptController );

   void OnQuit(wxCommandEvent& event);
   void OnAbout(wxCommandEvent& event);

   void OnViewCollection(wxCommandEvent& event);
   void OnViewCollectionOverview(wxCommandEvent& awxEvt);
   void OnImportSource(wxCommandEvent& awxEvt);
   void OnViewCollectionAs(wxCommandEvent& awxEvt);

   void SetView( wxPanel* aptEVTHandler );

private:
   wxDECLARE_EVENT_TABLE();

   CMainWindow* m_Controller;
   wxPanel* m_CurrentPanel;

   void uiBuildDefaultMenus();
   void uiSetView();
};

