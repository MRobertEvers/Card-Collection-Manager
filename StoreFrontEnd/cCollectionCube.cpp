#include "cCollectionCube.h"
#include "vCollectionCube.h"
#include "StoreFrontEnd.h"
#include "MainWindow.h"
#include "IMenuEventSource.h"

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
}
