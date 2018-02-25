#include "vcCollectionCubeGroup.h"
#include "MainWindow.h"
#include "wx/wxprec.h"
#include "Group.h"
#include "GroupItemData.h"
#include "vcCollectionCubeDisplay.h"
#include <algorithm>
#include <set>

#define COLUMN_WIDTH 160
#define COLUMN_TEXT_LENGTH 20

wxBEGIN_EVENT_TABLE( vcCollectionCubeGroup, wxPanel )
EVT_LIST_ITEM_SELECTED( vcCollectionCubeDisplay::Group_List, vcCollectionCubeGroup::onItemSelection )
EVT_LIST_ITEM_DESELECTED( vcCollectionCubeDisplay::Group_List, vcCollectionCubeGroup::onItemDeselection )
wxEND_EVENT_TABLE()

vcCollectionCubeGroup::vcCollectionCubeGroup( wxPanel* aptParent,
                                              wxWindowID aiWID,
                                              wxString aszColumn,
                                              int aiColumnIndex )
   : wxListView(aptParent, aiWID, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxVSCROLL | wxALWAYS_SHOW_SB ), m_iSelection(-1), m_iColumnIndex(aiColumnIndex),
     m_szColumnName(aszColumn)
{
   wxFlexGridSizer* boxSizer = new wxFlexGridSizer( 1, 1, 0, 0 );
   //this->SetScrollbar(wx)
   boxSizer->AddGrowableCol( 0 );

   // Order is Count, Name*, Mana Cost, Card Type
   this->SetSizer( boxSizer );
   this->SetSizeHints( wxSize( COLUMN_WIDTH, wxDefaultSize.GetHeight() ) );
}

vcCollectionCubeGroup::~vcCollectionCubeGroup()
{

}

void 
vcCollectionCubeGroup::PopulateList( std::vector<GroupItemData*> avecItemData, Group aGrp )
{
   //this->SetFont( wxFont( wxFontInfo( 8 ).FaceName( "Helvetica" ) ) );
   // This is the column head.
   // this->SetFont( wxFont( wxFontInfo( 12 ).FaceName( "Helvetica" ).Bold() ) );
   wxString szColumn = m_szColumnName + " (" + std::to_string(avecItemData.size()) + ")";
   //this->InsertColumn( 0, m_szColumnName, wxLIST_FORMAT_LEFT, COLUMN_WIDTH );

   auto col = wxListItem();
   col.SetFont( wxFont( wxFontInfo( 12 ).FaceName( "Trebuchet MS" ).Bold() ).MakeBold() );
   col.SetColumn( 0 );
   col.SetText( szColumn );
   col.SetWidth( COLUMN_WIDTH-20 );
   col.SetAlign( wxLIST_FORMAT_LEFT );
   auto ints = this->InsertColumn( 0, col );
   

   //map<wxString, vector<GroupItemData*>, Group::Sorting> mapGroups( *aGrp.GetSortingFunctor() );
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
}

int 
vcCollectionCubeGroup::GetColumnIndex()
{
   return m_iColumnIndex;
}

void 
vcCollectionCubeGroup::DeselectAll()
{
   auto selectedItem = this->GetFirstSelected();
   while( selectedItem != -1 )
   {
      this->Select( selectedItem, false );
      selectedItem = this->GetNextSelected( selectedItem );
   }
}

int 
vcCollectionCubeGroup::GetSelection()
{
   return m_iSelection;
}

void
vcCollectionCubeGroup::onItemSelection( wxListEvent& awxEvt )
{
   m_iSelection = awxEvt.GetIndex();
   if( !m_vecHashes[m_iSelection].IsEmpty() )
   {
      awxEvt.SetInt( m_iColumnIndex );
      awxEvt.SetString( m_vecHashes[m_iSelection] );
      awxEvt.Skip();
   }
}

void
vcCollectionCubeGroup::onItemDeselection( wxListEvent& awxEvt )
{
   m_iSelection = -1;
   awxEvt.StopPropagation();
}

void 
vcCollectionCubeGroup::performDisplay(
   map<wxString, vector<GroupItemData*>, Group::Sorting> amapGrps, const Group& aGrp )
{
   for( auto groupData : amapGrps )
   {
      if( !aGrp.IsEmpty() )
      {
         //this->SetFont( wxFont( wxFontInfo( 8 ).FaceName( "Helvetica" ) ).Bold() );
         wxString buf = groupData.first + " (" + std::to_string( groupData.second.size() ) + ")";
         int i = this->GetItemCount();
         long tmp = this->InsertItem( i, buf, 0 );
         this->SetItemData( tmp, i );
         this->SetItemFont( i, wxFont( wxFontInfo( 9 ).FaceName( "Trebuchet MS" ) ).Bold() );
         m_vecHashes.push_back( "" );
         //this->SetFont( wxFont( wxFontInfo( 8 ).FaceName( "Helvetica" ) ) );
      }

      for( auto& itemData : groupData.second )
      {
         displayNormal( itemData );
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
   int i = this->GetItemCount();
   long tmp = this->InsertItem( i, buf, 0 );
   this->SetItemData( tmp, i );
   this->SetItemFont( i, wxFont( wxFontInfo( 9 ).FaceName( "Trebuchet MS" ) ) );
   m_vecHashes.push_back( aszHash );
}

void 
vcCollectionCubeGroup::performDisplayWithSubGrouping( 
   map<wxString, vector<GroupItemData*>, Group::Sorting> amapGrps,
   const Group& aGrp )
{
   Group subGroup = aGrp.GetSubGroup( "" );

   for( auto groupData : amapGrps )
   {
      map<wxString, set<GroupItemData*, Group::ItemSorting>, Group::Sorting>
         mapSubGrpSorting( *subGroup.GetSortingFunctor() );
      for( auto& itemData : groupData.second )
      {
         wxString szSubGrp = subGroup.GetGroup( *itemData );
         mapSubGrpSorting[szSubGrp].insert( itemData );
      }

      if( !aGrp.IsEmpty() )
      {
         //this->SetFont( wxFont( wxFontInfo( 8 ).FaceName( "Helvetica" ) ).Bold() );
         wxString buf = groupData.first + " (" + std::to_string( groupData.second.size() ) + ")";
         int i = this->GetItemCount();
         long tmp = this->InsertItem( i, buf, 0 );
         this->SetItemData( tmp, i );
         this->SetItemFont( i, wxFont( wxFontInfo( 9 ).FaceName( "Trebuchet MS" ) ).Bold() );
         m_vecHashes.push_back( "" );
         //this->SetFont( wxFont( wxFontInfo( 8 ).FaceName( "Helvetica" ) ) );
      }

      bool bFirst = true;
      for( auto& itemSubGrpData : mapSubGrpSorting )
      {
         if( !bFirst )
         {
            // Space in between each subgroup.
            displayNormal( "", "" );
         }

         bFirst = false;
         for( auto& itemData : itemSubGrpData.second )
         {
            displayNormal( itemData );
         }
      }

      // Space in between each subgroup.
      displayNormal( "", "" );
   }
}