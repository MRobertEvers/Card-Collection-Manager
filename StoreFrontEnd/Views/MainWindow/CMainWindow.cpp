#include "CMainWindow.h"
#include "VMainWindow.h"
#include "../ViewTemplates/IMenuEventHandler.h"
#include "../StoreFrontEnd/StoreFrontEnd.h"
#include "../Views/CollectionView/CollectionView.h"
#include "../Views/CollectionView/CollectionView.h"
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
   m_View->ReleaseMenuEventHandler();

   auto tmp = new CollectionsOverview( m_View );
   m_ptrControlledView = std::shared_ptr<IControlledView>( tmp );
   m_View->SetView( tmp->GetView() );
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

      auto tmp = new CollectionView( m_View, ptse->GetCollection( szID ) );

      m_ptrControlledView = std::shared_ptr<IControlledView>( tmp );
      m_View->SetView( tmp->GetView() );
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