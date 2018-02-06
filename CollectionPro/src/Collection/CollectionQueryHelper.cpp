#include "CollectionQueryHelper.h"
#include <algorithm>
#include "Collection.h"
using namespace std;

CollectionQueryHelper::CollectionQueryHelper(Collection* aptCollection)
   : m_ptCollection(aptCollection)
{

}

CollectionQueryHelper::~CollectionQueryHelper()
{

}


// TODO: Create more logical function flow.
vector<string>
CollectionQueryHelper::QueryCollection(Query aiQueryParms)
{
   if( aiQueryParms.GetClassed() )
   {
      // TODO: Complete this implementation.
      //return getListGroupedByNameEnumByHash(aiQueryParms);
      return getListGroupedByHashEnumUIDs(aiQueryParms);
   }
   else if ( aiQueryParms.GetUIDs() )
   {
      return getListGroupedByHashEnumUIDs(aiQueryParms);
   }
   else
   {
      // Default search.
      return getListGroupedByHashEnumUIDs(aiQueryParms);

   }

}

multimap<string, CollectionQueryHelper::ItemData>
CollectionQueryHelper::getHashGroupsEnumUIDs(const Query& aiQueryParms)
{
   multimap<string, ItemData> mapSeenHashes;
   auto ptColSource = m_ptCollection->m_ptrCollectionSource;

   map<string, ItemData> mapCardHashes;
   for( auto iItem : m_ptCollection->getCollection() )
   {
      auto item = ptColSource->GetCardPrototype(iItem);
      ItemData cardData;
      cardData.Name = item->GetName();
      cardData.Item = item.Value();

      // 1. If we are only looking for certain card names, restrict here
      // so we can save time by skipping this iteration.
      if( !setFrontAndIsMatchToQuery(aiQueryParms, cardData) )
      {
         continue;
      }

      // 1. Count each occurence of an identical hash within a card if collapsing,
      // otherwise add it.
      mapCardHashes.clear();
      auto lstCopies = item->FindCopies(m_ptCollection->GetIdentifier(), All);
      for( auto copy : lstCopies )
      {
         // Look for copies that match.
         cardData.Hash = copy->GetHash(aiQueryParms.GetHashType());
         auto iter_Counted = mapCardHashes.find(cardData.Hash);
         if( (aiQueryParms.GetCollapsed()) &&
            (iter_Counted != mapCardHashes.end()) )
         {
            // If we are collapsing, and there is a match, count it. Otherwise, add card data.
            iter_Counted->second.Count++;
            iter_Counted->second.Groups.insert(copy->GetUID());
         }
         else
         {
            cardData.Copy = copy.get();
            cardData.Groups.insert(copy->GetUID());
            mapCardHashes.insert(make_pair(cardData.Hash, cardData));
         }
      }

      mapSeenHashes.insert(mapCardHashes.begin(), mapCardHashes.end());
   }

   return mapSeenHashes;
}

// TODO Refactor this/ Make note of the options that this cares about
// and what they do.
vector<string>
CollectionQueryHelper::getListGroupedByHashEnumUIDs( const Query& aiQueryParms )
{
   auto mapSeenHashes = getHashGroupsEnumUIDs(aiQueryParms);

   auto ptColSource = m_ptCollection->m_ptrCollectionSource;
   auto ptColDetails = m_ptCollection->m_ptrCollectionDetails;

   vector<string> lstFront;
   vector<string> lstBack;

   // 3. Create the list of strings, shorten them if necessary.
   for( auto card : mapSeenHashes )
   {
      // Ignore hashes if they aren't included in a nonempty search
      if( aiQueryParms.GetHashes().size() > 0 )
      {
         auto vecHashes = aiQueryParms.GetHashes();
         if( find(vecHashes.begin(), vecHashes.end(), card.second.Hash) == vecHashes.end() )
         {
            continue;
         }
      }

      string szLine;
      ItemData& cardData = card.second;

      // Build the strings
      if( !aiQueryParms.GetIncludeCount() )
      {
         // ToCardLine does not include the count if its 0.
         cardData.Count = 0;
      }

      // UIDs replaces the meta tags with UIDs.
      vector<Tag> vecMeta;
      if( aiQueryParms.GetUIDs() )
      {
         for( auto& szUID : cardData.Groups )
         {
            vecMeta.push_back(make_pair(CopyItem::GetUIDKey(), szUID));
         }
      }
      else
      {
         vecMeta = cardData.Copy->GetMetaTags(aiQueryParms.GetMetaType());
      }

      szLine = CollectionObject::ToCardLine(
         cardData.Copy->GetAddress(),
         cardData.Name,
         cardData.Copy->GetIdentifyingAttributes(),
         vecMeta,
         *ptColDetails->GetAddress(),
         cardData.Count
      );

      if( aiQueryParms.GetShort() )
      {
         ptColSource->CollapseCardLine(szLine);
      }

      if( cardData.Front )
      {
         lstFront.push_back(szLine);
      }
      else
      {
         lstBack.push_back(szLine);
      }
   }

   vector<string> lstRetVal;
   for( auto& szLine : lstFront )
   {
      lstRetVal.push_back(szLine);
   }

   for( auto& szLine : lstBack )
   {
      lstRetVal.push_back(szLine);
   }
   return lstRetVal;
}

multimap<string, CollectionQueryHelper::ItemData>
CollectionQueryHelper::getNameGroupsEnumHash(const Query& aiQueryParms)
{
   multimap<string, ItemData> mapSeenNames;
   auto ptColSource = m_ptCollection->m_ptrCollectionSource;

   for( auto iItem : m_ptCollection->getCollection() )
   {
      auto item = ptColSource->GetCardPrototype(iItem);
      ItemData cardData;
      cardData.Name = item->GetName();
      cardData.Item = item.Value();

      // 1. If we are only looking for certain card names, restrict here
      // so we can save time by skipping this iteration.
      if( !setFrontAndIsMatchToQuery(aiQueryParms, cardData) )
      {
         continue;
      }

      // 1. Count each occurence of an identical hash within a card if collapsing,
      // otherwise add it.
      auto lstCopies = item->FindCopies(m_ptCollection->GetIdentifier(), All);
      for( auto copy : lstCopies )
      {
         // Look for copies that match.
         cardData.Hash = copy->GetHash(aiQueryParms.GetHashType());
         auto iter_Counted = cardData.Groups.find(cardData.Hash);
         if( iter_Counted == cardData.Groups.end() )
         {
            cardData.Groups.insert(cardData.Hash);
         }
      }
      mapSeenNames.insert(make_pair(cardData.Name, cardData));
   }

   return mapSeenNames;
}

vector<string>
CollectionQueryHelper::getListGroupedByNameEnumByHash(const Query& aiQueryParms)
{
   auto mapSeenNames = getNameGroupsEnumHash(aiQueryParms);

   auto ptColSource = m_ptCollection->m_ptrCollectionSource;
   auto ptColDetails = m_ptCollection->m_ptrCollectionDetails;

   vector<string> lstFront;
   vector<string> lstBack;

   // 3. Create the list of strings, shorten them if necessary.
   for( auto card : mapSeenNames )
   {
      string szLine;
      ItemData& cardData = card.second;
      // Build the strings

      // If Classed (We should be but check again) replaces the meta tags.
      vector<Tag> vecMeta;
      if( aiQueryParms.GetClassed() )
      {
         cardData.Count = 0;
         for( auto& szUID : cardData.Groups )
         {
            vecMeta.push_back(make_pair(CopyItem::GetHashKey(), szUID));
         }
      }
      else
      {
         vecMeta = cardData.Copy->GetMetaTags(aiQueryParms.GetMetaType());
      }

      szLine = CollectionObject::ToCardLine(
         cardData.Copy->GetAddress(),
         cardData.Name,
         vector<Tag>(),
         vecMeta,
         *ptColDetails->GetAddress(),
         cardData.Count
      );

      if( cardData.Front )
      {
         lstFront.push_back(szLine);
      }
      else
      {
         lstBack.push_back(szLine);
      }
   }

   vector<string> lstRetVal;
   for( auto& szLine : lstFront )
   {
      lstRetVal.push_back(szLine);
   }

   for( auto& szLine : lstBack )
   {
      lstRetVal.push_back(szLine);
   }
   return lstRetVal;
}

bool 
CollectionQueryHelper::setFrontAndIsMatchToQuery( const Query& aiQueryParms, ItemData& rData )
{
   if( aiQueryParms.GetSearch() != "" )
   {
      string szSearch = aiQueryParms.GetSearch();
      transform(szSearch.begin(), szSearch.end(), szSearch.begin(), ::tolower);
      string szName = rData.Name;
      transform(szName.begin(), szName.end(), szName.begin(), ::tolower);

      auto iFindName = szName.find(szSearch);
      rData.Front = iFindName == 0;
      if( iFindName == string::npos )
      {
         return false;
      }
   }

   return true;
}