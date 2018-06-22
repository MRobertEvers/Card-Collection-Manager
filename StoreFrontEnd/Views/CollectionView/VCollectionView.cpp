#include "VCollectionView.h"
#include "CCollectionView.h"
#include "../Views/CardView/VCardView.h"
#include "../Views/CollectionEditor/viCollectionEditor.h"

wxBEGIN_EVENT_TABLE( VCollectionView, wxPanel )
//EVT_BUTTON( viCardEditor::Changes_Submit, vCollectionCube::onCardChanged )
//EVT_BUTTON( viCardEditor::Image_Changed, vCollectionCube::onNewItemSelectView )
EVT_GRID_CELL_LEFT_CLICK( VCollectionView::onItemClicked )
EVT_BUTTON( viCollectionEditor::Changes_Accept, VCollectionView::onCollectionEditorAccept )
wxEND_EVENT_TABLE()

VCollectionView::VCollectionView( wxFrame* aParent, wxWindowID aiWID )
   : wxPanel(aParent, aiWID)
{
   wxFlexGridSizer* boxSizer = new wxFlexGridSizer( 1, 3, 0, 0 );

   boxSizer->AddGrowableCol( 0 );
   boxSizer->AddGrowableRow( 0 );

   this->SetSizer( boxSizer );

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
      GetSizer()->Add( panel, wxSizerFlags( 1 ).Expand() );

      uiPrepareSidePanel();

      this->Layout();
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
   m_ptSidePanel->GetSizer()->Add( aptViewer->GetView( m_ptSidePanel ), wxSizerFlags( 0 ).Align(wxTOP | wxCenter) );
   this->Layout();
}

void 
VCollectionView::ShowCardInventoryViewer( IViewFactory* aptViewer )
{
   m_ptSidePanel->GetSizer()->Add( aptViewer->GetView(m_ptSidePanel), wxSizerFlags( 1 ).Expand() );
   this->Layout();
}


void 
VCollectionView::onCollectionEditorAccept( wxCommandEvent& awxEvt )
{
   m_ptController->OnCollectionEdited();
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

void 
VCollectionView::uiPrepareSidePanel()
{
   m_ptSidePanel = new wxPanel( this, wxID_ANY );
   wxFlexGridSizer* sideSizer = new wxFlexGridSizer( 2, 1, 0, 0 );
   sideSizer->AddGrowableCol( 0 );
   sideSizer->AddGrowableRow( 1 );
   m_ptSidePanel->SetSizer( sideSizer );
   this->GetSizer()->Add( m_ptSidePanel, wxSizerFlags( 1 ).Border( wxALL, 3 ).Expand() );
}
