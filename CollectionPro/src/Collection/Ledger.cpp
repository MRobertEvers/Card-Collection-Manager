#include "Ledger.h"
#include "CopyItem.h"
#include "CollectionObject.h"


Ledger::Ledger()
{
}


Ledger::~Ledger()
{
}


bool 
Ledger::AddOwned( std::shared_ptr<CopyItem> aptOwned )
{
   m_vecOwned.insert( aptOwned );
   return AddPresent( aptOwned );
}

void 
Ledger::RemoveOwned( std::shared_ptr<CopyItem> aptRemove )
{
   m_vecOwned.erase( aptRemove );
   RemovePresent( aptRemove );
}

bool 
Ledger::AddPresent( std::shared_ptr<CopyItem> aptPresent )
{
   m_mapPresent[aptPresent->GetObject()].insert( aptPresent );
   return m_vecPresent.insert( aptPresent ).second;
}

void 
Ledger::RemovePresent( std::shared_ptr<CopyItem> aptRemove )
{
   m_mapPresent[aptRemove->GetObject()].erase( aptRemove );
   m_vecPresent.erase( aptRemove );
}