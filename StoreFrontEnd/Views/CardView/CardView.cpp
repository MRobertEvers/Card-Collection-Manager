#include "CardView.h"
#include "VCardView.h"
#include "CCardView.h"

CardView::CardView( )
{
   
}

CardView::~CardView()
{
}

wxPanel* 
CardView::GetView(wxPanel* apParent)
{
   m_pView = new VCardView( apParent, wxID_ANY );

   m_pController = new CCardView( m_pView );
   m_pView->SetController( m_pController );

   return m_pView;
}

CCardView* 
CardView::GetController()
{
   return m_pController;
}