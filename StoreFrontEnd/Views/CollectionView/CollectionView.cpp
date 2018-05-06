#include "CollectionView.h"
#include "VCollectionView.h"
#include "CCollectionView.h"
#include "../Views/ViewTemplates/IMenuEventSource.h"

CollectionView::CollectionView( wxFrame* aptParent, IMenuEventSource* apSource, std::shared_ptr<CollectionInterface> aptModel )
{
   m_pView = new VCollectionView( aptParent, wxID_ANY );

   m_pController = new CCollectionView( m_pView, apSource, aptModel );
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

CCollectionView* 
CollectionView::GetController()
{
   return m_pController;
}