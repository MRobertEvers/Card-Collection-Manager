#include "CCollectionView.h"
#include "VCollectionView.h"
#include "CubeRenderer.h"
#include "../Views/CardView/CardView.h"
#include "../Views/CardView/CCardView.h"
#include "../Views/CollectionEditor/viCollectionEditor.h"
#include "../Views/CollectionHistory/viHistoryViewer.h"
#include "../Views/CollectionStats/vStatsViewer.h"
#include "../Views/CardInventoryViewer/CardInventoryViewer.h"

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

   if( pFirst != nullptr )
   {
      m_ptCardViewer->GetController()->SetModel( pFirst );
   }
   m_ptView->Draw( vecItems );
}

void 
CCollectionView::ViewItem( CardInterface* apItem )
{
   auto pCardViewer = m_ptCardViewer->GetController();
   pCardViewer->SetModel( apItem );

   auto pCardInv = m_ptInventoryEditor->GetController();
   pCardInv->SetModel( apItem );
}

void 
CCollectionView::OnDoEdit()
{
   auto pEditor = new viCollectionEditor( m_ptView, wxID_ANY, m_ptModel->GetColId() );
   pEditor->Show();
}

void 
CCollectionView::OnViewStats()
{
   auto view = new vStatsViewer( m_ptView, wxID_ANY, m_ptModel );
   view->SetFocus();
   view->Show();
}

void 
CCollectionView::OnViewHistory()
{
   auto view = new viHistoryViewer( m_ptView, wxID_ANY, m_ptModel );
   view->Show();
}

void 
CCollectionView::OnExportXMage()
{
   Query listQuery( true );
   listQuery.IncludeMetaType( MetaTag::None );
   listQuery.SetIncludeMeta( false );
   listQuery.SetIncludeIDs( false );

   auto ptSF = StoreFrontEnd::Server();
   ptSF->ExportCollection( m_ptModel->GetColId(), listQuery );
}

void 
CCollectionView::OnSave()
{
   auto ptSF = StoreFrontEnd::Server();
   ptSF->SaveCollection( m_ptModel->GetColId() );
}

void 
CCollectionView::OnCollectionEdited()
{
   // Simply calculate differences
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

   m_ptView->Draw( vecItems );
   uiShowNewestCard( pFirst );
}

void 
CCollectionView::uiShowCardViewer()
{
   // Need to think of a better way to do submodules
   if( m_ptCardViewer == nullptr )
   {
      m_ptCardViewer = std::shared_ptr<CardView>( new CardView(  ) );
      m_ptView->ShowCardViewer( m_ptCardViewer.get() );
   }
   if( m_ptInventoryEditor == nullptr )
   {
      m_ptInventoryEditor = std::shared_ptr<CardInventoryViewer>( new CardInventoryViewer() );
      m_ptView->ShowCardInventoryViewer( m_ptInventoryEditor.get() );
   }
}

void CCollectionView::uiShowNewestCard( CardInterface * apItem )
{
   m_ptCardViewer->GetController()->SetModel( apItem );
   m_ptInventoryEditor->GetController()->SetModel( apItem );
}
