#pragma once
#include "Ledger.h"
#include "..\Support\TryGet.h"

#include <map>
#include <string>
#include <memory>
#include <vector>

class Location;
class CopyItem;

class CollectionLedger : public Ledger
{
public:
   CollectionLedger();
   ~CollectionLedger();

   std::vector<std::weak_ptr<const CopyItem>> ViewOwned();
   std::vector<std::weak_ptr<const CopyItem>> ViewPresent();

private:
   friend class AddressBook;

   std::vector<std::weak_ptr<const CopyItem>> m_vecOwned;
   std::vector<std::weak_ptr<const CopyItem>> m_vecPresent;

public:
   static TryGetCopy<std::shared_ptr<CollectionLedger>> Lookup( const Location& aszBaseAddress );

private:
   static std::map<Location, std::shared_ptr<CollectionLedger>> ms_LedgerLookup;
};