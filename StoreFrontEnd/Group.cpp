#include "Group.h"
#include "GroupItemData.h"

Group&
Group::
GroupOn( const wxString& aszKey, bool abIsMetaKey )
{
   Key = aszKey;
   MetaKey = abIsMetaKey;
   return *this;
}

Group&
Group::
BroadenSubGroup( const wxString& aszValue )
{
   BroadenedValues.push_back( aszValue );
   return *this;
}

Group&
Group::
OverrideGrouping( const Group& aGrouping )
{
   Overrides.push_back( aGrouping );
   return *this;
}

wxString
Group::
GetGroup( const GroupItemData& aData )
{
   wxString szGroup;
   for( auto& over : Overrides )
   {
      szGroup = over.GetGroup( aData );
      if( szGroup != "" )
      {
         break;
      }
   }

   if( szGroup == "" )
   {
      if( MetaKey )
      {
         szGroup = aData.GetMetaTag( Key );
      }
      else
      {
         szGroup = aData.GetAttribute( Key );
      }

      for( auto& broadGroup : BroadenedValues )
      {
         if( szGroup.find( broadGroup ) != wxString::npos )
         {
            szGroup = broadGroup;
            break;
         }
      }
   }

   return szGroup;
}