#include "CubeRenderer.h"
#include "../StoreFrontEnd/CardInterface.h"
#include <algorithm>

using namespace std;

class CubeDisplayColumnSorter : public  Group::SortingOperator
{
public:
   bool operator()( const wxString& agrpLeft, const wxString& agrpRight ) const;
};

class CubeDisplayItemSorter : public  Group::SortingOperator
{
public:
   bool operator()( const wxString& agrpLeft, const wxString& agrpRight ) const;
};

bool
CubeDisplayColumnSorter::operator()( const wxString& agrpLeft, const wxString& agrpRight ) const
{
   vector<wxString> vecGrpOrder = { "W", "U", "B", "R", "G", "Multicolor", "Colorless" };

   auto iter_begin = vecGrpOrder.begin();
   auto iter_end = vecGrpOrder.end();

   int iLeft = distance( iter_begin, find( iter_begin, iter_end, agrpLeft ) );
   int iRight = distance( iter_begin, find( iter_begin, iter_end, agrpRight ) );

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

   iLeft = stoi( agrpLeft.ToStdString(), &iNumsLeft );
   iRight = stoi( agrpRight.ToStdString(), &iNumsRight );

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



CubeRenderer::CubeRenderer( wxPanel* aParent, wxWindowID aiWID )
   : wxScrolledWindow( aParent, aiWID ), m_Grouping(Group(true))
{
   wxBoxSizer* boxSizer = new wxBoxSizer( wxHORIZONTAL );
   this->SetSizer( boxSizer );
   this->SetScrollRate( 10, 20 );
}

CubeRenderer::~CubeRenderer()
{
   // Wxwidgets will delete
   //for( auto column : m_mapColumns )
   //{
   //   delete column.second;
   //}
}

void
CubeRenderer::Draw( std::vector<std::shared_ptr<IRendererItem>> avecItemData )
{
   this->Freeze();
   for( auto& existing : m_mapColumns )
   {
      existing.second->Destroy();
   }
   m_mapColumns.clear();

   uiBuildColumns( avecItemData );
  
   InitRenderer( avecItemData );
   this->Thaw();
}

// Returns true if the item was successfully removed.
bool
CubeRenderer::RemoveItem( std::shared_ptr<IRendererItem> aptItem )
{
   auto szGroup = GetGrouping().GetGroup( *aptItem );
   auto iter_findgroup = m_mapColumns.find( szGroup );
   if( iter_findgroup != m_mapColumns.end() )
   {
      ItemRemoved( aptItem );
      return iter_findgroup->second->RemoveItem( aptItem );
   }
   else
   {
      return false;
   }
}

void
CubeRenderer::AddItem( std::shared_ptr<IRendererItem> aptItem )
{
   auto szGroup = GetGrouping().GetGroup( *aptItem );
   auto iter_findgroup = m_mapColumns.find( szGroup );
   if( iter_findgroup != m_mapColumns.end() )
   {
      iter_findgroup->second->AddItem( aptItem );
   }
   else
   {
      uiAddColumn( szGroup, { aptItem } );
   }
   ItemAdded( aptItem );
}

Group 
CubeRenderer::GetGrouping()
{
   if( m_Grouping.IsEmpty() )
   {
      uiBuildGrouping();
   }

   return m_Grouping;
}

ColoredGroupColumnRenderer*
CubeRenderer::uiGetColumnRenderer( const wxString& aszGroupName, const Group& aGroup )
{
   ColoredGroupColumnRenderer* ptRenderer;

   if( aszGroupName == "Multicolor" )
   {
      ptRenderer = new MultiDistinctGroupColumnRenderer( this, wxID_ANY, aGroup );
   }
   else
   {
      ptRenderer = new OrderedSubgroupColumnRenderer( this, wxID_ANY, aGroup );
   }

   ptRenderer->SetBackgroundColor( uiGetColumnColor( aszGroupName ) );
   ptRenderer->SetTitle( aszGroupName );
   return ptRenderer;
}

void
CubeRenderer::uiBuildColumns( vector<std::shared_ptr<IRendererItem>> avecItems )
{
   map<wxString, vector<std::shared_ptr<IRendererItem>>, Group::Sorting> mapGroups( *GetGrouping().GetSortingFunctor() );

   for( auto& data : avecItems )
   {
      auto szGroup = GetGrouping().GetGroup( *data );
      mapGroups[szGroup].push_back( data );
   }

   for( auto& group : mapGroups )
   {
      uiAddColumn( group.first, group.second );
   }

   Layout();
}

void 
CubeRenderer::uiAddColumn( const wxString& aszColumnName, vector<std::shared_ptr<IRendererItem>> avecItemData )
{
   auto grouping = GetGrouping();
   auto ptColumn = uiGetColumnRenderer( aszColumnName, grouping.GetSubGroup(aszColumnName) );
   m_mapColumns.insert( make_pair( aszColumnName, ptColumn) );

   this->GetSizer()->Add( ptColumn, wxSizerFlags( 1 ).Left().Border( wxRIGHT, 3 ) );

   // Draw the column here.
   ptColumn->Draw( avecItemData );
}

void
CubeRenderer::uiBuildGrouping()
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


   Group defaultSubGroup;
   defaultSubGroup.GroupOn( "type", false )
      .BroadenGroup( "Creature" )
      .BroadenGroup( "Instant" )
      .BroadenGroup( "Sorcery" )
      .BroadenGroup( "Enchantment" )
      .BroadenGroup( "Artifact" )
      .BroadenGroup( "Planeswalker" )
      .BroadenGroup( "Land" );

   Group defaultSubGroupOrdering;
   defaultSubGroupOrdering.GroupOn( "cmc", false )
      .SetGroupSortingFunctor( new CubeDisplayItemSorter() );

   Group metatagColorOverrideGroup;
   metatagColorOverrideGroup.GroupOn( "colors" )
      .SetGroupSortingFunctor( new CubeDisplayColumnSorter() )
      .AliasGroup( "White", "W" )
      .AliasGroup( "Blue", "U" )
      .AliasGroup( "Black", "B" )
      .AliasGroup( "Red", "R" )
      .AliasGroup( "Green", "G" )
      .BroadenGroup( "::" )
      .AliasGroup( "::", "Multicolor" );
   // TODO: Override subgroups have no good way to be chosen.
   // Just add this subgroup to each
   // metatagColorOverrideGroup.AddSubGroup( "Multicolor", subGroup );

   Group subGroupOverride;
   subGroupOverride.GroupOn( "colors" );
   subGroupOverride.AliasGroup( "White::Blue", "Azorius" )
      .AliasGroup( "White::Black", "Orzhov" )
      .AliasGroup( "White::Red", "Boros" )
      .AliasGroup( "White::Green", "Selesnya" )
      .AliasGroup( "Blue::Black", "Dimir" )
      .AliasGroup( "Blue::Red", "Izzet" )
      .AliasGroup( "Blue::Green", "Simic" )
      .AliasGroup( "Black::Red", "Rakdos" )
      .AliasGroup( "Black::Green", "Golgari" )
      .AliasGroup( "Red::Green", "Gruul" );

   subGroup.OverrideGrouping( subGroupOverride );
    
   defaultGrp.AddSubGroup( "Multicolor", subGroup );
   defaultGrp.OverrideGrouping( metatagColorOverrideGroup );

   defaultSubGroup.SetDefaultSubGroup( defaultSubGroupOrdering );

   defaultGrp.SetDefaultSubGroup( defaultSubGroup );

   m_Grouping = defaultGrp;
}

wxColour 
CubeRenderer::uiGetColumnColor( const wxString& aszColumnName )
{
   if( aszColumnName == "W" )
   {
      return wxColour( 255, 252, 243 );
   }
   else if( aszColumnName == "U" )
   {
      return wxColour( 235, 245, 255 );
   }
   else if( aszColumnName == "B" )
   {
      return wxColour( 227, 227, 227 );
   }
   else if( aszColumnName == "R" )
   {
      return wxColour( 255, 231, 231 );
   }
   else if( aszColumnName == "G" )
   {
      return wxColour( 242, 255, 230 );
   }
   else if( aszColumnName == "Multicolor" )
   {
      return wxColour( 255, 255, 220 );
   }
   else if( aszColumnName == "Colorless" )
   {
      return wxColour( 255, 241, 228 );
   }

   return wxColour();
}


wxBEGIN_EVENT_TABLE( ColoredGroupColumnRenderer, wxInfiniteGrid )
EVT_SIZE( ColoredGroupColumnRenderer::onResize )
EVT_GRID_CELL_LEFT_CLICK( ColoredGroupColumnRenderer::onItemClicked )
wxEND_EVENT_TABLE()

ColoredGroupColumnRenderer::ColoredGroupColumnRenderer( wxPanel* aParent, wxWindowID aiWID,
                                                        const Group& aGroup )
   : wxInfiniteGrid( aParent, aiWID, wxDefaultPosition, wxDefaultSize, wxBORDER ), m_Group(aGroup)
{
   this->ShowScrollbars( wxScrollbarVisibility::wxSHOW_SB_NEVER, wxScrollbarVisibility::wxSHOW_SB_NEVER );
   this->CreateGrid( 0, 1 );
   this->DisableDragColSize();
   this->DisableDragRowSize();
   this->HideRowLabels();
   this->EnableGridLines( false );
}

ColoredGroupColumnRenderer::~ColoredGroupColumnRenderer()
{
}

void 
ColoredGroupColumnRenderer::SetTitle( const wxString& aszTitle )
{
   m_szGroupTitle = aszTitle;
}

void 
ColoredGroupColumnRenderer::SetBackgroundColor( const wxColour& aColour )
{
   m_ColumnColour = aColour;
}

void 
ColoredGroupColumnRenderer::SetFontColor( const wxColour& aColour )
{
   m_GeneralFontColor = aColour;
}


int
ColoredGroupColumnRenderer::DisplayItem( const wxString& buf, const wxString& aszHash,
                                         const wxFont& awxFont,
                                         bool abAlignCenter,
                                         const wxColour& awxFontColor,
                                         const wxColour& awxBackgroundColor,
                                         int aiRow,
                                         unsigned int aiRowHeight )
{
   int iRow;
   if( aiRow == -1 )
   {
      this->GetNumberRows();
   }
   else
   {
      iRow = aiRow;
   }
   this->InsertRows( iRow );
   this->SetCellValue( iRow, 0, buf );
   this->SetCellFont( iRow, 0, awxFont );
   this->SetReadOnly( iRow, 0, true );
   this->SetCellBackgroundColour( iRow, 0, awxBackgroundColor );
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

   return iRow;
}

void 
ColoredGroupColumnRenderer::UndisplayItem( int aiRow )
{
   this->DeleteRows( aiRow );
}

wxColour 
ColoredGroupColumnRenderer::GetBackgroundColor()
{
   return m_ColumnColour;
}

void
ColoredGroupColumnRenderer::onResize( wxSizeEvent& awxEvt )
{
   this->SetColSize( 0, this->GetVirtualSize().GetWidth() );
   awxEvt.Skip();
}

void 
ColoredGroupColumnRenderer::onItemClicked( wxGridEvent& awxEvt )
{
   //auto arrCells = this->GetSelectedCells();
   //for( size_t i = 0; i < GetNumberRows(); i++ )
   //{
   //   this->DeselectCell( i, 0 );
   //}
   awxEvt.Skip();
}

DisplayGroup::DisplayGroup( ColumnRenderer* apRenderer, DisplayNodeSource* apSource, wxString aszGroupName, Group aGroup,
                            std::vector<std::shared_ptr<IRendererItem>> avecItems, DisplayGroup* aParent )
   : m_pRenderer( apRenderer ), m_pSource(apSource), m_Group(aGroup), m_Parent(aParent), m_szGroupName(aszGroupName)
{
   if( !aGroup.IsEmpty() )
   {
      auto tmpMap = std::map<wxString, std::vector<std::shared_ptr<IRendererItem>>, Group::Sorting>( *aGroup.GetSortingFunctor() );
      m_mapChildren = std::map<wxString, DisplayGroup*, Group::Sorting>( *aGroup.GetSortingFunctor() );

      for( auto& data : avecItems )
      {
         // TODO: Need way to get meta tags from server so we can sort on them.
         auto szGroup = aGroup.GetGroup( *data );
         if( !aGroup.GetSubGroup( szGroup ).IsEmpty() || szGroup != "" )
         {
            tmpMap[szGroup].push_back( data );
         }
         else if( szGroup == "" )
         {
            // Defaults to sort alphabetically.
            m_setItems.insert( data );
         }
      }

      for( auto& group : tmpMap )
      {
         // Children Nodes.
         DisplayGroup* node = m_pSource->GetDisplayGroup(GetLevel()+1, group.first, aGroup.GetSubGroup( group.first ), group.second, this);
         m_mapChildren[group.first] = node;
      }
   }
   else
   {
      for( auto& data : avecItems )
      {
         m_setItems.insert( data );
      }
   }
}

DisplayGroup::~DisplayGroup()
{
   for( auto& child : m_mapChildren )
   {
      delete child.second;
   }
}

bool 
DisplayGroup::RemoveItem( std::shared_ptr<IRendererItem> aptItem )
{
   if( m_setItems.size() > 0 )
   {
      auto iter_find = m_setItems.find( aptItem );
      if( iter_find != m_setItems.end() )
      {
         int iPos = distance( m_setItems.begin(), iter_find );
         m_mapDrawnItems.erase( *iter_find );
         m_setItems.erase( iter_find );
         m_pRenderer->UndisplayItem( GetFirstItemRow() + iPos );
         return true;
      }
      else
      {
         return false;
      }
   }
   else
   {
      bool bDidRemove = false;
      bool bDidErase = false;
      wxString szBefore = "";
      wxString szAfter = "";
      auto iter_child = m_mapChildren.begin();
      for( ; iter_child != m_mapChildren.end(); iter_child++ )
      {
         auto& children = *iter_child;
         if( children.second->RemoveItem( aptItem ) )
         {
            // TODO: Check for empty child
            if( children.second->IsEmpty() )
            {
               children.second->UnDraw();

               // Store the following element
               auto next = iter_child;
               next++;
               if( next != m_mapChildren.end() )
               {
                  szAfter = next->first;
               }
               bDidErase = true;
               m_mapChildren.erase( children.first );
            }
            bDidRemove = true;
            break;
         }
         szBefore = iter_child->first;
      }

      if( bDidErase && szAfter == "" && szBefore != "" )
      {
         m_mapChildren[szBefore]->UnDraw();
         m_mapChildren[szBefore]->Draw();
      }

      if( bDidErase && szBefore == "" && szAfter != "" )
      {
         m_mapChildren[szAfter]->UnDraw();
         m_mapChildren[szAfter]->Draw();
      }


      return bDidRemove;
   }
}

bool 
DisplayGroup::AddItem( std::shared_ptr<IRendererItem> aptItem )
{
   if( !m_Group.IsEmpty() )
   {

      auto szGroup = m_Group.GetGroup( *aptItem );
      if( !szGroup.IsEmpty() )
      {
         // Do we have a group for this?
         auto iter_sub = m_mapChildren.find( szGroup );
         if( iter_sub != m_mapChildren.end() )
         {
            return iter_sub->second->AddItem( aptItem );
         }
         else
         {
            auto subGroup = m_Group.GetSubGroup( szGroup );
            DisplayGroup* node = m_pSource->GetDisplayGroup( GetLevel()+1, szGroup, subGroup, { aptItem }, this );
            auto iter_insert = m_mapChildren.insert( std::make_pair( szGroup, node ) );
            node->Draw();
            // Redo siblings.
            // TODO: maybe we should just redraw adjacent siblings for generality...
            if( iter_insert.first == m_mapChildren.begin() )
            {
               if( ++iter_insert.first != m_mapChildren.end() )
               {
                  iter_insert.first->second->UnDraw();
                  iter_insert.first->second->Draw();
               }
            }
            else if( iter_insert.first->first == m_mapChildren.rbegin()->first && m_mapChildren.size() > 1)
            {
               iter_insert.first--;
               iter_insert.first->second->UnDraw();
               iter_insert.first->second->Draw();
            }
            return true;
         }
      }
      else
      {
         return false;
      }
   }
   else
   {
      auto iter_insert = m_setItems.insert( aptItem );
      int iPos = distance( m_setItems.begin(), iter_insert );
      int iInsert = GetFirstItemRow() + iPos;
      m_pRenderer->DisplayItem( aptItem->GetName(), aptItem->GetHash(),
                                wxFont(), false, wxColour(),
                                m_pRenderer->GetBackgroundColor(),
                                iInsert );
      m_mapDrawnItems[*iter_insert] = true;
      // Redraw is not needed for a simple insert.
      return true;
   }
}

std::shared_ptr<IRendererItem> 
DisplayGroup::GetItem( unsigned int auiItemRow )
{
   auto iFirstItem = (unsigned int)GetFirstItemRow();
   if( iFirstItem <= auiItemRow && auiItemRow < iFirstItem + GetDrawSize() )
   {
      if( m_setItems.size() > 0 )
      {
         int iIndex = iFirstItem;
         for( auto iter_item = m_setItems.begin(); 
              iter_item != m_setItems.end(); 
              ++iter_item, ++iIndex )
         {
            if( iIndex == auiItemRow )
            {
               return *iter_item;
            }
         }
      }
      else
      {
         for( auto& child : m_mapChildren )
         {
            auto pFound = child.second->GetItem(auiItemRow);
            if( pFound != nullptr )
            {
               return pFound;
            }
         }

         // If we haven't returned out yet, then we return nullptr below.
      }
   }

   return nullptr;
}

int 
DisplayGroup::GetSize()
{
   int iSize = 0;

   for( auto& child : m_mapChildren )
   {
      iSize += child.second->GetSize();
   }

   return iSize + m_setItems.size();
}

int 
DisplayGroup::GetDrawSize()
{
   int iSize = GetTotalOverhead();

   for( auto& child : m_mapChildren )
   {
      iSize += child.second->GetDrawSize();
   }
   return iSize + m_setItems.size();
}

int 
DisplayGroup::GetFirstRow()
{
   if( IsFirstChild() )
   {
      return m_Parent == nullptr ? 0 : m_Parent->GetFirstItemRow();
   }
   else
   {
      int iStart = m_Parent == nullptr ? 0 : m_Parent->GetFirstItemRow();
      for( auto& sibling : m_Parent->m_mapChildren )
      {
         if( sibling.second == this )
         {
            // Children should override this to add any overhead drawn before m_setItems.
            return iStart;
         }
         else
         {
            iStart += sibling.second->GetDrawSize();
         }
      }
   }
}

// Overload if overhead is draw prior to items
int 
DisplayGroup::GetFirstItemRow()
{
   return GetFirstRow();
}

bool 
DisplayGroup::IsEmpty()
{
   return m_mapChildren.size() == 0 && m_setItems.size() == 0;
}

bool 
DisplayGroup::IsLastChild()
{
   if( m_Parent == nullptr )
   {
      return true;
   }
   else
   {
      int index = 0;
      for( auto& sibling : m_Parent->m_mapChildren )
      {
         if( sibling.second == this )
         {
            return index == m_Parent->m_mapChildren.size() - 1;
         }
         index++;
      }
   }
}

bool 
DisplayGroup::IsFirstChild()
{
   if( m_Parent == nullptr )
   {
      return true;
   }
   else
   {
      int index = 0;
      for( auto& sibling : m_Parent->m_mapChildren )
      {
         if( sibling.second == this )
         {
            return index == 0;
         }
         index++;
      }
   }
}

int 
DisplayGroup::GetLevel()
{
   if( m_Parent == nullptr )
   {
      return 0;
   }
   else
   {
      return m_Parent->GetLevel() + 1;
   }
}

OrderedSubgroupColumnRenderer::
TypeGroup::TypeGroup( ColumnRenderer* apRenderer, 
                      DisplayNodeSource* apSource,
                      wxString aszGroupName,
                      Group aGroup,
                      vector<std::shared_ptr<IRendererItem>> avecItems,
                      DisplayGroup* aParent )
   : DisplayGroup( apRenderer, apSource, aszGroupName, aGroup, avecItems, aParent), m_bHasDrawnLast(false), m_bHasDrawnHeader(false)
{

}

OrderedSubgroupColumnRenderer::
TypeGroup::~TypeGroup()
{

}

bool 
OrderedSubgroupColumnRenderer::TypeGroup::RemoveItem( std::shared_ptr<IRendererItem> aptItem )
{
   if( DisplayGroup::RemoveItem( aptItem ) )
   {
      if( m_bHasDrawnHeader )
      {
         m_pRenderer->UndisplayItem( GetFirstRow() );
         wxString buf = m_szGroupName + " (" + std::to_string( GetSize() ) + ")";
         m_pRenderer->DisplayItem( buf, "",
                                   wxFont( wxFontInfo( 11 ).FaceName( "Trebuchet MS" ) ).Bold(),
                                   true, wxColour( "BLACK" ),
                                   m_pRenderer->GetBackgroundColor(), 
                                   GetFirstRow() );
         m_bHasDrawnHeader = true;
      }
      return true;
   }
   return false;
}

bool 
OrderedSubgroupColumnRenderer::TypeGroup::AddItem( std::shared_ptr<IRendererItem> aptItem )
{
   if( DisplayGroup::AddItem( aptItem ) )
   {
      if( m_bHasDrawnHeader )
      {
         m_pRenderer->UndisplayItem( GetFirstRow() );
         wxString buf = m_szGroupName + " (" + std::to_string( GetSize() ) + ")";
         m_pRenderer->DisplayItem( buf, "",
                                   wxFont( wxFontInfo( 11 ).FaceName( "Trebuchet MS" ) ).Bold(),
                                   true, wxColour( "BLACK" ),
                                   m_pRenderer->GetBackgroundColor(), 
                                   GetFirstRow() );
         m_bHasDrawnHeader = true;
      }
      return true;
   }
   return false;
}

int 
OrderedSubgroupColumnRenderer::
TypeGroup::GetDrawSize()
{
   // Since this class doesn't draw any items, the GetSize() function will return incorrectly.
   return DisplayGroup::GetDrawSize() - m_setItems.size();
}

void 
OrderedSubgroupColumnRenderer::
TypeGroup::Draw()
{
   if( !m_bHasDrawnHeader )
   {
      wxString buf = m_szGroupName + " (" + std::to_string( GetSize() ) + ")";
      m_pRenderer->DisplayItem( buf, "",
                                wxFont( wxFontInfo( 11 ).FaceName( "Trebuchet MS" ) ).Bold(),
                                true, wxColour( "BLACK" ),
                                m_pRenderer->GetBackgroundColor(), 
                                GetFirstRow() );
      m_bHasDrawnHeader = true;
   }

   // Draw subgroups
   vector<map<wxString, DisplayGroup*, Group::Sorting>::iterator> vecEmptyChildren;
   for( auto iter = m_mapChildren.begin();
        iter != m_mapChildren.end(); ++iter )
   {
      if( iter->second->IsEmpty() )
      {
         vecEmptyChildren.push_back( iter );
      }
      else
      {
         iter->second->Draw();
      }
   }

   // Delete empty subgroups.
   for( auto& empty : vecEmptyChildren )
   {
      m_mapChildren.erase( empty );
   }

   // Undraw all the end space
   if( m_bHasDrawnLast && IsLastChild() )
   {
      // Undraw endspace
      m_pRenderer->UndisplayItem( GetFirstRow() + GetDrawSize() );
      m_bHasDrawnLast = false;
   }
   else if( !m_bHasDrawnLast && !IsLastChild() )
   {
      // Draw endspace
      // TODO: All these header things need to be updated to the right row...
      // Perhaps don't link on old spot but rather where it is expected to be.
      m_pRenderer->DisplayItem( "", "",
                                wxFont(), false, wxColour(),
                                m_pRenderer->GetBackgroundColor(), 
                                GetFirstRow() + GetDrawSize() );
      m_bHasDrawnLast = true;
   }
}

void 
OrderedSubgroupColumnRenderer::
TypeGroup::UnDraw()
{
   if( m_bHasDrawnHeader )
   {
      m_pRenderer->UndisplayItem( GetFirstRow() );
      m_bHasDrawnHeader = false;
   }

   if( m_bHasDrawnLast )
   {
      // Subtract one because GetDrawSize() counts the footer too.
      m_pRenderer->UndisplayItem( GetFirstRow() + GetDrawSize() - 1);
      m_bHasDrawnLast = false;
   }

   // Do these in reverse.
   for( auto iter_child = m_mapChildren.rbegin(); 
      iter_child != m_mapChildren.rend();
      iter_child++ )
   {
      iter_child->second->UnDraw();
   }
}

int
OrderedSubgroupColumnRenderer::
TypeGroup::GetFirstItemRow()
{
   int iFirstDrawnRow = GetFirstRow();
   if( m_bHasDrawnHeader )
   {
      iFirstDrawnRow += 1;
   }
   return iFirstDrawnRow;
}

int 
OrderedSubgroupColumnRenderer::
TypeGroup::GetTotalOverhead()
{
   int iHead = 0;

   if( m_bHasDrawnLast )
   {
      iHead++;
   }

   if( m_bHasDrawnHeader )
   {
      iHead++;
   }

   return iHead;
}

OrderedSubgroupColumnRenderer::
CMCGroup::CMCGroup( ColumnRenderer* apRenderer,
                    DisplayNodeSource* apSource,
                    wxString aszGroupName,
                    Group aGroup,
                    vector<std::shared_ptr<IRendererItem>> avecItems,
                    DisplayGroup* aParent )
   : DisplayGroup( apRenderer, apSource, aszGroupName, aGroup, avecItems, aParent), m_bHasDrawnFirst(false)
{

}

OrderedSubgroupColumnRenderer::
CMCGroup::~CMCGroup()
{

}

void 
OrderedSubgroupColumnRenderer::
CMCGroup::Draw()
{
   if( m_bHasDrawnFirst && IsFirstChild() )
   {
      // Undraw space
      m_pRenderer->UndisplayItem( GetFirstRow() );
      m_bHasDrawnFirst = false;
   }
   else if( !m_bHasDrawnFirst && !IsFirstChild() )
   {
      // Draw space.
       m_pRenderer->DisplayItem( "", "",
                                 wxFont(), false, wxColour(),
                                 m_pRenderer->GetBackgroundColor(), 
                                 GetFirstRow() );
       m_bHasDrawnFirst = true;
   }

   int iRow = GetFirstItemRow();
   for( auto& child : m_setItems )
   {
      m_pRenderer->DisplayItem( child->GetName(), child->GetHash(),
                                wxFont(), false, wxColour(),
                                m_pRenderer->GetBackgroundColor(),
                                iRow );
      m_mapDrawnItems[child] = true;
      iRow++;
   }
}

void 
OrderedSubgroupColumnRenderer::
CMCGroup::UnDraw()
{
   if( m_bHasDrawnFirst )
   {
      m_pRenderer->UndisplayItem( GetFirstRow() );
      m_bHasDrawnFirst = false;
   }

   // Collapse it like a dominoe
   for( int i = 0; i < m_mapDrawnItems.size(); i++ )
   {
      m_pRenderer->UndisplayItem( GetFirstRow() );
   }
   m_mapDrawnItems.clear();
}

int 
OrderedSubgroupColumnRenderer::
CMCGroup::GetFirstItemRow()
{
   int iFirstDrawnRow = GetFirstRow();
   if( m_bHasDrawnFirst )
   {
      iFirstDrawnRow += 1;
   }
   return iFirstDrawnRow;
}

int 
OrderedSubgroupColumnRenderer::
CMCGroup::GetTotalOverhead()
{
   if( m_bHasDrawnFirst )
   {
      return 1;
   }
   else
   {
      return 0;
   }
}

void
RootGroup::Draw()
{
   for( auto& child : m_mapChildren )
   {
      child.second->Draw();
   }
}

OrderedSubgroupColumnRenderer::OrderedSubgroupColumnRenderer( wxPanel* aParent,
                                                              wxWindowID aiWID, 
                                                              const Group& aGroup )
   : ColoredGroupColumnRenderer( aParent, aiWID, aGroup ) 
{

}

OrderedSubgroupColumnRenderer::~OrderedSubgroupColumnRenderer()
{
   delete m_Root;
}

void 
OrderedSubgroupColumnRenderer::Draw( vector<std::shared_ptr<IRendererItem>> avecItemData )
{
   m_Root = new RootGroup( this, this, m_Group, avecItemData );

   this->SetColLabelValue( 0, uiGetDisplayTitle() );
   m_Root->Draw();

   this->SetColSize( 0, this->GetVirtualSize().GetWidth() );
}

bool 
OrderedSubgroupColumnRenderer::RemoveItem( std::shared_ptr<IRendererItem> aptItem )
{
   this->Freeze();
   if( m_Root->RemoveItem( aptItem ) )
   {
      this->SetColLabelValue( 0, uiGetDisplayTitle() );
      this->Thaw();
      return true;
   }
   this->Thaw();
   return false;
}

void 
OrderedSubgroupColumnRenderer::AddItem( std::shared_ptr<IRendererItem> aptItem )
{
   this->Freeze();
   if( m_Root->AddItem( aptItem ) )
   {
      this->SetColLabelValue( 0, uiGetDisplayTitle() );
   }
   this->Thaw();
}

DisplayGroup* 
OrderedSubgroupColumnRenderer::GetDisplayGroup( int aiType,
                                                wxString aszGroupName,
                                                Group aGroup,
                                                std::vector<std::shared_ptr<IRendererItem>> avecItems,
                                                DisplayGroup* aParent )
{
   if( aiType == 1 )
   {
      return new TypeGroup( this, this, aszGroupName, aGroup, avecItems, aParent );
   }
   else if( aiType == 2 )
   {
      return new CMCGroup( this, this, aszGroupName, aGroup, avecItems, aParent );
   }
}

void 
OrderedSubgroupColumnRenderer::onItemClicked( wxGridEvent& awxEvt )
{
   ColoredGroupColumnRenderer::onItemClicked( awxEvt );
   auto iSelectedRow = awxEvt.GetRow();
   auto pClickedItem = m_Root->GetItem( iSelectedRow );
   if( pClickedItem != nullptr )
   {
      awxEvt.SetClientData( &*pClickedItem );
      awxEvt.Skip();
   }
}

wxString
OrderedSubgroupColumnRenderer::uiGetDisplayTitle()
{
   return m_szGroupTitle + " (" + std::to_string( m_Root->GetSize() ) + ")";
}

MultiDistinctGroupColumnRenderer::MultiDistinctGroupColumnRenderer( wxPanel* aParent, wxWindowID aiWID, const Group& aGroup )
   : ColoredGroupColumnRenderer( aParent, aiWID, aGroup )
{
   
}

MultiDistinctGroupColumnRenderer::~MultiDistinctGroupColumnRenderer()
{
   delete m_Root;
}

// ColoredGroupColumnRenderer
void 
MultiDistinctGroupColumnRenderer::Draw( std::vector<std::shared_ptr<IRendererItem>> avecItemData )
{
   m_Root = new RootGroup( this, this, m_Group, avecItemData );

   this->SetColLabelValue( 0, uiGetDisplayTitle() );
   m_Root->Draw();

   this->SetColSize( 0, this->GetVirtualSize().GetWidth() );
}

bool 
MultiDistinctGroupColumnRenderer::RemoveItem( std::shared_ptr<IRendererItem> aptItem )
{
   if( m_Root->RemoveItem( aptItem ) )
   {
      this->SetColLabelValue( 0, uiGetDisplayTitle() );
      return true;
   }
   return false;
}

void 
MultiDistinctGroupColumnRenderer::AddItem( std::shared_ptr<IRendererItem> aptItem )
{
   m_Root->AddItem( aptItem );
   this->SetColLabelValue( 0, uiGetDisplayTitle() );
}

// DisplayNodeSource
DisplayGroup* 
MultiDistinctGroupColumnRenderer::GetDisplayGroup( int aiType, wxString aszGroupName, Group aGroup,
                                                   std::vector<std::shared_ptr<IRendererItem>> avecItems,
                                                   DisplayGroup* aParent )
{
   if( aiType == 1 )
   {
      return new GuildGroup( this, this, aszGroupName, aGroup, avecItems, aParent );
   }
}

void 
MultiDistinctGroupColumnRenderer::onItemClicked( wxGridEvent& awxEvt )
{
   auto iSelectedRow = awxEvt.GetRow();
   auto pClickedItem = m_Root->GetItem( iSelectedRow );
   if( pClickedItem != nullptr )
   {
      awxEvt.SetClientData( &*pClickedItem );
      awxEvt.Skip();
   }
   ColoredGroupColumnRenderer::onItemClicked( awxEvt );
}

wxString
MultiDistinctGroupColumnRenderer::uiGetDisplayTitle()
{
   return m_szGroupTitle + " (" + std::to_string( m_Root->GetSize() ) + ")";
}

MultiDistinctGroupColumnRenderer::
GuildGroup::GuildGroup( ColumnRenderer* apRenderer,
                        DisplayNodeSource* apSource,
                        wxString aszGroupName,
                        Group aGroup,
                        std::vector<std::shared_ptr<IRendererItem>> avecItems,
                        DisplayGroup* aParent )
   : DisplayGroup( apRenderer, apSource, aszGroupName, aGroup, avecItems, aParent ), 
   m_bHasDrawnHeader( false ),
   m_bHasDrawnLastSpacer(false)
{

}

MultiDistinctGroupColumnRenderer::
GuildGroup::~GuildGroup()
{

}

bool 
MultiDistinctGroupColumnRenderer::GuildGroup::RemoveItem( std::shared_ptr<IRendererItem> aptItem )
{
   if( DisplayGroup::RemoveItem( aptItem ) )
   {
      if( m_bHasDrawnHeader )
      {
         m_pRenderer->UndisplayItem( GetFirstRow() );
         wxString buf = m_szGroupName + " (" + std::to_string( GetSize() ) + ")";
         m_pRenderer->DisplayItem( buf, "",
                                   wxFont( wxFontInfo( 11 ).FaceName( "Trebuchet MS" ) ).Bold(),
                                   true, wxColour( "BLACK" ),
                                   m_pRenderer->GetBackgroundColor(), 
                                   GetFirstRow() );
         m_bHasDrawnHeader = true;
      }
      return true;
   }
   return false;
}

bool 
MultiDistinctGroupColumnRenderer::GuildGroup::AddItem( std::shared_ptr<IRendererItem> aptItem )
{
   if( DisplayGroup::AddItem( aptItem ) )
   {
      if( m_bHasDrawnHeader )
      {
         m_pRenderer->UndisplayItem( GetFirstRow() );
         wxString buf = m_szGroupName + " (" + std::to_string( GetSize() ) + ")";
         m_pRenderer->DisplayItem( buf, "",
                                   wxFont( wxFontInfo( 11 ).FaceName( "Trebuchet MS" ) ).Bold(),
                                   true, wxColour( "BLACK" ),
                                   m_pRenderer->GetBackgroundColor(), 
                                   GetFirstRow() );
         m_bHasDrawnHeader = true;
      }
      return true;
   }
   return false;
}

void 
MultiDistinctGroupColumnRenderer::
GuildGroup::Draw()
{
   if( !m_bHasDrawnHeader )
   {
      // Undraw space
      wxString buf = m_szGroupName + " (" + std::to_string( GetSize() ) + ")";
      m_pRenderer->DisplayItem( buf, "",
                                wxFont( wxFontInfo( 11 ).FaceName( "Trebuchet MS" ) ).Bold(),
                                true, wxColour( "BLACK" ),
                                m_pRenderer->GetBackgroundColor(), 
                                GetFirstRow() );
      m_bHasDrawnHeader = true;
   }

   int iRow = GetFirstItemRow();
   for( auto& child : m_setItems )
   {
      m_pRenderer->DisplayItem( child->GetName(), child->GetHash(),
                                wxFont(), false, wxColour(),
                                m_pRenderer->GetBackgroundColor(),
                                iRow );
      m_mapDrawnItems[child] = true;
      iRow++;
   }

   // Undraw all the end space
   if( m_bHasDrawnLastSpacer && IsLastChild() )
   {
      // Undraw endspace
      m_pRenderer->UndisplayItem( GetFirstRow() + GetDrawSize() );
      m_bHasDrawnLastSpacer = false;
   }
   else if( !m_bHasDrawnLastSpacer && !IsLastChild() )
   {
      // Draw endspace
      // TODO: All these header things need to be updated to the right row...
      // Perhaps don't link on old spot but rather where it is expected to be.
      m_pRenderer->DisplayItem( "", "",
                                wxFont(), false, wxColour(),
                                m_pRenderer->GetBackgroundColor(), 
                                GetFirstRow() + GetDrawSize() );
      m_bHasDrawnLastSpacer = true;
   }
}

void
MultiDistinctGroupColumnRenderer::
GuildGroup::UnDraw()
{
   if( m_bHasDrawnHeader )
   {
      m_pRenderer->UndisplayItem( GetFirstRow() );
      m_bHasDrawnHeader = false;
   }

   if( m_bHasDrawnLastSpacer )
   {
      // Subtract 1 because getDrawSize() includes the footer in the count.
      m_pRenderer->UndisplayItem( GetFirstRow() + GetDrawSize() - 1 );
      m_bHasDrawnLastSpacer = false;
   }

   for( int i = 0; i < m_mapDrawnItems.size(); i++ )
   {
      m_pRenderer->UndisplayItem( GetFirstRow() );
   }
   m_mapDrawnItems.clear();
}

int 
MultiDistinctGroupColumnRenderer::
GuildGroup::GetFirstItemRow()
{
   int iFirstDrawnRow = GetFirstRow();
   if( m_bHasDrawnHeader )
   {
      iFirstDrawnRow += 1;
   }
   return iFirstDrawnRow;
}

int 
MultiDistinctGroupColumnRenderer::
GuildGroup::GetTotalOverhead()
{
   int iSize = 0;
   if( m_bHasDrawnHeader )
   {
      iSize++;
   }

   if( m_bHasDrawnLastSpacer )
   {
      iSize++;
   }

   return iSize;
}

// Make a copy... TODO: Something better.
RendererItem::RendererItem( CardInterface * apIFace )
   : m_pCard( std::shared_ptr<CardInterface>( new CardInterface(*apIFace) ) ) 
{

}

RendererItem::~RendererItem()
{
}

CardInterface *
RendererItem::GetBase()
{
   return m_pCard.get();
}

std::string
RendererItem::GetHash() const
{
   if( m_hashMemo.IsEmpty() )
   {
      auto self = const_cast<RendererItem*>(this);
      self->m_hashMemo = m_pCard->GetHash();
   }
   return m_hashMemo.ToStdString();
}

bool 
RendererItem::RepresentsUID(const std::string& aszUID) const
{
   auto iter_memo = m_mapUIDMemo.find( aszUID );
   if( iter_memo != m_mapUIDMemo.end() )
   {
      return iter_memo->second;
   }
   else
   {
      bool bMatch = false;
      for( auto& szUID : m_pCard->GetRepresentingUIDs() )
      {
         if( szUID == aszUID )
         {
            bMatch = true;
            break;
         }
      }
      auto self = const_cast<RendererItem*>(this);
      self->m_mapUIDMemo[aszUID] = bMatch;
      return bMatch;
   }
}

std::string
RendererItem::GetName() const
{
   return m_pCard->GetName();
}

std::string
RendererItem::GetMetaTag( const std::string & aszKey ) const
{
   auto iter_memo = m_mapMetaMemo.find( aszKey );
   if( iter_memo != m_mapMetaMemo.end() )
   {
      return iter_memo->second.ToStdString();
   }
   else
   {
      auto szVal = m_pCard->GetMetaTag( aszKey );
      auto self = const_cast<RendererItem*>(this);
      self->m_mapMetaMemo[aszKey] = szVal;
      return szVal;
   }
}

std::string
RendererItem::GetAttribute( const std::string & aszKey ) const
{
   auto iter_memo = m_mapAttrMemo.find( aszKey );
   if( iter_memo != m_mapAttrMemo.end() )
   {
      return iter_memo->second.ToStdString();
   }
   else
   {
      auto szVal = m_pCard->GetAttribute( aszKey );
      auto self = const_cast<RendererItem*>(this);
      self->m_mapAttrMemo[aszKey] = szVal;
      return szVal;
   }
}
