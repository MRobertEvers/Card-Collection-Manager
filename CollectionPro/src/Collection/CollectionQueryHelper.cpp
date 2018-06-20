#include "CollectionQueryHelper.h"
#include <algorithm>
#include "Collection.h"
using namespace std;

CollectionQueryHelper::CollectionQueryHelper(Collection* aptCollection)
   : m_ptCollection(aptCollection), m_bInvalidState(true)
{

}

CollectionQueryHelper::~CollectionQueryHelper()
{

}

vector<string>
CollectionQueryHelper::QueryCollection(Query aiQueryParms)
{
   if( aiQueryParms.GetIsHashSearch() )
   {
      // Fast Search
      return queryMemoList( aiQueryParms );
   }
   else
   {
      // Full Search
      return queryCompleteList( aiQueryParms );
   }
}

void
CollectionQueryHelper::InvalidateState()
{
   m_bInvalidState = true;
}

multimap<string, CollectionQueryHelper::ItemData>
CollectionQueryHelper::createHashToItemMap(const Query& aiQueryParms)
{
   multimap<string, ItemData> mapSeenHashes;
   auto ptColSource = m_ptCollection->m_ptrCollectionSource;

   multimap<string, ItemData> mapCardHashes;
   for( auto iItem : m_ptCollection->GetCollectionItems() )
   {
      auto item = iItem.first;
      ItemData cardData;
      cardData.Name = item->GetName();
      cardData.Item = item;

      // 1. If we are only looking for certain card names, restrict here
      // so we can save time by skipping this iteration.
      if( !setFrontAndIsMatchToQuery(aiQueryParms, cardData) )
      {
         continue;
      }

      // 1. Count each occurence of an identical hash within a card if collapsing,
      // otherwise add it.
      mapCardHashes.clear();
      auto lstCopies = iItem.second;
      for( auto copyW : lstCopies )
      {
         auto copy = copyW;
         // Look for copies that match.
         cardData.Hash = copy->GetHash();

         auto iter_Counted = mapCardHashes.find(cardData.Hash);
         if( ( aiQueryParms.GetCollapsed() )       &&
             ( iter_Counted != mapCardHashes.end() ) )
         {
            // If we are collapsing, and there is a match, count it. Otherwise, add card data.
            iter_Counted->second.Count++;
            iter_Counted->second.Groups.insert(copy->GetUID());
         }
         else
         {
            // Add the card as an additional entry.
            cardData.Copy = copy;
            cardData.Groups.clear();
            cardData.Count = 1;
            cardData.Groups.insert(copy->GetUID());
            mapCardHashes.insert(make_pair(cardData.Hash, cardData));
         }
      }

      mapSeenHashes.insert(mapCardHashes.begin(), mapCardHashes.end());
   }

   return mapSeenHashes;
}

void 
CollectionQueryHelper::memoizeHashToCollapsedItemMap()
{
   // This function is used to speed up searches on hashes.
   m_mapFastSearchCache.clear();

   Query qAllItems(true);
   auto multiMap = createHashToItemMap( qAllItems );
   m_mapFastSearchCache.insert( multiMap.begin(), multiMap.end() );
}

vector<string> 
CollectionQueryHelper::queryMemoList( const Query& aiQueryParms )
{
   // This function should only be used if the caller
   // is searching for specific hashes.
   if( m_bInvalidState )
   {
      memoizeHashToCollapsedItemMap();
      m_bInvalidState = false;
   }

   // Perform the hash Search.
   multimap<string, ItemData> mapSearch;
   for( auto& szFindHash : aiQueryParms.GetHashes() )
   {
      auto iter_findHash = m_mapFastSearchCache.find( szFindHash );
      if( iter_findHash != m_mapFastSearchCache.end() )
      {
         mapSearch.insert( *iter_findHash );
      }
   }

   return performQuery( aiQueryParms, mapSearch );
}

vector<string>
CollectionQueryHelper::queryCompleteList( const Query& aiQueryParms )
{
   auto mapSeenHashes = createHashToItemMap( aiQueryParms );
   return performQuery( aiQueryParms, mapSeenHashes );
}

vector<string> 
CollectionQueryHelper::performQuery( const Query& aiQueryParms, 
                                     const multimap<string, ItemData>& amapSearch )
{
   auto& mapSeenHashes = amapSearch;

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
         if( find( vecHashes.begin(), vecHashes.end(), card.second.Hash ) == vecHashes.end() )
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
            vecMeta.push_back( make_pair( MetaTag::GetUIDKey(), szUID ) );
         }
      }
      else
      {
         vecMeta = cardData.Copy->GetMetaTags( aiQueryParms.GetMetaType() );
      }

      // Remove the ID String or the Hash string if they are excluded.
      auto vecIDs = cardData.Copy->GetIdentifyingAttributes();
      if( !aiQueryParms.GetIncludeIDString() )
      {
         vecIDs.clear();
      }

      if( !aiQueryParms.GetIncludeMetaString() )
      {
         vecMeta.clear();
      }

      szLine = CollectionObject::ToCardLine(
         cardData.Copy->GetAddress(),
         cardData.Name,
         vecIDs,
         vecMeta,
         *ptColDetails->GetAddress(),
         cardData.Count
      );

      if( aiQueryParms.GetIsShort() )
      {
         ptColSource->CollapseCardLine( szLine );
      }

      if( cardData.Front )
      {
         lstFront.push_back( szLine );
      }
      else
      {
         lstBack.push_back( szLine );
      }
   }

   vector<string> lstRetVal;
   for( auto& szLine : lstFront )
   {
      lstRetVal.push_back( szLine );
   }

   for( auto& szLine : lstBack )
   {
      lstRetVal.push_back( szLine );
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