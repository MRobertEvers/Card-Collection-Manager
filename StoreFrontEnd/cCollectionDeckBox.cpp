#include "cCollectionDeckBox.h"
#include "vCollectionDeckBox.h"
#include "StoreFrontEnd.h"
#include "MainWindow.h"

cCollectionDeckBox::cCollectionDeckBox(MainFrame* aParent, const wxString& aszColName)
   : IMenuEventHandler(aParent), m_ColID(aszColName)
{
   auto ptSF = StoreFrontEnd::Server();
   m_wxDeckbox = new vCollectionDeckBox(aParent, 9, m_ColID );

   registerSendMenuEvents();
}

cCollectionDeckBox::~cCollectionDeckBox()
{
   m_wxDeckbox->Destroy();
}

wxPanel* 
cCollectionDeckBox::GetView()
{
   return m_wxDeckbox;
}

void
cCollectionDeckBox::BindEventHandler()
{
   prepareMenuItem("Edit", Menu_DeckEditor);
   prepareMenuItem( "Save", Menu_Save );
   prepareMenuItem( "View As Cube", Menu_View_As_Cube );
   registerMenu("Collection");
}

void
cCollectionDeckBox::handleEvent(unsigned int aiEvent)
{
   if( aiEvent == Menu_DeckEditor )
   {
      m_wxDeckbox->ShowCollectionEditor();
   }
   else if( aiEvent == Menu_Save )
   {
      auto ptSF = StoreFrontEnd::Server();
      ptSF->SaveCollection( m_ColID.ToStdString() );
   }
   else if( aiEvent == cCollectionDeckBox::Menu_View_As_Cube )
   {
      wxCommandEvent updateEvt( wxEVT_MENU );
      updateEvt.SetId( MainFrame::Menu_View_As );
      updateEvt.SetString( m_ColID );
      updateEvt.SetInt( MainFrame::Menu_View_As_Cube );
      ::wxPostEvent( m_wxDeckbox, updateEvt );
   }
}
