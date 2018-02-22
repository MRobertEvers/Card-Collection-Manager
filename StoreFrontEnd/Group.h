#pragma once
#include "wx/wxprec.h"

class GroupItemData;

class Group
{
public:
   Group& GroupOn( const wxString& aszKey, bool abIsMetaKey = true );
   Group& BroadenSubGroup( const wxString& aszValue );
   Group& OverrideGrouping( const Group& aGrouping );

   wxString GetGroup( const GroupItemData& aData );
private:
   wxString Key;
   bool MetaKey;
   std::vector<wxString> BroadenedValues;
   std::vector<Group> Overrides;
};