#pragma once
#include <wx/wxprec.h>
#include <vector>

class IMenuEventHandler;

class IMenuEventSource : public wxFrame
{
public:
   IMenuEventSource( wxWindowID id, const wxString& aszTitle );
   ~IMenuEventSource();

   void RegisterMenu( wxMenu* fileMenu, const wxString& aszTitle );
   void RegisterSendMenuEvents();
   void ReleaseMenuEventHandler();
   void BindMenuEventHandler( IMenuEventHandler* aEventHandler );

   void OnGenericMenuEvent( wxCommandEvent& awxEvt );

protected:
   void uiProtectMenus( unsigned int aiNum );
   wxMenuBar* m_wxMenuBar;

private:
   IMenuEventHandler* m_EventHandler;
   std::vector<wxMenu*> m_vecViewMenus;
   unsigned int m_uiDefaultMenus;
};

