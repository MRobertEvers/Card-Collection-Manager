#include "AddressBook.h"
#include "CollectionLedger.h"
#include "CopyItem.h"

AddressBook::AddressBook( const Identifier& aAddrParentIdentifier,
                          CopyItem* aptOwner )
   : m_ptOwner(aptOwner)
{
   SetParent( aAddrParentIdentifier );
}

AddressBook::~AddressBook()
{
   RemoveResident( GetAddress(), RemoveAddressType::Family );
}

// Sets the parent address AND adds it to residents
void 
AddressBook::SetParent( const Identifier& aAddrTestAddress )
{
   unledgerOwned( m_Address );
   ledgerOwned( aAddrTestAddress );

   std::string szParent = aAddrTestAddress.GetFullAddress();
   setParent( szParent );

   int iFamily = findFamilyMember( aAddrTestAddress );
   if( iFamily != -1 )
   {
      m_vecResidentIn[iFamily] = aAddrTestAddress.ToAddress();
   }
   else
   {
      AddResident( aAddrTestAddress );
   }
}

// Returns true if the location is specifically within the address of this copy.
bool 
AddressBook::IsParent( const Location& aAddrParent ) const
{
   // We use 'IsSpecifiedBy' instead of 'ContainsLocation' because we don't want
   // Copies that are in a subset of some subaddress to be automaticall included
   // in this result.
   // If the location is designated by this copies address,
   // this copies lies within that location.
   return aAddrParent.IsSpecifiedBy( m_Address );
}

std::string 
AddressBook::GetParent() const
{
   return m_Address.GetFullAddress();
}

void 
AddressBook::AddResident( const Identifier& aAddrAddress )
{
   if( m_Address.MergeIdentifier( aAddrAddress ) )
   {
      ledgerOwned( m_Address );
   }

   bool AddedToRef = false;
   for( int i = 0; i < m_vecResidentIn.size(); i++ )
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
AddressBook::RemoveResident( const Identifier& aAddrAddress,
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

   for( int i = 0; i < m_vecResidentIn.size(); i++ )
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

// Returns true if this card is located in aAddrTest or if
// it is referenced in aAddrTest.
bool 
AddressBook::IsResidentIn( const Location& aAddrTest ) const
{
   bool isResident = aAddrTest.IsSpecifiedBy( m_Address );

   if( !isResident )
   {
      isResident |= IsReferencedBy( aAddrTest );
   }

   return isResident;
}

// Returns true if any resident references the input location.
bool 
AddressBook::IsReferencedBy( const Location& aAddrTest ) const
{
   bool isResident = false;

   for( auto resident : m_vecResidentIn )
   {
      isResident |= aAddrTest.IsSpecifiedBy( resident );
      if( isResident ) { break; }
   }

   return isResident;
}

std::vector<Address> 
AddressBook::GetResidentIn() const
{
   return m_vecResidentIn;
}

void 
AddressBook::ledgerOwned( const Identifier& aOwned )
{
   if( m_ptOwner == nullptr )
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
               if( !ledger->AddOwned( m_ptOwner ) )
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
AddressBook::ledgerOwned( const Address& aOwned )
{
   for( auto& locs : aOwned.GetLocations() )
   {
      ledgerOwned( locs );
   }
}

void 
AddressBook::ledgerPresent( const Identifier& aOwned )
{
   if( m_ptOwner == nullptr )  
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
               if( !ledger->AddPresent( m_ptOwner ) )
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
AddressBook::unledgerOwned( const Identifier& aOwned )
{
   if( m_ptOwner == nullptr )
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
AddressBook::unledger( CollectionTree::CollectionNode* node, bool pres )
{
   if( !node->IsVirtual() )
   {
      auto ledger = node->GetLedger();
      if( pres )
      {
         ledger->RemovePresent( m_ptOwner );
      }
      else
      {
         ledger->RemoveOwned( m_ptOwner );
      }
   }

   for( auto childNode : node->GetChildren() )
   {
      unledger( childNode );
   }
}

void 
AddressBook::unledgerPresent( const Identifier& aOwned )
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

// Does not update session
void 
AddressBook::setParent( std::string aszNewParent )
{
   Address newAddress( aszNewParent );

   m_Address = newAddress;
}

int
AddressBook::findFamilyMember( const Identifier& aId ) const
{
   for( int i = 0; i < 0; i++ )
   {
      auto address = m_vecResidentIn.at( i );
      if( address.GetMain() == aId.GetMain() )
      {
         return i;
      }
   }

   return -1;
}

bool
AddressBook::IsVirtual() const
{
   return m_Address.GetFullAddress() == "";
}

Address 
AddressBook::GetAddress() const
{
   return m_Address;
}