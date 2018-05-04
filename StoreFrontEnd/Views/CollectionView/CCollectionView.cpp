#include "CCollectionView.h"
#include "VCollectionView.h"
#include "CubeRenderer.h"
#include "../Views/CardView/CardView.h"
#include "../Views/CardView/CCardView.h"

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
   // Below is for CUBEVIEW
   Query query;
   query.AnyMeta();
   m_ptModel->PrepareInterface( query );
   m_ptModel->Refresh();

   std::vector<CardInterface*> vecItems;
   CardInterface* pFirst = nullptr;
   for( auto& item : m_ptModel->GetItemInterfaces() )
   {
      if( pFirst == nullptr )
      {
         pFirst = &item;
      }
      vecItems.push_back( &item );
   }
   
   m_ptView->SetRenderer( new CubeRenderer( m_ptView, wxID_ANY ) );
   uiShowCardViewer();

   m_ptCardViewer->GetController()->SetModel( pFirst );
   m_ptView->Draw( vecItems );
}

void 
CCollectionView::uiShowCardViewer()
{
   if( m_ptCardViewer == nullptr )
   {
      m_ptCardViewer = std::shared_ptr<CardView>( new CardView( m_ptView ) );
      m_ptView->ShowCardViewer( m_ptCardViewer->GetView() );
   }
}
