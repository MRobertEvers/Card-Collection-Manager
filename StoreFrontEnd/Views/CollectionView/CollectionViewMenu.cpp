#include "CollectionViewMenu.h"
#include "CCollectionView.h"


CollectionViewMenu::CollectionViewMenu( CCollectionView* apController )
   : m_pController(apController)
{

}

CollectionViewMenu::~CollectionViewMenu()
{

}

// IMenuEventHandler
// The instance handle events here
void
CollectionViewMenu::OnMenuEvent( wxCommandEvent& awxEvt )
{
   unsigned int iEvent = awxEvt.GetId();
   if( iEvent == Menu_Edit )
   {
      m_pController->OnDoEdit();
   }
   else if( iEvent == Menu_Save )
   {
      m_pController->OnSave();
   }
   else if( iEvent == Menu_Stats )
   {
      m_pController->OnViewStats();
   }
   else if( iEvent == Menu_History )
   {
      m_pController->OnViewHistory();
   }
   else if( iEvent == Menu_XMage )
   {
      m_pController->OnExportXMage();
   }
}


// The instance puts itself on the menubar
void
CollectionViewMenu::BindToMenu( wxMenuBar* apMenu, wxEvtHandler* apFrame )
{
   wxMenu* menuNew = new wxMenu();
   appendToMenu( apFrame, menuNew, "Save", Menu_Save );
   appendToMenu( apFrame, menuNew, "Edit", Menu_Edit );
   appendToMenu( apFrame, menuNew, "View Stats", Menu_Stats );
   appendToMenu( apFrame, menuNew, "View History", Menu_History );
   appendToMenu( apFrame, menuNew, "XPort XMage", Menu_XMage );
   apMenu->Append( menuNew, "Collection" );
}

void
CollectionViewMenu::ReleaseMenu( wxMenuBar* apMenu, wxEvtHandler* apFrame )
{
   auto iMenu = apMenu->FindMenu( "Collection" );
   apMenu->Remove( iMenu );
}

void 
CollectionViewMenu::appendToMenu( wxEvtHandler* apFrame, wxMenu* apMenu,
                                  const wxString& aszItemName, unsigned int aiID )
{
   wxMenuItem* wxMI = new wxMenuItem( apMenu, aiID, aszItemName );
   apMenu->Append( wxMI );
   apFrame->Bind( wxEVT_MENU, &CollectionViewMenu::OnMenuEvent, this, aiID );
}