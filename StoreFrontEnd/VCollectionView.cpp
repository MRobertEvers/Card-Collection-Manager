#include "VCollectionView.h"

VCollectionView::VCollectionView( wxPanel* aParent, wxWindowID aiWID )
   : wxPanel(aParent, aiWID)
{
   wxFlexGridSizer* boxSizer = new wxFlexGridSizer( 1, 3, 0, 0 );

   boxSizer->AddGrowableCol( 0 );
   boxSizer->AddGrowableRow( 0 );

   this->SetSizer( boxSizer );

   this->GetParent()->SetMinSize( wxSize( 550, 500 ) );
   this->GetParent()->Layout();
}

VCollectionView::~VCollectionView()
{
   delete m_ptRenderer;
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