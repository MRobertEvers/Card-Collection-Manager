#include "VCardView.h"

VCardView::VCardView( wxWindow* aptParent, wxWindowID aiWID )
   : wxPanel(aptParent, aiWID)
{
   wxFlexGridSizer* boxSizer = new wxFlexGridSizer( 4, 1, 0, 0 );//(wxVERTICAL);//(2,1,0,0);
   boxSizer->SetFlexibleDirection( wxVERTICAL );
   boxSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_NONE );
   // boxSizer->AddGrowableRow(1);
   boxSizer->AddGrowableRow( 2 );

   this->SetSizer( boxSizer );
   this->SetSize( 250, 500 );
}

VCardView::~VCardView()
{

}

void 
VCardView::SetController( CCardView* apController )
{
   m_pController = apController;
}