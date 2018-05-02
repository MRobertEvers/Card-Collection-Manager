#include "CCardView.h"

CCardView::CCardView( VCardView* apView, CardInterface* apModel )
   : m_pView(apView), m_pModel(apModel)
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