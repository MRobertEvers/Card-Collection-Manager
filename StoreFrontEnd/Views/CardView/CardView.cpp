#include "CardView.h"
#include "VCardView.h"
#include "CCardView.h"

CardView::CardView( wxFrame* aptParent, CardInterface* apModel )
{
   m_pView = new VCardView( aptParent, wxID_ANY );

   m_pController = new CCardView( m_pView, apModel );
   m_pView->SetController( m_pController );
}

CardView::~CardView()
{
   // View is deleted by wxWidgets
   delete m_pController;
}

wxPanel* 
CardView::GetView()
{
   return m_pView;
}