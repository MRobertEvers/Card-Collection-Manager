#include "CCardView.h"

CCardView::CCardView( VCardView* apView )
   : m_pView(apView)
{
  
}

CCardView::~CCardView()
{

}

void 
CCardView::SetModel( CardInterface* apModel )
{
   m_pModel = apModel;
}