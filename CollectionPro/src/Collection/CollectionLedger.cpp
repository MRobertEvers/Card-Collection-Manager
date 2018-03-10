#include "CollectionLedger.h"

CollectionLedger::CollectionLedger()
{

}

CollectionLedger::~CollectionLedger()
{

}

std::vector<std::weak_ptr<const CopyItem>>
CollectionLedger::ViewOwned()
{
   return m_vecOwned;
}

std::vector<std::weak_ptr<const CopyItem>>
CollectionLedger::ViewPresent()
{
   return m_vecPresent;
}

TryGetCopy<std::shared_ptr<CollectionLedger>> 
CollectionLedger::Lookup( const Location& aID )
{
   TryGetCopy<std::shared_ptr<CollectionLedger>> ptRetVal;

   auto iter_find = ms_LedgerLookup.find( aID );
   if( iter_find != ms_LedgerLookup.end() )
   {
      ptRetVal.Set( iter_find->second );
   }

   return ptRetVal;
}