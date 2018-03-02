#include "vcCollectionCubeGroup.h"
#include "MainWindow.h"
#include "Group.h"
#include "GroupItemData.h"
#include "vcCollectionCubeDisplay.h"
#include <algorithm>
#include <set>
#include <wx/event.h>

#define COLUMN_WIDTH 154
#define COLUMN_TEXT_LENGTH 5000

wxBEGIN_EVENT_TABLE( vcCollectionCubeGroup, wxGrid )
EVT_SIZE( vcCollectionCubeGroup::onResize )
EVT_GRID_RANGE_SELECT( vcCollectionCubeGroup::onSelectRange )
EVT_SCROLLWIN( vcCollectionCubeGroup::onScroll )
EVT_GRID_LABEL_LEFT_CLICK( vcCollectionCubeGroup::onSelectColumn )
//EVT_LIST_COL_BEGIN_DRAG( vcCollectionCubeDisplay::Group_List, vcCollectionCubeGroup::onColumnResize )
EVT_GRID_CELL_LEFT_CLICK( vcCollectionCubeGroup::onItemSelection )
//EVT_LIST_ITEM_DESELECTED( vcCollectionCubeDisplay::Group_List, vcCollectionCubeGroup::onItemDeselection )
wxEND_EVENT_TABLE()

vcCollectionCubeGroup::vcCollectionCubeGroup( wxPanel* aptParent,
                                              wxWindowID aiWID,
                                              wxString aszColumn,
                                              int aiColumnIndex )
   : wxGrid(aptParent, aiWID, wxDefaultPosition, wxDefaultSize, wxBORDER ), m_iSelection(-1), m_iColumnIndex(aiColumnIndex),
     m_szColumnName(aszColumn), m_wxColor(wxColour("WHITE")), m_wxFontColor( wxColour( 78,111,47) ),
     m_wxSubSectionFontColor( wxColour( "BLACK" ) )
{
//   wxFlexGridSizer* boxSizer = new wxFlexGridSizer( 1, 1, 0, 0 );
//   //this->SetScrollbar(wx)
//   boxSizer->AddGrowableCol( 0 );
   this->ShowScrollbars( wxScrollbarVisibility::wxSHOW_SB_NEVER, wxScrollbarVisibility::wxSHOW_SB_NEVER );
   this->CreateGrid( 0, 1 );
   this->DisableDragColSize();
   this->DisableDragRowSize();
   this->HideRowLabels();
   this->EnableGridLines( false );

   // Order is Count, Name*, Mana Cost, Card Type
   //this->SetSizer( boxSizer );
   //this->SetSizeHints( wxSize( COLUMN_WIDTH, 2000 ) );
}

vcCollectionCubeGroup::~vcCollectionCubeGroup()
{

}

void 
vcCollectionCubeGroup::PopulateList( std::vector<GroupItemData*> avecItemData, Group aGrp )
{
   wxString szColumn = m_szColumnName + " (" + std::to_string(avecItemData.size()) + ")";
   this->SetColLabelValue( 0, szColumn );

   map<wxString, vector<GroupItemData*>, Group::Sorting> mapGroups( *aGrp.GetSortingFunctor() );
   for( auto& data : avecItemData )
   {
      // TODO: Need way to get meta tags from server so we can sort on them.
      auto szGroup = aGrp.GetGroup( *data );
      mapGroups[szGroup].push_back( data );
   }

   if( aGrp.GetSubGroup( "" ).IsEmpty() )
   {
      performDisplay( mapGroups, aGrp );
   }
   else
   {
      performDisplayWithSubGrouping( mapGroups, aGrp );
   }

   //this->SetSizeHints( wxSize( COLUMN_WIDTH, this->GetItemCount()*19.15 + 30 ) );
   this->resizeColumn();
}

int 
vcCollectionCubeGroup::GetColumnIndex()
{
   return m_iColumnIndex;
}

void 
vcCollectionCubeGroup::DeselectAll()
{
   //auto selectedItem = this->GetFirstSelected();
   //while( selectedItem != -1 )
   //{
   //   this->Select( selectedItem, false );
   //   selectedItem = this->GetNextSelected( selectedItem );
   //}
}

int 
vcCollectionCubeGroup::GetSelection()
{
   return m_iSelection;
}

void 
vcCollectionCubeGroup::SetSubSectionFontColor( const wxColour& awxColor )
{
   m_wxSubSectionFontColor = awxColor;
}

void 
vcCollectionCubeGroup::SetBackgroundColor( const wxColour& awxColor )
{
   m_wxColor = awxColor;
}

void
vcCollectionCubeGroup::onItemSelection( wxGridEvent& awxEvt )
{
   m_iSelection = awxEvt.GetRow();
   auto arrCells = this->GetSelectedCells();
   for( size_t i = 0; i < arrCells.size(); i++ )
   {
      this->DeselectCell( arrCells[i].GetRow(), arrCells[i].GetCol() );
   }
   if( !m_vecHashes[m_iSelection].IsEmpty() )
   {
      awxEvt.SetString( m_vecHashes[m_iSelection] );
      awxEvt.Skip();
   }
}

//void
//vcCollectionCubeGroup::onItemDeselection( wxListEvent& awxEvt )
//{
//   m_iSelection = -1;
//   awxEvt.StopPropagation();
//}

void 
vcCollectionCubeGroup::onResize( wxSizeEvent& awxEvt )
{
   resizeColumn();
   awxEvt.Skip();
}

void 
vcCollectionCubeGroup::onSelectRange( wxGridRangeSelectEvent& awxEvt )
{
   this->DeselectAll();
   awxEvt.StopPropagation();
}

void 
vcCollectionCubeGroup::onSelectColumn( wxGridEvent& awxEvt )
{
   awxEvt.StopPropagation();
}

void 
vcCollectionCubeGroup::onScroll( wxScrollWinEvent& awxEvt )
{
   awxEvt.ResumePropagation( 999 );
   awxEvt.Skip();
}

void 
vcCollectionCubeGroup::resizeColumn()
{
   this->SetColSize( 0, this->GetVirtualSize().GetWidth() );
}

void 
vcCollectionCubeGroup::performDisplay(
   map<wxString, vector<GroupItemData*>, Group::Sorting> amapGrps, const Group& aGrp )
{
   bool bFirst = true;
   for( auto groupData : amapGrps )
   {
      if( !bFirst )
      {
         displayNormal( "", "", wxFont( wxFontInfo( 2 ).FaceName( "Trebuchet MS" ) ).Bold(),
                        false, m_wxColor, m_wxFontColor );
      }

      if( !aGrp.IsEmpty() )
      {
         wxString buf = groupData.first + " (" + std::to_string( groupData.second.size() ) + ")";
         displayNormal( buf, "", getSubSectionFont(), true, m_wxColor, m_wxSubSectionFontColor );
      }

      for( auto& itemData : groupData.second )
      {
         displayNormal( itemData );
      }
      bFirst = false;
   }
}

void
vcCollectionCubeGroup::performDisplayWithSubGrouping(
   map<wxString, vector<GroupItemData*>, Group::Sorting> amapGrps,
   const Group& aGrp )
{
   Group subGroup = aGrp.GetSubGroup( "" );

   int iLast = amapGrps.size();
   int index = 0;
   for( auto groupData : amapGrps )
   {
      map<wxString, multiset<GroupItemData*, Group::ItemSorting>, Group::Sorting>
         mapSubGrpSorting( *subGroup.GetSortingFunctor() );
      for( auto& itemData : groupData.second )
      {
         wxString szSubGrp = subGroup.GetGroup( *itemData );
         mapSubGrpSorting[szSubGrp].insert( itemData );
      }

      if( !aGrp.IsEmpty() )
      {
         wxString buf = groupData.first + " (" + std::to_string( groupData.second.size() ) + ")";
         displayNormal( buf, "", getSubSectionFont(), true, m_wxColor, m_wxSubSectionFontColor );
      }

      bool bFirst = true;
      for( auto& itemSubGrpData : mapSubGrpSorting )
      {
         if( !bFirst )
         {
            // Space in between each subgroup.
            displayNormal( "", "", getSubSectionFont(), false, m_wxColor, m_wxFontColor);
         }

         bFirst = false;
         for( auto& itemData : itemSubGrpData.second )
         {
            displayNormal( itemData );
         }
      }

      if( ++index != iLast )
      {
         // Space in between each subgroup.
         displayNormal( "", "" );
      }
   }
}


void vcCollectionCubeGroup::displayNormal( GroupItemData* itemData )
{
   const int cutoff = COLUMN_TEXT_LENGTH;
   wxString buf = itemData->GetName();
   if( buf.size() > cutoff )
   {
      // Split at a space.
      unsigned int iStart = string::npos;
      unsigned int iPrev = 0;
      while( (iPrev = buf.rfind( ' ', iStart-1 )) > cutoff )
      {
         if( iPrev == iStart || iStart == 0)
         {
            break;
         }
         iStart = iPrev;
      }
      if( iPrev <= cutoff )
      {
         iStart = iPrev;
      }
      else
      {
         iStart = 0;
      }

      if( iStart != 0 && iStart < buf.Trim().size()-1 )
      {
         displayNormal( buf.substr( 0, iStart ), itemData->GetHash() );
         displayNormal( buf.substr( iStart, buf.size() - iStart ), itemData->GetHash() );
      }
      else
      {
         displayNormal( buf, itemData->GetHash() );
      }

   }
   else
   {
      displayNormal( buf, itemData->GetHash() );
   }
}

void vcCollectionCubeGroup::displayNormal( const wxString &buf, const wxString& aszHash )
{
   displayNormal( buf, aszHash, wxFont( wxFontInfo( 9 ).FaceName( "Trebuchet MS" ) ), false, m_wxColor, m_wxFontColor );
}

void 
vcCollectionCubeGroup::displayNormal( const wxString& buf, const wxString& aszHash,
                                      const wxFont& awxFont, bool abAlignCenter, 
                                      const wxColour& awxColor, const wxColour& awxFontColor,
                                      unsigned int aiRowHeight )
{
   int iRow = this->GetNumberRows();
   this->InsertRows( this->GetNumberRows() );
   this->SetCellValue( iRow, 0, buf );
   this->SetCellFont( iRow, 0, awxFont );
   this->SetReadOnly( iRow, 0, true );
   this->SetCellBackgroundColour( iRow, 0, awxColor );
   if( abAlignCenter )
   {
      this->SetCellAlignment( iRow, 0, wxALIGN_CENTER, wxALIGN_CENTER );
   }
   if( aiRowHeight > 0 )
   {
      this->SetRowMinimalHeight( iRow, aiRowHeight );
      this->SetRowSize( iRow, aiRowHeight );
   }
   this->SetCellTextColour( iRow, 0, awxFontColor );
   this->SetCellRenderer( iRow, 0, new wxGridCellAutoWrapStringRenderer() );
   m_vecHashes.push_back( aszHash );
}

wxFont 
vcCollectionCubeGroup::getSubSectionFont()
{
   return wxFont( wxFontInfo( 11 ).FaceName( "Trebuchet MS" ) ).Bold();
}