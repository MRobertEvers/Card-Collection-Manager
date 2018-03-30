#include "IMenuEventHandler.h"
#include "IMenuEventSource.h"

IMenuEventHandler::IMenuEventHandler( IMenuEventSource* aParent)
{
   m_MFParent = aParent;
}

IMenuEventHandler::~IMenuEventHandler()
{

}

void 
IMenuEventHandler::MenuEvent(wxCommandEvent& awxEvt)
{
   auto iEvtID = awxEvt.GetId();
   handleEvent(iEvtID);
}

void 
IMenuEventHandler::ReleaseEventHandler()
{
   for( auto& menuItem : m_mapTitleToEventID )
   {
      m_MFParent->Unbind(wxEVT_MENU, &IMenuEventSource::OnGenericMenuEvent, m_MFParent, menuItem.second);
   }
}


void 
IMenuEventHandler::registerSendMenuEvents()
{
   m_MFParent->RegisterSendMenuEvents();
}

void 
IMenuEventHandler::prepareMenuItem(const wxString& aszTitle, const unsigned int aiID)
{
   m_mapTitleToEventID.insert(std::make_pair(aszTitle, aiID));
}

void 
IMenuEventHandler::registerMenu(const wxString& aszMenuName)
{
   wxMenu* menuNew = new wxMenu();
   for( auto& menuItem : m_mapTitleToEventID )
   {
      wxMenuItem* wxMI = new wxMenuItem(menuNew, menuItem.second, menuItem.first);
      menuNew->Append(wxMI);
      m_MFParent->Bind(wxEVT_MENU, &IMenuEventSource::OnGenericMenuEvent, m_MFParent, menuItem.second);
   }
   m_MFParent->RegisterMenu(menuNew, aszMenuName);
}