#pragma once
#include <wx/wxprec.h>
#include "Group.h"
#include <wx/grid.h>
#include <wx/scrolwin.h>
#include <wx/colour.h>
#include <vector>
#include <map>

class GroupItemData;
class wxFont;

class vcCollectionCubeGroup : public wxGrid
{
public:
   vcCollectionCubeGroup( wxPanel* aptParent,
                          wxWindowID aiWID,
                          wxString aszColumn,
                          int aiColumnIndex );
   ~vcCollectionCubeGroup();

   void PopulateList( std::vector<CardInterface*> avecItemData, Group aGrp );
   int GetColumnIndex();
   void DeselectAll();
   int GetSelection();
   void SetSubSectionFontColor( const wxColour& awxColor );
   void SetBackgroundColor(const wxColour& awxColor);

private:
   wxDECLARE_EVENT_TABLE();

   wxString m_szColumnName;
   wxColour m_wxFontColor;
   wxColour m_wxColor;
   wxColour m_wxSubSectionFontColor;
   std::vector<wxString> m_vecHashes;

   int m_iSelection;
   int m_iColumnIndex;

   //void onColumnResize( wxListEvent& awxEvt );

   void onItemSelection( wxGridEvent& awxEvt );

   //void onItemDeselection( wxListEvent& awxEvt );
   void onResize( wxSizeEvent& awxEvt );
   void onSelectRange( wxGridRangeSelectEvent& awxEvt );
   void onSelectColumn( wxGridEvent& awxEvt );
   void onScroll( wxScrollWinEvent& awxEvt );

   void resizeColumn();
   void performDisplay( std::map<wxString, std::vector<CardInterface*>, Group::Sorting> amapGrps,
                        const Group& agrp );
   void performDisplayWithSubGrouping( std::map<wxString, std::vector<CardInterface*>, Group::Sorting> amapGrps,
      const Group& agrp );
   void displayNormal( CardInterface* itemData );
   void displayNormal( const wxString& buf, const wxString& aszHash );
   void displayNormal( const wxString& buf, const wxString& aszHash, const wxFont& awxFont, 
                       bool abAlignCenter, const wxColour& awxColor, const wxColour& awxFontColor,
                       unsigned int aiRowHeight = 0 );

   
   wxFont getSubSectionFont();
};