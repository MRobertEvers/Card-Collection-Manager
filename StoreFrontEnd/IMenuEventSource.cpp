#include "IMenuEventSource.h"
#include "IMenuEventHandler.h"


IMenuEventSource::IMenuEventSource(wxWindowID id, const wxString& aszTitle )
   : wxFrame(NULL, id, aszTitle), m_EventHandler( nullptr )
{

}


IMenuEventSource::~IMenuEventSource()
{
}

void 
IMenuEventSource::RegisterMenu( wxMenu* fileMenu, const wxString& aszTitle )
{
   m_wxMenuBar->Append( fileMenu, aszTitle );
}

void
IMenuEventSource::RegisterSendMenuEvents()
{
   // m_bEvtHandlerViewFlag = true;
}

void
IMenuEventSource::ReleaseMenuEventHandler()
{
   // Remove Menus from menu bar
   auto iMenus = m_wxMenuBar->GetMenuCount();
   for( size_t i = m_uiDefaultMenus; i < iMenus; i++ )
   {
      m_wxMenuBar->Remove( i );
   }

   // Unbind Events
   if( m_EventHandler )
   {
      m_EventHandler->ReleaseEventHandler();
   }

   // Clear tracking menus
   for( unsigned int i = 0; i < m_vecViewMenus.size(); i++ )
   {
      auto ptMenu = m_wxMenuBar->Remove( i + 2 );
      delete ptMenu;
   }
   m_vecViewMenus.clear();
}

void
IMenuEventSource::BindMenuEventHandler( IMenuEventHandler* aEventHandler )
{
   m_EventHandler = aEventHandler;

   if( m_EventHandler )
   {
      m_EventHandler->BindEventHandler();
   }
}

void
IMenuEventSource::OnGenericMenuEvent( wxCommandEvent& awxEvt )
{
   if( m_EventHandler )
   {
      m_EventHandler->MenuEvent( awxEvt );
   }
}

void 
IMenuEventSource::uiProtectMenus( unsigned int aiNum )
{
   m_uiDefaultMenus = aiNum;
}