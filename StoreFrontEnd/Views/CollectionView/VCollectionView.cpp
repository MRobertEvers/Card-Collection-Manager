#include "VCollectionView.h"
#include "CCollectionView.h"
#include "CubeRenderer.h"
#include "../Views/CardView/VCardView.h"
#include "../Views/CollectionEditor/viCollectionEditor.h"

wxBEGIN_EVENT_TABLE( VCollectionView, wxPanel )
//EVT_BUTTON( viCardEditor::Changes_Submit, vCollectionCube::onCardChanged )
//EVT_BUTTON( viCardEditor::Image_Changed, vCollectionCube::onNewItemSelectView )
EVT_GRID_CELL_LEFT_CLICK( VCollectionView::onItemClicked )
EVT_BUTTON( VCollectionView::COLLECTION_EDITED, VCollectionView::onCollectionEditorAccept )
EVT_BUTTON( viCollectionEditor::Changes_Accept, VCollectionView::onCollectionEditorAccept )
wxEND_EVENT_TABLE()

VCollectionView::VCollectionView( wxFrame* aParent, wxWindowID aiWID )
   : wxPanel(aParent, aiWID)
{
   m_mgr.SetManagedWindow( this );
   this->GetParent()->SetMinSize( wxSize( 550, 500 ) );

   auto pTargetFrame = this->GetParent();
   if( pTargetFrame->GetSize().GetWidth() < pTargetFrame->GetMinSize().GetWidth() ||
       pTargetFrame->GetSize().GetHeight() < pTargetFrame->GetMinSize().GetHeight() )
   {
      pTargetFrame->SetSize( pTargetFrame->GetMinSize() );
   }

   this->GetParent()->Layout();
}

VCollectionView::~VCollectionView()
{
   // Don't need to destroy this because wxwidgets will.
   //m_ptRenderer->Destroy();
   m_mgr.UnInit();
}

void 
VCollectionView::SetController( CCollectionView* aptController )
{
   m_ptController = aptController;
}

// Takes ownership
void 
VCollectionView::SetRenderer( FlexibleGroupRenderer* aptRenderer )
{
   m_ptRenderer = aptRenderer;

   // TODO: this is ugly;
   auto panel = dynamic_cast<wxPanel*>(aptRenderer);
   if( panel != nullptr )
   {
      m_mgr.AddPane( panel, 
         wxAuiPaneInfo().CenterPane() );
      m_mgr.Update();
      //GetSizer()->Add( panel, wxSizerFlags( 1 ).Expand() );

      //uiPrepareSidePanel();
   }
}

void 
VCollectionView::Draw( std::vector<CardInterface*> avecItemData )
{
   if( m_ptRenderer != nullptr )
   {
      std::vector<std::shared_ptr<IRendererItem>> vecItems;
      for( auto& ptr : avecItemData )
      {
         vecItems.push_back( std::shared_ptr<IRendererItem>(new RendererItem(ptr) ) );
      }
      m_ptRenderer->Draw( vecItems );
   }
}

void
VCollectionView::Undraw( const wxString & aszDisplay, const wxString & aszUID )
{
   auto iter_item = m_ptRenderer->LookupItem( aszDisplay.ToStdString(), aszUID.ToStdString() );
   if( iter_item != nullptr )
   {
      m_ptRenderer->RemoveItem( iter_item );
   }
}

void
VCollectionView::Draw( CardInterface* apNew )
{
   for( auto& uid : apNew->GetRepresentingUIDs() )
   {
      Undraw( apNew->GetName(), uid );
   }
   m_ptRenderer->AddItem( std::shared_ptr<IRendererItem>( new RendererItem( apNew ) ) );
}

void 
VCollectionView::ShowCardViewer( IViewFactory* aptViewer )
{
   auto view = aptViewer->GetView( this );
   m_mgr.AddPane( view,
      wxAuiPaneInfo().Right().CaptionVisible( false ).CloseButton( false ).Floatable(false).Fixed().BestSize( view->GetBestSize()) );
   m_mgr.Update();
   //m_ptSidePanel->GetSizer()->Add( aptViewer->GetView( m_ptSidePanel ), wxSizerFlags( 0 ).Align(wxTOP | wxCenter) );
   //this->Layout();
}

void 
VCollectionView::ShowCardInventoryViewer( IViewFactory* aptViewer )
{
   auto view = aptViewer->GetView( this );
   m_mgr.AddPane( view,
      wxAuiPaneInfo().Right().CaptionVisible(false).CloseButton(false).Floatable(false).BestSize( view->GetBestSize() ) );
   m_mgr.Update();
   //m_ptSidePanel->GetSizer()->Add( aptViewer->GetView(m_ptSidePanel), wxSizerFlags( 1 ).Expand() );
   //this->Layout();
}


void 
VCollectionView::onCollectionEditorAccept( wxCommandEvent& awxEvt )
{
   // m_ptController->OnCollectionEdited();
   CollectionDelta* delta = dynamic_cast<CollectionDelta*>((CollectionDelta*)awxEvt.GetClientData());
   if( delta == nullptr )
   {
      m_ptController->OnCollectionEdited();
   }
   else
   {
      auto myDelta = std::shared_ptr<CollectionDelta>( delta );
      // Take ownership of the data.
      m_ptController->OnCollectionEdited( myDelta );
   }
}


void 
VCollectionView::onItemClicked( wxGridEvent& awxEvt )
{
   auto pItem = dynamic_cast<RendererItem*>((RendererItem*)awxEvt.GetClientData());
   if( pItem != nullptr )
   {
      m_ptController->ViewItem( pItem->GetBase() );
   }
   awxEvt.Skip();
}

void
FlexibleGroupRenderer::InitRenderer( std::vector<std::shared_ptr<IRendererItem>> avecItemData )
{
   for( auto& item : avecItemData )
   {
      m_mapLookup.insert( std::make_pair( item->GetName(), item ) );
   }
}

std::shared_ptr<IRendererItem> 
FlexibleGroupRenderer::LookupItem( const std::string & aszDisplay, const std::string & aszUID )
{
   auto pair_item_range = m_mapLookup.equal_range( aszDisplay );
   for( auto iter_named = pair_item_range.first;
        iter_named != pair_item_range.second;
        iter_named++ )
   {
      if( iter_named->second->RepresentsUID( aszUID ) )
      {
         return iter_named->second;
      }
   }

   return std::shared_ptr<IRendererItem>();
}

void
FlexibleGroupRenderer::ItemRemoved( std::shared_ptr<IRendererItem> apAdded )
{
   auto pair_item_range = m_mapLookup.begin();
   for( ;
      pair_item_range != m_mapLookup.end();
      pair_item_range++ )
   {
      if( pair_item_range->second == apAdded )
      {
         m_mapLookup.erase( pair_item_range );
         break;
      }
   }
}

void
FlexibleGroupRenderer::ItemAdded( std::shared_ptr<IRendererItem> apAdded )
{
   m_mapLookup.insert( std::make_pair( apAdded->GetName(), apAdded ) );
}
