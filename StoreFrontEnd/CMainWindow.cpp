#include "CMainWindow.h"
#include "IMenuEventHandler.h"
#include "StoreFrontEnd.h"
#include "CollectionCube.h"
#include "VMainWindow.h"
#include "ivCollectionView.h"
#include "CollectionsOverview.h"
#include "cCollectionCube.h"
#include "vCollectionsOverview.h"
#include "IControlledView.h"

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

   if( aType == Cube_View )
   {
      m_View->ReleaseMenuEventHandler();

      CollectionCube* cube = new CollectionCube(m_View, ptsf->GetCollectionID(aszColID.ToStdString()));
      m_ptrControlledView = std::shared_ptr<IControlledView>( cube );
      m_View->SetView( cube->GetView() );
   }
   else
   {

   }
}

void 
CMainWindow::ImportSource()
{
   StoreFrontEnd::Client()->DownloadImportSourceFile();
   StoreFrontEnd::Server()->ImportCollectionSource();
}