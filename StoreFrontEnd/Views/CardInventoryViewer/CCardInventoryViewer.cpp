#include "CCardInventoryViewer.h"
#include "VCardInventoryViewer.h"

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
