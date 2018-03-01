#pragma once
#include "wx/wxprec.h"
#include <wx/object.h> 
#include <wx/grid.h>

class vcCollectionDeckBoxItemList;
class viCardEditor;
class viCollectionEditor;
class vcCollectionCubeDisplay;
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

   vcCollectionCubeDisplay* m_vcItemList;
   viCardEditor* m_viCardEditor;
   viCollectionEditor* m_viColEditor;
   wxString m_wxszColID;

   void onEditorAccept( wxCommandEvent& awxEvt );
   void onEditorDecline( wxCommandEvent& awxEvt );
   void onCardChanged( wxCommandEvent& awxEvt );
   void onNewItemSelectView( wxCommandEvent& awxEvt );
   void onNewItemSelected( wxGridEvent& awxEvt );
   void onDeckEditor( wxCommandEvent& awxEvt );
   void buildGroupPanel();
   void notifyCardEditor( const wxString& aszHash );
};

