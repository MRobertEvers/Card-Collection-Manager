#pragma once
#include "..\Views\wxExtras\wxInfiniteGrid.h"
#include "..\StoreFrontEnd\Group.h"
#include "../ViewTemplates/IViewFactory.h"
#include <wx/wxprec.h>
#include <wx/object.h> 
#include <wx/grid.h>
#include <wx/aui/framemanager.h>
#include <memory>
#include <map>

class VCardView;
class CardInterface;
class CCollectionView;

class IRendererItem : public IGroupItem
{
public:
   virtual std::string GetHash() const = 0;

   // Supports multiple UIDs per hash or name.
   virtual bool RepresentsUID( const std::string& aszKey ) const = 0 ;

   // IGroupItem
   virtual std::string GetName() const = 0;
   virtual std::string GetMetaTag( const std::string& aszKey ) const = 0;
   virtual std::string GetAttribute( const std::string& aszKey ) const = 0;
};

class GroupRenderer
{
public:
   // Returns true if the item was successfully removed.
   virtual void Draw( std::vector<std::shared_ptr<IRendererItem>> avecItemData ) = 0;

   virtual bool RemoveItem( std::shared_ptr<IRendererItem> aptItem ) = 0;
   virtual void AddItem( std::shared_ptr<IRendererItem> aptItem ) = 0;
};

class FlexibleGroupRenderer : public GroupRenderer
{
public:
   // Default Item lookup provided here.
   virtual void InitRenderer( std::vector<std::shared_ptr<IRendererItem>> avecItemData );
   virtual std::shared_ptr<IRendererItem> LookupItem( const std::string& aszDisplay, const std::string& aszUID );

private:
   std::multimap<std::string, std::shared_ptr<IRendererItem>> m_mapLookup;
};

// The renderer must be set before trying to show cardviewer etc.
class VCollectionView : public wxPanel
{
public:
   enum 
   {
      COLLECTION_EDITED = 3044
   };

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
   wxAuiManager m_mgr;

   void onCollectionEditorAccept( wxCommandEvent& awxEvt );
   void onItemClicked( wxGridEvent& awxEvt );

};
