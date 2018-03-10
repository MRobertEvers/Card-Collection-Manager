#include "Ledger.h"



Ledger::Ledger()
{
}


Ledger::~Ledger()
{
}



void 
Ledger::AddOwned( std::weak_ptr<CopyItem> aptOwned )
{
   m_vecOwned.push_back( aptOwned );
   AddPresent( aptOwned );
}

void 
Ledger::RemoveOwned( std::weak_ptr<CopyItem> aptRemove )
{

}

void 
Ledger::AddPresent( std::weak_ptr<CopyItem> aptPresent )
{
   m_vecPresent.push_back( aptPresent );
}

void 
Ledger::RemovePresent( std::weak_ptr<CopyItem> aptRemove )
{

}