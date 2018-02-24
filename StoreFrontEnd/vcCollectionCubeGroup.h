#pragma once
#include "wx/wxprec.h"
#include "wx/listctrl.h"
#include "Group.h"
#include <vector>

class Group;
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
   void DeselectAll();
   int GetSelection();

private:
   wxDECLARE_EVENT_TABLE();

   std::vector<wxString> m_vecHashes;

   int m_iSelection;
   int m_iColumnIndex;

   void onItemSelection( wxListEvent& awxEvt );
   void onItemDeselection( wxListEvent& awxEvt );
};