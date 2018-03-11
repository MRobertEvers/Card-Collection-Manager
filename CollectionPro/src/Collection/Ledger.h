#pragma once
#include <set>
#include <memory>

class CopyItem;

class Ledger
{
public:
   Ledger();
   ~Ledger();

   bool AddOwned( CopyItem* aptOwned );
   void RemoveOwned( CopyItem* aptRemove );
   bool AddPresent( CopyItem* aptPresent );
   void RemovePresent( CopyItem* aptRemove );

protected:
   std::set<const CopyItem*> m_vecOwned;
   std::set<const CopyItem*> m_vecPresent;
};

