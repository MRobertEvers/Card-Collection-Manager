#include "Ledger.h"



Ledger::Ledger()
{
}


Ledger::~Ledger()
{
}


bool 
Ledger::AddOwned( CopyItem* aptOwned )
{
   m_vecOwned.insert( aptOwned );
   return AddPresent( aptOwned );
}

void 
Ledger::RemoveOwned( CopyItem* aptRemove )
{
   m_vecOwned.erase( aptRemove );
}

bool 
Ledger::AddPresent( CopyItem* aptPresent )
{
   return m_vecPresent.insert( aptPresent ).second;
}

void 
Ledger::RemovePresent( CopyItem* aptRemove )
{
   m_vecPresent.erase( aptRemove );
}