#include "CCardInventoryViewer.h"
#include "VCardInventoryViewer.h"
#include "../StoreFrontEnd/StoreFrontEnd.h"
#include "../StoreFrontEnd/CardInterface.h"
#include "../Views//CollectionView/VCollectionView.h"

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
CCardInventoryViewer::OnSave( const CardInterface& aIface, const wxString& aszNewVal )
{
   vector<string> vecChanges;
   for( auto& uid : aIface.GetRepresentingUIDs() )
   {
      auto szDelta = aIface.SetAttribute( "set", aszNewVal.ToStdString(), uid );
      vecChanges.push_back( szDelta );
   }

   // Send event with new CollectionDelta
   wxCommandEvent updateEvt( wxEVT_BUTTON );
   updateEvt.SetId( VCollectionView::COLLECTION_EDITED );

   // Data used in the callback.

   ::wxPostEvent( m_pView->GetParent(), updateEvt );
}

void 
CCardInventoryViewer::OnReset( CardInterface* apModel )
{
}

void 
CCardInventoryViewer::OnOpenEditor( CardInterface* apModel )
{
}
