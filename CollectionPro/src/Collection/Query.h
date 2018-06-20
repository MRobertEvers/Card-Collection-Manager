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
      m_bClassed = false;
      m_bIncludeIDString = true;
      m_bIncludeMetaString = true;
   }

   void UIDs() { Default(true); m_bUIDs = true; }
   void Classed() { Default(true); m_bClassed = true; }
   Query& FindHash(const std::string& aszHash) { m_vecHashes.push_back(aszHash); return *this; }
   Query& Short() { m_bIsShort = true; return *this; }
   Query& IncludeCount() { m_bIncludeCount = true; return *this; }
   Query& SetIncludeIDs( bool abInclude ) { m_bIncludeIDString = abInclude; return *this; }
   Query& SetIncludeMeta( bool abInclude ) { m_bIncludeMetaString = abInclude; return *this; }
   Query& SearchFor(const std::string& Search) { m_szSearch = Search; return *this; }
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
   bool GetClassed() const { return m_bClassed; }
   std::vector<std::string> GetHashes() const { return m_vecHashes;  }
   bool GetIsHashSearch() const { return m_vecHashes.size() > 0; }

private:
   bool m_bClassed;
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