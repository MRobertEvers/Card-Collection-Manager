#include "vcCollectionCubeGroup.h"
#include "MainWindow.h"
#include "wx/wxprec.h"
#include "Group.h"
#include "GroupItemData.h"
#include "vcCollectionCubeDisplay.h"
#include <algorithm>

#define COLUMN_WIDTH 120

wxBEGIN_EVENT_TABLE( vcCollectionCubeGroup, wxPanel )
EVT_LIST_ITEM_SELECTED( vcCollectionCubeDisplay::Group_List, vcCollectionCubeGroup::onItemSelection )
EVT_LIST_ITEM_DESELECTED( vcCollectionCubeDisplay::Group_List, vcCollectionCubeGroup::onItemDeselection )
wxEND_EVENT_TABLE()

vcCollectionCubeGroup::vcCollectionCubeGroup( wxPanel* aptParent,
                                              wxWindowID aiWID,
                                              wxString aszColumn,
                                              int aiColumnIndex )
   : wxListView(aptParent, aiWID), m_iSelection(-1), m_iColumnIndex(aiColumnIndex)
{
   wxFlexGridSizer* boxSizer = new wxFlexGridSizer( 1, 1, 0, 0 );

   boxSizer->AddGrowableCol( 0 );
   // Order is Count, Name*, Mana Cost, Card Type
   this->SetSizer( boxSizer );

   this->SetFont( wxFont( wxFontInfo( 8 ).FaceName( "Helvetica" ) ) );
   this->SetSizeHints( wxSize( COLUMN_WIDTH, wxDefaultSize.GetHeight() ) );
   this->InsertColumn( 0, aszColumn, wxLIST_FORMAT_LEFT, COLUMN_WIDTH );
}

vcCollectionCubeGroup::~vcCollectionCubeGroup()
{

}

void 
vcCollectionCubeGroup::PopulateList( std::vector<GroupItemData*> avecItemData )
{
   for( auto itemData : avecItemData )
   {
      wxString buf = itemData->GetName();
      int i = this->GetItemCount();
      long tmp = this->InsertItem( i, buf, 0 );
      this->SetItemData( tmp, i );
      m_vecHashes.push_back( itemData->GetHash() );
   }
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
   awxEvt.SetInt( m_iColumnIndex );
   awxEvt.SetString( m_vecHashes[m_iSelection] );
   awxEvt.Skip();
}

void
vcCollectionCubeGroup::onItemDeselection( wxListEvent& awxEvt )
{
   m_iSelection = -1;
   awxEvt.StopPropagation();
}