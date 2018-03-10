#pragma once

#include "..\Addressing\Addresser.h"
#include <memory>
#include <vector>
#include <string>

class CopyItem;

class AddressBook
{
public:
   enum RemoveAddressType : int
   {
      Individual = 0x0,
      Family = 0x1
   };

public:
   AddressBook( const Identifier& aAddrParentIdentifier,
                std::shared_ptr<CopyItem> aptOwner );
   ~AddressBook();

   void SetParent( const Identifier& aAddrTestAddress );
   bool IsParent( const Location& aAddrTestAddress ) const;
   std::string GetParent() const;

   void AddResident( const Identifier& aAddrAddress );
   int RemoveResident( const Identifier& aAddrAddress,
                       RemoveAddressType aiRemoveType = Individual );

   bool IsResidentIn( const Location& aAddrAddress ) const;
   bool IsReferencedBy( const Location& aAddrAddress ) const;
   std::vector<Address> GetResidentIn() const;

   bool IsVirtual() const;
   Address GetAddress() const;

private:
   // Includes <Address>-<SubAddress1>,<SubAddress2>...
   // SubAddressX's smallest prime factor is the xth prime.
   Address m_Address;
   std::vector<Address> m_vecResidentIn;
   std::weak_ptr<CopyItem> m_ptOwner;

   void ledgerOwned(const Identifier& aOwned);
   void ledgerPresent( const Identifier& aOwned );
   void unledgerOwned( const Identifier& aOwned );
   void unledgerPresent( const Identifier& aOwned );

   void setParent( std::string aszNewParent );
   int findFamilyMember( const Identifier& aId ) const;
};