#pragma once
#include "wx/wxprec.h"
#include <wx/listctrl.h>

class vcCollectionDeckBoxItemList : public wxPanel
{
public:
   enum
   {
      List = 0x0
   };

   vcCollectionDeckBoxItemList(wxWindow* aptParent, const wxString& aszColID);
   ~vcCollectionDeckBoxItemList();

   void RefreshList();

private:
   // This is so we can populate event data before sending it up.
   wxDECLARE_EVENT_TABLE();

   wxListCtrl* m_wxListControl;
   wxString m_wxszColID;

   int m_iSelection;

   void onItemSelection(wxListEvent& awxEvt);
   void onItemDeselection(wxListEvent& awxEvt);
};

