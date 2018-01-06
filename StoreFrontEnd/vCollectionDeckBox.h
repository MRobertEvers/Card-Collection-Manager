#pragma once
#include "wx/wxprec.h"
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
   wxString m_wxszColID;

   void onEditorAccept(wxCommandEvent& awxEvt);
   void buildItemList();
};

