#include "CMainWindow.h"
#include "VMainWindow.h"
#include "../StoreFrontEnd/StoreFrontEnd.h"
#include "../ViewTemplates/IMenuEventHandler.h"
#include "../Views/CollectionView/CollectionView.h"
#include "../Views/CollectionView/CCollectionView.h"
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
   uiSetMainPanel( std::shared_ptr<IControlledView>( new CollectionsOverview( m_View ) ) );
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
      uiSetMainPanel( std::shared_ptr<IControlledView>( new CollectionView( m_View, ptse->GetCollection( szID ) ) ) );
   }
}

void 
CMainWindow::ImportSource()
{
   StoreFrontEnd::Client()->DownloadImportSourceFile();
   StoreFrontEnd::Server()->ImportCollectionSource();
}

void 
CMainWindow::uiSetMainPanel( std::shared_ptr<IControlledView> apView )
{
   if( m_ptrControlledView != nullptr )
   {
      auto pMenuHandler = m_ptrControlledView->GetEventHandler();
      if( pMenuHandler != nullptr )
      {
         pMenuHandler->ReleaseMenu( m_View->GetMenu(), m_View );
      }
   }

   m_ptrControlledView = apView;
   auto pMenuHandler = m_ptrControlledView->GetEventHandler();
   if( pMenuHandler != nullptr )
   {
      pMenuHandler->BindToMenu( m_View->GetMenu(), m_View );
   }

   m_View->SetView( m_ptrControlledView->GetView() );
}