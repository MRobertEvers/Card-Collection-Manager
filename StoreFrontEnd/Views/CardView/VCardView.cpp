#include "VCardView.h"
#include "CardRenderer.h"

VCardView::VCardView( wxWindow* aptParent, wxWindowID aiWID )
   : wxPanel(aptParent, aiWID)
{
   wxBoxSizer* boxSizer = new wxBoxSizer( wxVERTICAL );//( 4, 1, 0, 0 );//(wxVERTICAL);//(2,1,0,0);
   // boxSizer->SetFlexibleDirection( wxVERTICAL );
   // boxSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_NONE );
   // boxSizer->AddGrowableRow(1);
   // boxSizer->AddGrowableRow( 2 );

   this->SetSizer( boxSizer );
   this->SetSizeHints( 250, 500 );

   m_pRenderer = new CardRenderer( this );
   boxSizer->Add( m_pRenderer, wxSizerFlags( 1 ).Expand() );
   this->Layout();
}

VCardView::~VCardView()
{
   delete m_pController;
}

void 
VCardView::SetController( CCardView* apController )
{
   m_pController = apController;
}

void 
VCardView::ViewCard( CardInterface* apInterface )
{
   m_pRenderer->DisplayImage( apInterface );
}