#pragma once
#include "wx/wxprec.h"
#include <map>
#include <wx/listctrl.h>

class vcdCDBIListItemData;

class vcCollectionDeckBoxItemList : public wxPanel
{
public:
   class Group
   {
   public:
      Group& GroupOn(const wxString& aszKey, bool abIsMetaKey = true);
      Group& BroadenSubGroup(const wxString& aszValue);
      Group& OverrideGrouping(const Group& aGrouping);

      wxString GetGroup(const vcdCDBIListItemData& aData);
   private:
      wxString Key;
      bool MetaKey;
      std::vector<wxString> BroadenedValues;
      std::vector<Group> Overrides;
   };

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
   std::map<wxString, std::vector<vcdCDBIListItemData*>> 
      defaultGrouping(std::vector<vcdCDBIListItemData>& avecItems);

   void displayGrouping(const std::map<wxString,std::vector<vcdCDBIListItemData*>> &amapGrouping);
   void addListItem(vcdCDBIListItemData& aData);
};

