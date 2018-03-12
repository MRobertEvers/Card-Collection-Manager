#pragma once
#include <set>
#include <map>
#include <string>
#include <memory>

class CopyItem;
class CollectionObject;


class Ledger
{
public:
   Ledger();
   ~Ledger();

   bool AddOwned( std::shared_ptr<CopyItem> aptOwned );
   void RemoveOwned( std::shared_ptr<CopyItem> aptRemove );
   bool AddPresent( std::shared_ptr<CopyItem> aptPresent );
   void RemovePresent( std::shared_ptr<CopyItem> aptRemove );

protected:
   std::map<CollectionObject*, std::set<std::shared_ptr<CopyItem>>> m_mapPresent;
   std::set<std::shared_ptr<CopyItem>> m_vecOwned;
   std::set<std::shared_ptr<CopyItem>> m_vecPresent;
};

