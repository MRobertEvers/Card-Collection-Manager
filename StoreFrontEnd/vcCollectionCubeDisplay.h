#pragma once
#include "wx/wxprec.h"
#include "wx/listctrl.h"
#include "GroupItemData.h"
#include <map>
#include <vector>

class vcCollectionCubeGroup;

class vcCollectionCubeDisplay : public wxPanel
{
public:
   enum
   {
      Group_List = 0x0
   };
   vcCollectionCubeDisplay( wxPanel* aptParent, wxWindowID aiWID, const wxString& aszColID );
   ~vcCollectionCubeDisplay();

   void RefreshList();
   GroupItemData GetItemByListIndex( int Ind );
   GroupItemData GetFirst();
   int GetFirstInt();
   bool IsEmpty();

private:

   std::map<wxString, GroupItemData> m_mapItemData;
   std::map<int, vcCollectionCubeGroup*> m_mapColGroups;
   std::vector<GroupItemData> m_vecDataItems;
   wxString m_szColID;

   void clearDisplay();
   void onItemSelection( wxListEvent& awxEvt );
   std::map<wxString, std::vector<GroupItemData*>> defaultGrouping( std::vector<GroupItemData>& avecItems );
};