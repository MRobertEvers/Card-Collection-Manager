#include "CCardInventoryViewer.h"
#include "VCardInventoryViewer.h"
#include "../StoreFrontEnd/StoreFrontEnd.h"

CCardInventoryViewer::CCardInventoryViewer( VCardInventoryViewer * apView )
   : m_pView(apView)
{
}

CCardInventoryViewer::~CCardInventoryViewer()
{
}

void 
CCardInventoryViewer::SetModel( CardInterface * apModel )
{
   if( apModel != nullptr )
   {
      m_pModel = apModel;
      m_pView->ViewCard( apModel );
   }
}

void
CCardInventoryViewer::OnSave( CardInterface* apModel, const wxString& aszNewVal )
{
   //auto ptSF = StoreFront::Ser
}

void 
CCardInventoryViewer::OnReset( CardInterface* apModel )
{
}

void 
CCardInventoryViewer::OnOpenEditor( CardInterface* apModel )
{
}
