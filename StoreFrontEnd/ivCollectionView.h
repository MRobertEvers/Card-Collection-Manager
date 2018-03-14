#pragma once
#include "wx/wxprec.h"
#include <memory>

class viCardEditor;
class MainFrame;
class viCollectionEditor;
class CollectionInterface;

class ivCollectionView : public wxPanel
{
public:
   ivCollectionView( MainFrame* aptParent, wxWindowID aiWID, const wxString& aszColID );
   ~ivCollectionView();

   virtual void ShowCollectionEditor();
   virtual void CloseCollectionEditor();
   virtual void PrepareInterface(  ) = 0;

protected:
   viCollectionEditor* m_viColEditor;
   viCardEditor* m_viCardEditor;
   wxString m_wxszColID;
   std::shared_ptr<CollectionInterface> m_ptCollectionInterface;

   virtual void onEditorAccept( wxCommandEvent& awxEvt );
   virtual void onEditorDecline( wxCommandEvent& awxEvt );
   virtual void onCardChanged( wxCommandEvent& awxEvt );
   virtual void onNewItemSelectView( wxCommandEvent& awxEvt );
   virtual void onDeckEditor( wxCommandEvent& awxEvt );
   virtual void notifyCardEditor( const wxString& aszHash );
};

