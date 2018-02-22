#pragma once
#include <wx/listctrl.h>
#include <wx/object.h> 
#include "wx/wxprec.h"

class vcCollectionDeckBoxItemList;
class viCardEditor;
class viCollectionEditor;
class MainFrame;

class vCollectionCube : public wxPanel
{
public:
   vCollectionCube( MainFrame* aptParent, wxWindowID aiWID, const wxString& aszColID );
   ~vCollectionCube();

   void ShowCollectionEditor();
   void CloseCollectionEditor();

private:
   wxDECLARE_EVENT_TABLE();

   vcCollectionDeckBoxItemList* m_vcItemList;
   viCardEditor* m_viCardEditor;
   viCollectionEditor* m_viColEditor;
   wxString m_wxszColID;

   void onEditorAccept( wxCommandEvent& awxEvt );
   void onEditorDecline( wxCommandEvent& awxEvt );
   void onCardChanged( wxCommandEvent& awxEvt );
   void onNewItemSelected( wxListEvent& awxEvt );
   void onDeckEditor( wxCommandEvent& awxEvt );
   void buildItemList();
   void notifyCardEditor( const wxString& aszHash, int aiIndex );
};

