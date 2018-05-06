#pragma once
#include <map>
#include <wx\wxprec.h>
#include <wx\string.h>
#include <wx/event.h>
#include <wx/menu.h>

// Events must be bound to the main window because the owner of this handler
// isn't in the event chain of the menu that it binds to.
class IMenuEventHandler
{
public:
   enum
   {
      GenericMenuEvent = 0x444
   };

public:
   virtual ~IMenuEventHandler() {}

   // The instance handle events here
   virtual void OnMenuEvent(wxCommandEvent& awxEvt) = 0;

   // The instance puts itself on the menubar
   virtual void BindToMenu( wxMenuBar* apMenu, wxEvtHandler* apFrame ) = 0;

   virtual void ReleaseMenu( wxMenuBar* apMenu, wxEvtHandler* apFrame ) = 0;
};