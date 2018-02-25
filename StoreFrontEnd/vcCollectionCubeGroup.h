#pragma once
#include "wx/wxprec.h"
#include "wx/listctrl.h"
#include "Group.h"
#include <vector>
#include <map>

class GroupItemData;

class vcCollectionCubeGroup : public wxListView
{
public:
   vcCollectionCubeGroup( wxPanel* aptParent,
                          wxWindowID aiWID,
                          wxString aszColumn,
                          int aiColumnIndex );
   ~vcCollectionCubeGroup();

   void PopulateList( std::vector<GroupItemData*> avecItemData, Group aGrp );
   int GetColumnIndex();
   void DeselectAll();
   int GetSelection();

private:
   wxDECLARE_EVENT_TABLE();

   wxString m_szColumnName;
   std::vector<wxString> m_vecHashes;

   int m_iSelection;
   int m_iColumnIndex;

   void onItemSelection( wxListEvent& awxEvt );
   void onItemDeselection( wxListEvent& awxEvt );

   void performDisplay( std::map<wxString, std::vector<GroupItemData*>, Group::Sorting> amapGrps,
                        const Group& agrp );
   void displayNormal( GroupItemData* itemData );
   void displayNormal( const wxString& buf, const wxString& aszHash );
   void performDisplayWithSubGrouping( std::map<wxString, std::vector<GroupItemData*>, Group::Sorting> amapGrps,
                                       const Group& agrp );
};