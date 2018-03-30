#pragma once
#include <wx/wxprec.h>
#include <wx/statusbr.h>
#include <memory>

class viCardEditor;
class IMenuEventSource;
class viCollectionEditor;
class CollectionInterface;

class ivCollectionView : public wxPanel
{
public:
   ivCollectionView( IMenuEventSource* aptParent, wxWindowID aiWID, const wxString& aszColID );
   ~ivCollectionView();

   virtual void PrepareInterface() = 0;


   virtual void ShowCollectionEditor();
   virtual void CloseCollectionEditor();
   virtual void ShowStats();
   virtual void ShowHistory();

protected:
   viCollectionEditor* m_viColEditor;
   viCardEditor* m_viCardEditor;
   wxString m_wxszColID;
   std::shared_ptr<CollectionInterface> m_ptCollectionInterface;
   wxStatusBar* m_infoBar;

   void displayInfoBar();
   void refreshInfoBar();
   virtual void onEditorAccept( wxCommandEvent& awxEvt );
   virtual void onEditorDecline( wxCommandEvent& awxEvt );
   virtual void onCardChanged( wxCommandEvent& awxEvt );
   virtual void onNewItemSelectView( wxCommandEvent& awxEvt );
   virtual void onDeckEditor( wxCommandEvent& awxEvt );
   virtual void notifyCardEditor( const wxString& aszHash );
};

