#pragma once
#include "wx/wxprec.h"
#include <wx/listctrl.h>
#include <string>

class vcCollectionsMenuList : public wxPanel
{
public:
   enum
   {
      List = 0x0
   };

   vcCollectionsMenuList(wxWindow* aptParent);
   ~vcCollectionsMenuList();

   void AddCollectionOption(std::string aszCollectionName);

private:
   // This is so we can populate event data before sending it up.
   wxDECLARE_EVENT_TABLE();

   wxListCtrl* m_wxListControl;
   int m_iSelection;

   void buildButtons();

   void onViewCollection(wxCommandEvent& awxEvt);
   void onItemSelection(wxListEvent& awxEvt);
   void onItemDeselection(wxListEvent& awxEvt);
};

