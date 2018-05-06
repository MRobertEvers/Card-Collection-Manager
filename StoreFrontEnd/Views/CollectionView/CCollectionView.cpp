#include "CCollectionView.h"
#include "VCollectionView.h"
#include "CubeRenderer.h"
#include "../Views/CardView/CardView.h"
#include "../Views/CardView/CCardView.h"
#include "../Views/CollectionEditor/viCollectionEditor.h"
#include "../Views/CollectionHistory/viHistoryViewer.h"
#include "../Views/CollectionStats/vStatsViewer.h"

CCollectionView::CCollectionView( VCollectionView* aptView,
                                  IMenuEventSource* apSource,
                                  std::shared_ptr<CollectionInterface> aptModel )
   : IMenuEventHandler( apSource ), m_ptModel(aptModel), m_ptView(aptView)
{
   registerSendMenuEvents();
}

CCollectionView::~CCollectionView()
{

}


void
CCollectionView::BindEventHandler()
{
   prepareMenuItem( "Edit", Menu_Edit );
   prepareMenuItem( "Save", Menu_Save );
   prepareMenuItem( "Stats", Menu_Stats );
   prepareMenuItem( "History", Menu_History );
   prepareMenuItem( "Export As XMage", Menu_XMage );
   //prepareMenuItem( "View As Cube", Menu_View_As_Cube );
   registerMenu( "Collection" );
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
CCollectionView::ViewItem( CardInterface* apItem )
{
   auto pCardViewer = m_ptCardViewer->GetController();
   pCardViewer->SetModel( apItem );
}

void
CCollectionView::handleEvent( unsigned int aiEvent )
{
   if( aiEvent == Menu_Edit )
   {
      auto pEditor = new viCollectionEditor( m_ptView, wxID_ANY, m_ptModel->GetColId() );
      pEditor->Show();
      //m_wxDeckbox->ShowCollectionEditor();
   }
   else if( aiEvent == Menu_Save )
   {
      auto ptSF = StoreFrontEnd::Server();
      ptSF->SaveCollection( m_ptModel->GetColId() );
   }
   else if( aiEvent == Menu_Stats )
   {
      auto view = new vStatsViewer( m_ptView, wxID_ANY, m_ptModel );
      view->SetFocus();
      view->Show();
   }
   else if( aiEvent == Menu_History )
   {
      auto view = new viHistoryViewer( m_ptView, wxID_ANY, m_ptModel );
      view->Show();
   }
   else if( aiEvent == Menu_XMage )
   {
      Query listQuery( true );
      listQuery.IncludeMetaType( None );
      listQuery.HashType( CopyItem::HashType::Ids );
      listQuery.SetIncludeMeta( false );
      listQuery.SetIncludeIDs( false );

      auto ptSF = StoreFrontEnd::Server();
      ptSF->ExportCollection( m_ptModel->GetColId(), listQuery );
   }
//   else if( aiEvent == cCollectionDeckBox::Menu_View_As_Cube )
//   {
//      wxCommandEvent updateEvt( wxEVT_MENU );
//      updateEvt.SetId( MainFrame::Menu_View_As );
//      updateEvt.SetString( m_ColID );
//      updateEvt.SetInt( MainFrame::Menu_View_As_Cube );
//      ::wxPostEvent( m_wxDeckbox, updateEvt );
//   }
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
