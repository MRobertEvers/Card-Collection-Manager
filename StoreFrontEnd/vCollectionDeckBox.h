#pragma once
#include <wx/listctrl.h>
#include "wx/wxprec.h"
#include "viCardEditor.h"

class vcCollectionDeckBoxItemList;

class vCollectionDeckBox : public wxPanel
{
public:
   vCollectionDeckBox(wxWindow* aptParent, wxWindowID aiWID, const wxString& aszColID);
   ~vCollectionDeckBox();

   void ShowCollectionEditor();

private:
   wxDECLARE_EVENT_TABLE();

   vcCollectionDeckBoxItemList* m_vcItemList;
   viCardEditor* m_viCardEditor;
   wxString m_wxszColID;

   void onEditorAccept(wxCommandEvent& awxEvt);
   void onNewItemSelected(wxListEvent& awxEvt);
   void buildItemList();
   void notifyCardEditor(const wxString& aszHash);
};

