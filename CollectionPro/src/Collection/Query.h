#pragma once
#include "CopyItem.h"
#include <string>
#include <vector>

class Query
{
public:
   Query(bool Collapsed = false) { Default(Collapsed); }

   void Default(bool Collapsed = false)
   {
      m_bCollapsed = Collapsed;
      m_mttMetaType = MetaTag::Type::Visible;
      m_bIsShort = false;
      m_bIncludeCount = true;
      m_bUIDs = false;
      m_bIncludeIDString = true;
      m_bIncludeMetaString = true;
   }

   // Meta tags will not be returned, instead, the list of UIDs associated with the grouped
   // items will be returned in their place. e.g. If collapsed is true, it will return the UID of each
   // item represented by that string. Otherwise, it will simply return the UID of the item it represents
   void UIDs() { Default(true); m_bUIDs = true; }

   // True indicates to return each seen hash only once (count of each is included unless explicitly stated otherwise)
   // The metatags returned, if included, are the metatags of the first seen copy during search.
   // Only the Hidden/Tacking meta should differ among copies.
   Query& SetCollapse( bool abInclude ) { m_bCollapsed = abInclude; return *this; }

   // Adding a string here indicates that the query will only return items matching one of the hashes.
   Query& FindHash(const std::string& aszHash) { m_vecHashes.push_back(aszHash); return *this; }

   // True indicates that the "Shortened" card string is produced e.g. <name> [M15] 
   Query& Short() { m_bIsShort = true; return *this; }

   // True indicates that the 1x, 2x, etc will be included in the string return.
   Query& IncludeCount() { m_bIncludeCount = true; return *this; }

   // True indicates that the { set="blah" } aka Attribute field should be included
   Query& SetIncludeIDs( bool abInclude ) { m_bIncludeIDString = abInclude; return *this; }

   // True indicates that the <name> <{attrs}> : { meta="blah" } aka Meta field should be included
   Query& SetIncludeMeta( bool abInclude ) { m_bIncludeMetaString = abInclude; return *this; }

   // Adding a string here indicates that the query will only return items with matching names.
   Query& SearchFor(const std::string& Search) { m_szSearch = Search; return *this; }

   // Any of the following indicate which type of metatags should be included in the query return.
   // Overrided by SetIncludeIDs
   Query& IncludeMetaType(MetaTag::Type mtType) { m_mttMetaType = mtType; return *this; }
   Query& AnyMeta() { m_mttMetaType = MetaTag::Type::Any; return *this; }
   Query& NoMeta() { m_mttMetaType = MetaTag::Type::None; return *this; }
   Query& PublicMeta() { m_mttMetaType = MetaTag::Type::Public; return *this; }
   Query& IgnoredMeta() { m_mttMetaType = MetaTag::Type::Ignored; return *this; }
   Query& TrackingMeta() { m_mttMetaType = MetaTag::Type::Tracking; return *this; }
   Query& HiddenMeta() { m_mttMetaType = MetaTag::Type::Hidden; return *this; }

   bool GetIncludeCount() const { return m_bIncludeCount; }
   bool GetIncludeIDString() const { return m_bIncludeIDString; }
   bool GetIncludeMetaString() const { return m_bIncludeMetaString; }
   bool GetIsShort() const { return m_bIsShort; }
   std::string GetSearch() const { return m_szSearch; }
   bool GetCollapsed() const { return m_bCollapsed; }
   MetaTag::Type GetMetaType() const { return m_mttMetaType; }
   bool GetUIDs() const { return m_bUIDs; }
   std::vector<std::string> GetHashes() const { return m_vecHashes;  }
   bool GetIsHashSearch() const { return m_vecHashes.size() > 0; }

private:
   bool m_bUIDs;
   std::string m_szSearch;
   bool m_bCollapsed;
   bool m_bIncludeCount;
   bool m_bIncludeIDString;
   bool m_bIncludeMetaString;
   MetaTag::Type m_mttMetaType;
   std::vector<std::string> m_vecHashes;
   
   
   bool m_bIsShort;
};