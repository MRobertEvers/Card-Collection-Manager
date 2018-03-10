#pragma once
#include <vector>
#include <memory>

class CopyItem;

class Ledger
{
public:
   Ledger();
   ~Ledger();

   void AddOwned( std::weak_ptr<CopyItem> aptOwned );
   void RemoveOwned( std::weak_ptr<CopyItem> aptRemove );
   void AddPresent( std::weak_ptr<CopyItem> aptPresent );
   void RemovePresent( std::weak_ptr<CopyItem> aptRemove );

private:
   std::vector<std::weak_ptr<CopyItem>> m_vecOwned;
   std::vector<std::weak_ptr<CopyItem>> m_vecPresent;
};

