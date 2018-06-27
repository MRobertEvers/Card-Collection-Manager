#include "VCollectionView.h"
#include "CCollectionView.h"
#include "../Views/CardView/VCardView.h"
#include "../StoreFrontEnd/CollectionInterface.h"
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
VCollectionView::SetRenderer( GroupRenderer* aptRenderer )
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
      m_ptRenderer->Draw( avecItemData );
   }
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
   //auto delta = dynamic_cast<CollectionDelta*>((CollectionDelta*)awxEvt.GetClientData());
   //if( delta != nullptr )
   //{
   //   auto resolve = m_ptController->Resolve( delta );
   //   for( auto& rem : resolve.GetRemoved() )
   //   {
   //      m_ptRenderer->RemoveItem( rem );
   //   }
   //}
   //else
   {
      m_ptController->OnCollectionEdited();
   }
}


void 
VCollectionView::onItemClicked( wxGridEvent& awxEvt )
{
   auto pItem = dynamic_cast<CardInterface*>((CardInterface*)awxEvt.GetClientData());
   if( pItem != nullptr )
   {
      m_ptController->ViewItem( pItem );
   }
   awxEvt.Skip();
}
