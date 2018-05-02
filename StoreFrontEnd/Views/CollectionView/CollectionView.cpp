#include "CollectionView.h"
#include "VCollectionView.h"
#include "CCollectionView.h"

CollectionView::CollectionView( wxFrame* aptParent, std::shared_ptr<CollectionInterface> aptModel )
{
   m_pView = new VCollectionView( aptParent, wxID_ANY );

   m_pController = new CCollectionView( m_pView, aptModel );
   m_pView->SetController( m_pController );
   m_pController->SetCubeRenderer();
}

CollectionView::~CollectionView()
{
   delete m_pController;
}

wxPanel* 
CollectionView::GetView()
{
   return m_pView;
}