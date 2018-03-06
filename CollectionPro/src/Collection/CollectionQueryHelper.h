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
      CopyItem* Copy;
      set<string> Groups;
   };

   Collection * m_ptCollection;

   multimap<string, ItemData> createHashToItemMap(const Query& aiQueryParms);
   vector<string> getListGroupedByHashEnumUIDs( const Query& aiQueryParms );

   bool setFrontAndIsMatchToQuery(const Query& aiQueryParms, ItemData& rData);
};