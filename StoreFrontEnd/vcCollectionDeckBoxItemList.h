#pragma once
#include "wx/wxprec.h"
#include <map>
#include <wx/listctrl.h>

class GroupItemData;


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
   GroupItemData GetItemByListIndex(int Ind);
   GroupItemData GetFirst();
   int GetFirstInt();
   bool IsEmpty();
private:
   // This is so we can populate event data before sending it up.
   wxDECLARE_EVENT_TABLE();

   std::vector<GroupItemData> m_vecDataItems;
   std::vector<GroupItemData*> m_vecDataItemsDisplayOrder;
   wxListCtrl* m_wxListControl;
   wxString m_wxszColID;

   int m_iSelection;

   void onItemSelection(wxListEvent& awxEvt);
   void onItemDeselection(wxListEvent& awxEvt);
   std::map<wxString, std::vector<GroupItemData*>> 
      performGrouping(std::vector<GroupItemData>& avecItems);

   void displayGrouping(const std::map<wxString,std::vector<GroupItemData*>> &amapGrouping);
   void addListItem(GroupItemData& aData);
};

