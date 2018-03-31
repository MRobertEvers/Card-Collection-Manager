#include "vcCollectionCubeDisplay.h"
#include "vcCollectionCubeGroup.h"
#include "../MainWindow/MainWindow.h"
#include "../StoreFrontEnd/StoreFrontEnd.h"
#include "../StoreFrontEnd/CardInterface.h"
#include "../StoreFrontEnd/CollectionInterface.h"
#include <algorithm>
#include <wx/gdicmn.h>

bool 
CubeDisplayColumnSorter::operator()( const wxString& agrpLeft, const wxString& agrpRight ) const
{
   std::vector<wxString> vecGrpOrder = { "W", "U", "B", "R", "G", "Multicolor", "Colorless" };

   auto iter_begin = vecGrpOrder.begin();
   auto iter_end = vecGrpOrder.end();
   
   int iLeft = std::distance( iter_begin, std::find( iter_begin, iter_end, agrpLeft ) );
   int iRight = std::distance( iter_begin, std::find( iter_begin, iter_end, agrpRight ) );

   return iLeft == iRight ? agrpLeft < agrpRight : iLeft < iRight;
}
bool
CubeDisplayItemSorter::operator()( const wxString& agrpLeft, const wxString& agrpRight ) const
{
   // Expects CMC
   unsigned int iNumsLeft;
   int iLeft;
   unsigned int iNumsRight;
   int iRight;

   iLeft = std::stoi( agrpLeft.ToStdString(), &iNumsLeft );
   iRight = std::stoi( agrpRight.ToStdString(), &iNumsRight );

   if( iNumsLeft <= 0 )
   {
      iLeft = 0;
   }

   if( iNumsRight <= 0 )
   {
      iRight = 0;
   }

   return iLeft < iRight;
}

wxBEGIN_EVENT_TABLE( vcCollectionCubeDisplay, wxPanel )
wxEND_EVENT_TABLE()

vcCollectionCubeDisplay::vcCollectionCubeDisplay( wxPanel* aptParent, wxWindowID aiWID, 
                                                  std::shared_ptr<CollectionInterface> aptInt )
   : wxScrolledWindow( aptParent, aiWID, wxDefaultPosition, wxDefaultSize, wxLB_ALWAYS_SB ), m_ptColInt( aptInt )
{
   wxBoxSizer* boxSizer = new wxBoxSizer( wxHORIZONTAL );
   this->SetSizer( boxSizer );
   RefreshList();
   this->SetScrollRate( 10, 20 );
}

vcCollectionCubeDisplay::~vcCollectionCubeDisplay()
{

}

void 
vcCollectionCubeDisplay::RefreshList()
{
   m_ptColInt->Refresh();

   auto defGroup = defaultGroup();
   auto mapGroups = performGrouping( m_ptColInt->GetItemInterfaces(), defGroup );
   this->Freeze();
   clearDisplay();
   int index = 0;
   for( auto& grp : mapGroups )
   {
      auto grpList = new vcCollectionCubeGroup( this, Group_List, grp.first, index );
      grpList->SetSubSectionFontColor( getGroupFontColor( grp.first ) );
      grpList->SetBackgroundColor( getGroupColor( grp.first ) );
      m_mapColGroups.insert( std::make_pair( index++, grpList ) );

      grpList->PopulateList( grp.second, defGroup.GetSubGroup( grp.first ) );

      this->GetSizer()->Add(grpList, wxSizerFlags(1).Left().Border(wxRIGHT, 3));
   }
   this->Layout();
   this->Thaw();
}

CardInterface 
vcCollectionCubeDisplay::GetItemByListIndex( int Ind )
{
   return m_ptColInt->GetItemInterfaces()[Ind];
}

CardInterface
vcCollectionCubeDisplay::GetFirst()
{
   if( m_ptColInt->GetItemInterfaces().size() > 0 )
   {
      return *m_ptColInt->GetItemInterfaces().begin();
   }
   else
   {
      return CardInterface();
   }
}

int 
vcCollectionCubeDisplay::GetFirstInt()
{
   return 0;
}

bool 
vcCollectionCubeDisplay::IsEmpty()
{
   return m_ptColInt->GetItemInterfaces().size() == 0;
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
             .SetGroupSortingFunctor( new CubeDisplayColumnSorter() )
             .AliasGroup( "White", "W" )
             .AliasGroup( "Blue", "U" )
             .AliasGroup( "Black", "B" )
             .AliasGroup( "Red", "R" )
             .AliasGroup( "Green", "G" )
             .BroadenGroup( "::" )
             .AliasGroup( "::", "Multicolor" )
             .AliasGroup( "", "Colorless" );

   Group subGroup;
   subGroup.GroupOn( "colors", false );
   subGroup.AliasGroup( "White::Blue", "Azorius" )
           .AliasGroup( "White::Black", "Orzhov" )
           .AliasGroup( "White::Red", "Boros" )
           .AliasGroup( "White::Green", "Selesnya" )
           .AliasGroup( "Blue::Black", "Dimir" )
           .AliasGroup( "Blue::Red", "Izzet" )
           .AliasGroup( "Blue::Green", "Simic" )
           .AliasGroup( "Black::Red", "Rakdos" )
           .AliasGroup( "Black::Green", "Golgari" )
           .AliasGroup( "Red::Green", "Gruul" );

   defaultGrp.AddSubGroup( "Multicolor", subGroup );

   Group defaultSubGroup;
   defaultSubGroup.GroupOn("type", false)
                  .BroadenGroup("Creature")
                  .BroadenGroup("Instant")
                  .BroadenGroup("Sorcery")
                  .BroadenGroup("Enchantment")
                  .BroadenGroup("Artifact")
                  .BroadenGroup("Planeswalker")
                  .BroadenGroup("Land");

   Group defaultSubGroupOrdering;
   defaultSubGroupOrdering.GroupOn("cmc", false)
                          .SetGroupSortingFunctor( new CubeDisplayItemSorter() );

   defaultSubGroup.SetDefaultSubGroup( defaultSubGroupOrdering );

   defaultGrp.SetDefaultSubGroup( defaultSubGroup );

   return defaultGrp;
}

wxColour 
vcCollectionCubeDisplay::getGroupColor( const wxString& aszGroup )
{
   if( aszGroup == "W" )
   {
      return wxColour( 255, 252, 243 );
   }
   else if( aszGroup == "U" )
   {
      return wxColour( 235, 245, 255 );
   }
   else if( aszGroup == "B" )
   {
      return wxColour( 227, 227, 227 );
   }
   else if( aszGroup == "R" )
   {
      return wxColour( 255, 231, 231 );
   }
   else if( aszGroup == "G" )
   {
      return wxColour( 242, 255, 230 );
   }
   else if( aszGroup == "Multicolor" )
   {
      return wxColour( 255, 255, 220 );
   }
   else if( aszGroup == "Colorless" )
   {
      return wxColour( 255, 241, 228 );
   }

   return wxColour();
}

wxColour 
vcCollectionCubeDisplay::getGroupFontColor( const wxString& aszGroup )
{
   if( aszGroup == "W" )
   {
      return wxColour( 185, 159, 100 );
   }
   else if( aszGroup == "U" )
   {
      return wxColour( 0, 77, 153 );
   }
   else if( aszGroup == "B" )
   {
      return wxColour( 0, 0, 0 );
   }
   else if( aszGroup == "R" )
   {
      return wxColour( 153, 0, 0 );
   }
   else if( aszGroup == "G" )
   {
      return wxColour( 77, 153, 0 );
   }
   else if( aszGroup == "Multicolor" )
   {
      return wxColour( 179, 153, 0 );
   }
   else if( aszGroup == "Colorless" )
   {
      return wxColour( 153, 77, 0 );
   }

   return wxColour();
}

std::map<wxString, std::vector<CardInterface*>, Group::Sorting>
vcCollectionCubeDisplay::performGrouping( std::vector<CardInterface>& avecItems, const Group& aGrp )
{
   map<wxString, vector<CardInterface*>, Group::Sorting> mapGroups( *aGrp.GetSortingFunctor() );

   // So you assign each item in your collection to a GroupItemData,
   // then the do something like below to extract the group that they should belong.
   for( auto& data : avecItems )
   {
      // TODO: Need way to get meta tags from server so we can sort on them.
      auto szGroup = aGrp.GetGroup( data );
      mapGroups[szGroup].push_back( &data );
   }

   return mapGroups;
}