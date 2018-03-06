#pragma once
#include "wx/wxprec.h"

class viCardEditor;
class MainFrame;
class viCollectionEditor;

class ivCollectionView : public wxPanel
{
public:
   ivCollectionView( MainFrame* aptParent, wxWindowID aiWID, const wxString& aszColID );
   ~ivCollectionView();

   virtual void ShowCollectionEditor();
   virtual void CloseCollectionEditor();

protected:
   viCollectionEditor* m_viColEditor;
   viCardEditor* m_viCardEditor;
   wxString m_wxszColID;

   virtual void onEditorAccept( wxCommandEvent& awxEvt );
   virtual void onEditorDecline( wxCommandEvent& awxEvt );
   virtual void onCardChanged( wxCommandEvent& awxEvt );
   virtual void onNewItemSelectView( wxCommandEvent& awxEvt );
   virtual void onDeckEditor( wxCommandEvent& awxEvt );
   virtual void notifyCardEditor( const wxString& aszHash );
};

