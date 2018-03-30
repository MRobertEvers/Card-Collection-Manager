#include "CMainWindow.h"
#include "IMenuEventHandler.h"
#include "StoreFrontEnd.h"
#include "CollectionCube.h"
#include "VMainWindow.h"
#include "ivCollectionView.h"
#include "CollectionsOverview.h"
#include "cCollectionCube.h"
#include "vCollectionsOverview.h"

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
   auto memleak = new CollectionsOverview( m_View );
   m_View->SetView( memleak->GetView() );
}

void 
CMainWindow::ShowCollection( const wxString& aszColID, CollectionViewType aType )
{
   if( aType == Cube_View )
   {
      m_View->ReleaseMenuEventHandler();

      CollectionCube cube(m_View, aszColID);
      m_View->SetView( cube.GetView() );
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