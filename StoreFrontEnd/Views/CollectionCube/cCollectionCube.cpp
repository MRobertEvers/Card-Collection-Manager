#include "cCollectionCube.h"
#include "vCollectionCube.h"
#include "../StoreFrontEnd/StoreFrontEnd.h"
#include "../MainWindow/MainWindow.h"
#include "../ViewTemplates/IMenuEventSource.h"

cCollectionCube::cCollectionCube( IMenuEventSource* aParent, vCollectionCube* aView, const wxString& aszColName )
   : IMenuEventHandler( aParent ), m_ColID( aszColName ), m_wxCube(aView)
{
   registerSendMenuEvents();
}

cCollectionCube::~cCollectionCube()
{

}

wxPanel*
cCollectionCube::GetView()
{
   return m_wxCube;
}

void
cCollectionCube::BindEventHandler()
{
   prepareMenuItem( "Edit", Menu_DeckEditor );
   prepareMenuItem( "Save", Menu_Save );
   prepareMenuItem( "Stats", Menu_Stats );
   prepareMenuItem( "History", Menu_ViewHist );
   prepareMenuItem( "View As Deck", Menu_View_As_Deck );
   // TODO: I want this to be a submenu.
   prepareMenuItem( "Export As XMage", Menu_Export_XMage );
   registerMenu( "Collection" );
}

void
cCollectionCube::handleEvent( unsigned int aiEvent )
{
   if( aiEvent == Menu_DeckEditor )
   {
      m_wxCube->ShowCollectionEditor();
   }
   else if( aiEvent == Menu_Save )
   {
      auto ptSF = StoreFrontEnd::Server();
      ptSF->SaveCollection( m_ColID.ToStdString() );
   }
   else if( aiEvent == Menu_Stats )
   {
      m_wxCube->ShowStats();
   }
   else if( aiEvent == cCollectionCube::Menu_View_As_Deck )
   {
      wxCommandEvent updateEvt( wxEVT_MENU );
      updateEvt.SetId( MainFrame::Menu_View_As );
      updateEvt.SetString( m_ColID );
      updateEvt.SetInt( MainFrame::Menu_View_As_Deck );
      ::wxPostEvent( m_wxCube, updateEvt );
   }
   else if( aiEvent == cCollectionCube::Menu_ViewHist )
   {
      m_wxCube->ShowHistory();
   }
   else if( aiEvent == cCollectionCube::Menu_Export_XMage )
   {
      Query listQuery( true );
      listQuery.IncludeMetaType( None );
      listQuery.HashType( CopyItem::HashType::Ids );
      listQuery.SetIncludeMeta( false );
      listQuery.SetIncludeIDs( false );

      auto ptSF = StoreFrontEnd::Server();
      ptSF->ExportCollection( m_ColID.ToStdString(), listQuery );
   }
}
