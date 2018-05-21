#include "Group.h"
#include "CardInterface.h"

Group::ItemSorting::ItemSorting()
{
   m_ptSorter = std::shared_ptr<SortingOperator>( new SortingOperator() );
}

Group::ItemSorting::ItemSorting( SortingOperator* aptSorting )
   : ItemSorting()
{
   if( aptSorting != nullptr )
   {
      m_ptSorter = std::shared_ptr<SortingOperator>( aptSorting );
   }
}

bool
Group::ItemSorting::operator()( const CardInterface* agrpLeft, const CardInterface* agrpRight ) const
{
   return (*m_ptSorter)(agrpLeft->GetName(), agrpRight->GetName());
}


bool
Group::SortingOperator::operator()( const wxString& agrpLeft, const wxString& agrpRight ) const
{
   return agrpLeft < agrpRight;
}

Group::Sorting::Sorting()
{
   m_ptSorter = std::shared_ptr<SortingOperator>( new SortingOperator() );
}

Group::Sorting::Sorting( SortingOperator* aptSorting )
   : Sorting()
{
   if( aptSorting != nullptr )
   {
      m_ptSorter = std::shared_ptr<SortingOperator>( aptSorting );
   }
}

bool
Group::Sorting::operator()( const wxString& agrpLeft, const wxString& agrpRight ) const
{
   return (*m_ptSorter)(agrpLeft, agrpRight);
}

Group::Group(bool abIsEmpty)
   : BIsEmpty(abIsEmpty)
{
   if( !BIsEmpty )
   {
      DefaultSubGroup = std::shared_ptr<Group>( new Group( true ) );
   }
   SortingFunctor = std::shared_ptr<Sorting>( new Sorting() );
   ItemSortingFunctor = std::shared_ptr<ItemSorting>( new ItemSorting() );
}

wxString 
Group::GetParentGroupName()
{
   return ParentGroupName;
}

Group& 
Group::SetSubGroupOf( const wxString& aszName )
{
   ParentGroupName = aszName;
   return *this;
}

Group&
Group::GroupOn( const wxString& aszKey, bool abIsMetaKey )
{
   Key = aszKey;
   MetaKey = abIsMetaKey;
   return *this;
}

Group& 
Group::AliasGroup( const wxString& aszGroup, const wxString& aszAlias )
{
   Aliases.insert( std::make_pair( aszGroup, aszAlias ) );
   return *this;
}

Group&
Group::BroadenGroup( const wxString& aszValue )
{
   BroadenedValues.push_back( aszValue );
   return *this;
}

Group&
Group::OverrideGrouping( const Group& aGrouping )
{
   Overrides.push_back( aGrouping );
   return *this;
}

Group& 
Group::SetDefaultSubGroup( const Group& aGrouping )
{
   DefaultSubGroup = std::shared_ptr<Group>( new Group( aGrouping ) );
   return *this;
}

Group& 
Group::AddSubGroup( const wxString& aszMajorGroup, Group& aGrouping )
{
   aGrouping.SetSubGroupOf( aszMajorGroup );
   SubGroups.insert( std::make_pair(aszMajorGroup, aGrouping ) );
   return *this;
}

// THIS TAKES OWNERSHIP OF THE POINTER
Group& 
Group::SetGroupSortingFunctor( SortingOperator* aptFunctor )
{
   SortingFunctor = std::shared_ptr<Sorting>(new Sorting(aptFunctor));
   return *this;
}

Group& 
Group::SetItemSortingFunctor( SortingOperator* aptFunctor )
{
   ItemSortingFunctor = std::shared_ptr<ItemSorting>( new ItemSorting( aptFunctor ) );
   return *this;
}

wxString
Group::
GetGroup( const CardInterface& aData ) const
{
   wxString szGroup;
   // If the item has the override field, then that field is what is used.
   for( auto& over : Overrides )
   {
      szGroup = over.GetGroup( aData );
      if( szGroup != "" )
      {
         break;
      }
   }

   // If there is no override.
   if( szGroup.IsEmpty() )
   {
      if( MetaKey )
      {
         szGroup = aData.GetMetaTag( Key.ToStdString() );
      }
      else
      {
         szGroup = aData.GetAttribute( Key.ToStdString() );
      }

      for( auto& broadGroup : BroadenedValues )
      {
         if( szGroup.find( broadGroup ) != wxString::npos )
         {
            szGroup = broadGroup;
            break;
         }
      }

      // Apply aliases if there are any.
      auto iter_Alias = Aliases.find( szGroup );
      if( iter_Alias != Aliases.end() )
      {
         szGroup = iter_Alias->second;
      }
   }

   return szGroup;
}

Group 
Group::GetSubGroup( const wxString& aszGroup ) const
{
   auto iter_SubGroup = SubGroups.find( aszGroup );
   if( iter_SubGroup != SubGroups.end() )
   {
      // TODO: Should this be GetSubGroup?
      return iter_SubGroup->second;
   }

   return *DefaultSubGroup;
}

std::shared_ptr<Group::Sorting> 
Group::GetSortingFunctor() const
{
   return SortingFunctor;
}

std::shared_ptr<Group::ItemSorting>
Group::GetItemSortingFunctor() const
{
   return ItemSortingFunctor;
}

bool 
Group::IsEmpty() const
{
   return BIsEmpty;
}