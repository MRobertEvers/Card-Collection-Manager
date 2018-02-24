#include "vcCollectionCubeDisplay.h"
#include "vcCollectionCubeGroup.h"
#include "MainWindow.h"
#include "StoreFrontEnd.h"
#include "Group.h"
#include <algorithm>

bool 
CubeDisplayPrimarySorter::operator()( const wxString& agrpLeft, const wxString& agrpRight ) const
{
   std::vector<wxString> vecGrpOrder = { "W", "U", "B", "R", "G" };

   auto iter_begin = vecGrpOrder.begin();
   auto iter_end = vecGrpOrder.end();
   
   int iLeft = std::distance( iter_begin, std::find( iter_begin, iter_end, agrpLeft ) );
   int iRight = std::distance( iter_begin, std::find( iter_begin, iter_end, agrpRight ) );

   return iLeft < iRight;
}

vcCollectionCubeDisplay::vcCollectionCubeDisplay( wxPanel* aptParent, wxWindowID aiWID, const wxString& aszColID )
   : wxPanel( aptParent, aiWID ), m_szColID(aszColID)
{
   wxBoxSizer* boxSizer = new wxBoxSizer( wxHORIZONTAL );
   this->SetSizer( boxSizer );
   RefreshList();
}

vcCollectionCubeDisplay::~vcCollectionCubeDisplay()
{

}

void 
vcCollectionCubeDisplay::RefreshList()
{
   auto ptSF = StoreFrontEnd::Instance();
   Query query;
   query.AnyMeta();
   auto lstCol = ptSF->GetAllCardsStartingWith( m_szColID.ToStdString(), query );

   m_vecDataItems.clear();
   for( auto& szItem : lstCol )
   {
      GroupItemData data( szItem, GroupItemData::LONG_NAME );
      m_vecDataItems.push_back( data );
   }

   auto defGroup = defaultGroup();
   auto mapGroups = performGrouping( m_vecDataItems, defGroup );
   this->Freeze();
   clearDisplay();
   int index = 0;
   for( auto& grp : mapGroups )
   {
      auto grpList = new vcCollectionCubeGroup( this, Group_List, grp.first, index );
      m_mapColGroups.insert( std::make_pair( index++, grpList ) );

      grpList->PopulateList( grp.second );

      this->GetSizer()->Add(grpList, wxSizerFlags(0).Left().Expand());
   }
   PostSizeEvent();
   this->Thaw();
}

GroupItemData 
vcCollectionCubeDisplay::GetItemByListIndex( int Ind )
{
   return m_vecDataItems[Ind];
}

GroupItemData 
vcCollectionCubeDisplay::GetFirst()
{
   return m_vecDataItems[0];
}

int 
vcCollectionCubeDisplay::GetFirstInt()
{
   return 0;
}

bool 
vcCollectionCubeDisplay::IsEmpty()
{
   return false;
}

void 
vcCollectionCubeDisplay::clearDisplay()
{
   for( auto& item : m_mapColGroups )
   {
      this->GetSizer()->Detach( item.second );
      item.second->Destroy();
   }
   m_mapColGroups.clear();
}

Group 
vcCollectionCubeDisplay::defaultGroup()
{
   Group defaultGrp;
   defaultGrp.GroupOn( "colors", false )
             .SetSortingFunctor( new CubeDisplayPrimarySorter() )
             .AliasGroup( "Red", "R" )
             .AliasGroup( "White", "W" )
             .AliasGroup( "Blue", "U" )
             .AliasGroup( "Black", "B" )
             .AliasGroup( "Green", "G" );

   Group subGroup;

   return defaultGrp;
}

std::map<wxString, std::vector<GroupItemData*>, Group::Sorting>
vcCollectionCubeDisplay::performGrouping( std::vector<GroupItemData>& avecItems, const Group& aGrp )
{
   map<wxString, vector<GroupItemData*>, Group::Sorting> mapGroups( *aGrp.GetSortingFunctor() );

   // So you assign each item in your collection to a GroupItemData,
   // then the do something like below to extract the group that they should belong.
   for( auto& data : avecItems )
   {
      // TODO: Need way to get meta tags from server so we can sort on them.
      mapGroups[aGrp.GetGroup( data )].push_back( &data );
   }

   return mapGroups;
}