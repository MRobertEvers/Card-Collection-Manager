#include "VCardView.h"

VCardView::VCardView( wxWindow* aptParent, wxWindowID aiWID )
   : wxPanel(aptParent, aiWID)
{

}

VCardView::~VCardView()
{

}

void 
VCardView::SetController( CCardView* apController )
{
   m_pController = apController;
}