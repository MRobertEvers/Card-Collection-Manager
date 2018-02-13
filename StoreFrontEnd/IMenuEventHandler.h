#pragma once
#include <map>
#include <wx\wxprec.h>
#include <wx\string.h>
#include <wx/event.h>
#include <wx/menu.h>

class MainFrame;

// Requires      registerSendMenuEvents to be put in the constructor
// so that the event receiver knows to register event handlers
class IMenuEventHandler
{
public:
   IMenuEventHandler(MainFrame* aParent);
   virtual ~IMenuEventHandler();

   void MenuEvent(wxCommandEvent& awxEvt);

   virtual wxPanel* GetView() = 0;
   virtual void BindEventHandler() = 0;

   void ReleaseEventHandler();

protected:
   virtual void handleEvent(unsigned int aiEvent) = 0;

   void registerSendMenuEvents();
   
   void prepareMenuItem(const wxString& aszTitle, const unsigned int aiID);

   void registerMenu(const wxString& aszMenuName);

   MainFrame* m_MFParent;
private:
   std::map<wxString, unsigned int> m_mapTitleToEventID;
};