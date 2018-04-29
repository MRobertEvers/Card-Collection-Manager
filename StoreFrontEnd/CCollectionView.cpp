#include "CCollectionView.h"
#include "VCollectionView.h"
#include "CubeRenderer.h"

CCollectionView::CCollectionView( VCollectionView* aptView,
                                  std::shared_ptr<CollectionInterface> aptModel )
   : m_ptModel(aptModel), m_ptView(aptView)
{

}

CCollectionView::~CCollectionView()
{

}

void 
CCollectionView::SetCubeRenderer()
{
   std::vector<CardInterface*> vecItems;
   for( auto& item : m_ptModel->GetItemInterfaces() )
   {
      vecItems.push_back( &item );
   }
   m_ptView->SetRenderer( new CubeRenderer( m_ptView, wxID_ANY ) );
   m_ptView->Draw( vecItems );
}
