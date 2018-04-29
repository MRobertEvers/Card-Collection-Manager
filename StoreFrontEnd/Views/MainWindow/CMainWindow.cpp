#include "CMainWindow.h"
#include "VMainWindow.h"
#include "../ViewTemplates/IMenuEventHandler.h"
#include "../StoreFrontEnd/StoreFrontEnd.h"
#include "../CCollectionView.h"
#include "../VCollectionView.h"
#include "../ViewTemplates/ivCollectionView.h"
#include "../ViewTemplates/IControlledView.h"
#include "../CollectionsOverview/CollectionsOverview.h"
#include "../CollectionsOverview/vCollectionsOverview.h"

CMainWindow::CMainWindow( VMainWindow* aptView )
   : m_View(aptView)
{
   ShowCollectionsOverview();
}


CMainWindow::~CMainWindow()
{
}

void 
CMainWindow::ShowCollectionsOverview()
{
   auto memleak = new CollectionsOverview( m_View );
   m_ptrControlledView = std::shared_ptr<IControlledView>( memleak );
   m_View->ReleaseMenuEventHandler();
   m_View->SetView( memleak->GetView() );
}

void 
CMainWindow::ShowCollection( const wxString& aszColID, CollectionViewType aType )
{
   auto ptsf = StoreFrontEnd::Server();
   auto ptse = StoreFrontEnd::Client();
   wxString szID = ptsf->GetCollectionID( aszColID.ToStdString() );
   // TODO: THIS SHOULD NOT BE A LITERAL STRING.
   if( szID == "NF" )
   {
      szID = aszColID;
   }

   if( aType == Cube_View )
   {
      m_View->ReleaseMenuEventHandler();

      VCollectionView* view = new VCollectionView( m_View, wxID_ANY );
      auto ptCol = new CollectionInterface( szID.ToStdString() );
      ptCol->Refresh();
      auto pt = std::shared_ptr<CollectionInterface>( ptCol );
      CCollectionView* cube = new CCollectionView( view, pt );
      //m_ptrControlledView = std::shared_ptr<IControlledView>( view );
      view->SetController( cube );
      cube->SetCubeRenderer();
      m_View->SetView( view );

   }
   else
   {
      //m_View->ReleaseMenuEventHandler();

      //CollectionDeckBox* cube = new CollectionDeckBox( m_View, szID );
      //m_ptrControlledView = std::shared_ptr<IControlledView>( cube );
      //m_View->SetView( cube->GetView() );
   }
}

void 
CMainWindow::ImportSource()
{
   StoreFrontEnd::Client()->DownloadImportSourceFile();
   StoreFrontEnd::Server()->ImportCollectionSource();
}