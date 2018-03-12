#pragma once
#include "AddressBook.h"
#include "..\Addressing\Addresser.h"
#include "CollectionTree.h"

#include <memory>
#include <vector>
#include <string>

class LedgerBook : public AddressBook
{
public:
   LedgerBook( const Identifier& aAddrParentIdentifier, 
               std::weak_ptr<CopyItem> aptOwner );
   ~LedgerBook();

   virtual void SetParent( const Identifier& aAddrTestAddress ) override;

   virtual void AddResident( const Identifier& aAddrAddress ) override;
   virtual int RemoveResident( const Identifier& aAddrAddress,
                               RemoveAddressType aiRemoveType = Individual ) override;

private:
   std::weak_ptr<CopyItem> m_ptOwner;

   void ledgerOwned( const Identifier& aOwned );
   void ledgerOwned( const Address& aOwned );
   void ledgerPresent( const Identifier& aOwned );
   void unledgerOwned( const Identifier& aOwned );
   void unledger( CollectionTree::CollectionNode* node, bool pres = false );
   void unledgerPresent( const Identifier& aOwned );
};

