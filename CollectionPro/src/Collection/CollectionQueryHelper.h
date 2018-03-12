#pragma once
#include <vector>
#include <string>
#include <set>
#include <map>
#include "Query.h"

using namespace std;
class Collection;

class CollectionQueryHelper
{
public:
   CollectionQueryHelper(Collection* aptCollection);
   ~CollectionQueryHelper();

   vector<string> QueryCollection(Query aiQueryParms);
   void InvalidateState();
private:
   struct ItemData
   {
   public:
      ItemData() {};
      ItemData(const ItemData& other)
      {
         Name = other.Name;
         Hash = other.Hash;
         Count = other.Count;
         Front = other.Front;
         Item = other.Item;
         Copy = other.Copy;
         Groups = set<string>(other.Groups);
      }
      string Name;
      string Hash;
      unsigned int Count = 1;
      bool Front = true;
      CollectionObject* Item;
      shared_ptr<CopyItem> Copy;
      set<string> Groups;
   };

private:
   Collection * m_ptCollection;
   bool m_bInvalidState;
   multimap<string, ItemData> m_mapFastSearchCache;

   multimap<string, ItemData> createHashToItemMap(const Query& aiQueryParms);
   void memoizeHashToCollapsedItemMap();
   vector<string> queryMemoList( const Query& aiQueryParms );
   vector<string> queryCompleteList( const Query& aiQueryParms );
   vector<string> performQuery( const Query& aiQueryParms, const multimap<string, ItemData>& amapSearch );

   bool setFrontAndIsMatchToQuery(const Query& aiQueryParms, ItemData& rData);
};