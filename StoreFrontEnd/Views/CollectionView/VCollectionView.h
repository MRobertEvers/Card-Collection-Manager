#pragma once
#include "..\Views\wxExtras\wxInfiniteGrid.h"
#include "..\StoreFrontEnd\Group.h"
#include "../ViewTemplates/IViewFactory.h"
#include <wx/wxprec.h>
#include <wx/object.h> 
#include <wx/grid.h>
#include <memory>
#include <map>

class VCardView;
class CardInterface;
class CCollectionView;

class GroupRenderer
{
public:
   // Returns true if the item was successfully removed.
   virtual void Draw( std::vector<CardInterface*> avecItemData ) = 0;

   virtual bool RemoveItem( CardInterface* aptItem ) = 0;
   virtual void AddItem( CardInterface* aptItem ) = 0;
};

// The renderer must be set before trying to show cardviewer etc.
class VCollectionView : public wxPanel
{
public:
   VCollectionView( wxFrame* aParent, wxWindowID aiWID );
   ~VCollectionView();

   void SetController( CCollectionView* aptController );

   // Takes ownership
   void SetRenderer( GroupRenderer* aptRenderer );

   void Draw( std::vector<CardInterface*> avecItemData );

   void ShowCardViewer( IViewFactory* aptViewer );
   void ShowCardInventoryViewer( IViewFactory* aptViewer );

private:
   wxDECLARE_EVENT_TABLE();

   CCollectionView* m_ptController;
   GroupRenderer* m_ptRenderer;
   wxPanel* m_ptSidePanel;

   void onCollectionEditorAccept( wxCommandEvent& awxEvt );
   void onItemClicked( wxGridEvent& awxEvt );

   void uiPrepareSidePanel();
};
