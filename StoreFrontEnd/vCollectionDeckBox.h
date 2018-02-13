#pragma once
#include <wx/listctrl.h>
#include <wx/object.h> 
#include "wx/wxprec.h"
#include "viCardEditor.h"
#include "IMenuEventHandler.h"

class vcCollectionDeckBoxItemList;
class MainFrame;

class vCollectionDeckBox : public wxPanel
{
public:


   vCollectionDeckBox(MainFrame* aptParent, wxWindowID aiWID, const wxString& aszColID);
   ~vCollectionDeckBox();

   void ShowCollectionEditor();

private:
   wxDECLARE_EVENT_TABLE();

   vcCollectionDeckBoxItemList* m_vcItemList;
   viCardEditor* m_viCardEditor;
   wxString m_wxszColID;

   void onEditorAccept(wxCommandEvent& awxEvt);
   void onNewItemSelected(wxListEvent& awxEvt);
   void onDeckEditor(wxCommandEvent& awxEvt);
   void buildItemList();
   void notifyCardEditor(const wxString& aszHash);
};

