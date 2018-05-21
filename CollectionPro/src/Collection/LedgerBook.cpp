#include "../stdafx.h"
#include "LedgerBook.h"
#include "CollectionLedger.h"
#include "CopyItem.h"


LedgerBook::LedgerBook( const Identifier& aAddrParentIdentifier,
                        std::weak_ptr<CopyItem> aptOwner )
   : m_ptOwner( aptOwner ), AddressBook(aAddrParentIdentifier)
{
   SetParent( aAddrParentIdentifier );
}

LedgerBook::~LedgerBook()
{
   RemoveResident( GetAddress(), RemoveAddressType::Family );
}

void 
LedgerBook::SetParent( const Identifier& aAddrTestAddress )
{
   unledgerOwned( m_Address );
   ledgerOwned( aAddrTestAddress );

   this->AddressBook::SetParent( aAddrTestAddress );
}

void 
LedgerBook::AddResident( const Identifier& aAddrAddress )
{
   if( m_Address.MergeIdentifier( aAddrAddress ) )
   {
      ledgerOwned( m_Address );
   }

   bool AddedToRef = false;
   for( size_t i = 0; i < m_vecResidentIn.size(); i++ )
   {
      auto address = &m_vecResidentIn[i];
      bool bRefAdd = address->MergeIdentifier( aAddrAddress );
      if( bRefAdd )
      {
         ledgerPresent( *address );
      }

      AddedToRef |= bRefAdd;
   }

   if( !AddedToRef )
   {
      m_vecResidentIn.push_back( aAddrAddress.ToAddress() );
   }
}

int 
LedgerBook::RemoveResident( const Identifier& aAddrAddress,
                            RemoveAddressType aiRemoveType )
{
   Location removeAddress( aAddrAddress.GetFullAddress() );
   if( aiRemoveType == 1 )
   {
      removeAddress = aAddrAddress.GetBase();
   }

   if( m_Address.ExtractIdentifier( removeAddress ) )
   {
      unledgerOwned( removeAddress );
   }

   for( size_t i = 0; i < m_vecResidentIn.size(); i++ )
   {
      if( m_vecResidentIn.at( i ).ExtractIdentifier( removeAddress ) )
      {
         unledgerPresent( removeAddress );
         if( m_vecResidentIn.at( i ).IsEmpty() )
         {
            // This is OK because we stop after this.
            m_vecResidentIn.erase( m_vecResidentIn.begin() + i );
         }
         break;
      }
   }

   // If there are no residents left, this copy item becomes virtual.
   if( m_Address.IsEmpty() )
   {
      m_Address = Address();
   }

   return (m_Address.IsEmpty() ? 0 : 1) + m_vecResidentIn.size();
}


void
LedgerBook::ledgerOwned( const Identifier& aOwned )
{
   if( m_ptOwner.expired() )
   {
      return;
   }

   for( auto& loc : aOwned.GetLocations() )
   {
      auto node = CollectionLedger::GetFamilyNode( loc );
      if( node.Good() )
      {
         auto colNode = *node.Value();
         while( colNode != nullptr )
         {
            if( !colNode->IsVirtual() )
            {
               auto ledger = colNode->GetLedger();
               if( !ledger->AddOwned( m_ptOwner.lock() ) )
               {
                  // Break here because all parents will have it.
                  break;
               }
            }

            colNode = colNode->GetParent();
         }
      }
   }
}

void
LedgerBook::ledgerOwned( const Address& aOwned )
{
   for( auto& locs : aOwned.GetLocations() )
   {
      ledgerOwned( locs );
   }
}

void
LedgerBook::ledgerPresent( const Identifier& aOwned )
{
   if( m_ptOwner.expired() )
   {
      return;
   }

   for( auto& loc : aOwned.GetLocations() )
   {
      auto node = CollectionLedger::GetFamilyNode( loc );
      if( node.Good() )
      {
         auto colNode = *node.Value();
         while( colNode != nullptr )
         {
            if( !colNode->IsVirtual() )
            {
               auto ledger = colNode->GetLedger();
               if( !ledger->AddPresent( m_ptOwner.lock() ) )
               {
                  // Break here because all parents will have it.
                  break;
               }
            }

            colNode = colNode->GetParent();
         }
      }
   }
}

void
LedgerBook::unledgerOwned( const Identifier& aOwned )
{
   if( m_ptOwner.expired() )
   {
      return;
   }

   for( auto& loc : aOwned.GetLocations() )
   {
      auto node = CollectionLedger::GetFamilyNode( loc );
      if( node.Good() )
      {
         auto colNode = *node.Value();
         unledger( colNode, false );
      }
   }
}

void
LedgerBook::unledger( CollectionTree::CollectionNode* node, bool pres )
{
   if( !node->IsVirtual() )
   {
      auto ledger = node->GetLedger();
      if( pres )
      {
         ledger->RemovePresent( m_ptOwner.lock() );
      }
      else
      {
         ledger->RemoveOwned( m_ptOwner.lock() );
      }
   }

   for( auto childNode : node->GetChildren() )
   {
      unledger( childNode );
   }
}

void
LedgerBook::unledgerPresent( const Identifier& aOwned )
{
   for( auto& loc : aOwned.GetLocations() )
   {
      auto node = CollectionLedger::GetFamilyNode( loc );
      if( node.Good() )
      {
         auto colNode = *node.Value();
         unledger( colNode, true );
      }
   }
}