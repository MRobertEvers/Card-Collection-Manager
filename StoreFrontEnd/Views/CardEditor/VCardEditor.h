#pragma once
#include "../ViewTemplates/IViewFactory.h"
#include <list>
#include <wx/wxprec.h>
#include <wx/aui/framemanager.h>
#include <wx/grid.h>

class CardInterface;
class CCardEditor;
class VCardEditor : public wxFrame
{
public:
   enum
   {
      NEXT_BUTTON = 0x88,
      BACK_BUTTON = 0x89,
      OK_BUTTON = 0x90,
      CANCEL_BUTTON = 0x91
   };

public:
   VCardEditor( wxWindow* aptParent, wxWindowID aiWID );
   ~VCardEditor();

   void SetController( CCardEditor* apController );

   void SetCardTitle( const wxString& aszTitle );
   void ShowCardViewer( IViewFactory* aptViewer );
   void ShowAttributesTable( IViewFactory* aptViewer );

private:
   wxDECLARE_EVENT_TABLE();

   CCardEditor* m_pController;
   wxAuiManager m_mgr;

   wxTextCtrl* m_ptxtCtrl;

   void onItemSelected( wxGridEvent& awxEvt );
};

class CardEditorControlPanel : public wxPanel
{
public:
   CardEditorControlPanel();
   ~CardEditorControlPanel();

private:
   wxAuiManager m_mgr;
   
   wxString m_szViewingHash;
   std::list<wxString> m_vecSelectedUIDs;
};