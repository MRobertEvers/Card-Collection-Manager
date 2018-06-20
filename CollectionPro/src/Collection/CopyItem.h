#pragma once
#include "MetaTag.h"
#include "TraitItem.h"
#include "AddressBook.h"
#include "../Addressing/Addresser.h"

#include <string>
#include <set>
#include <vector>
#include <map>
#include <memory>

class CollectionObject;

// Each Copy Item has a Unique ID and a Class Hash ID and its last modified session date.
// UID is a 3 digit hex number, this is also called the Chain ID.
// Hash ID is an MD5 has of various traits.
// Last modified session date is an integer.
class CopyItem
{
private:
   // Use static constructor to build this class.
   CopyItem( const Identifier& aAddrParentIdentifier, CollectionObject* aptClass );

public:
   CopyItem( const CopyItem& aCopy );
   ~CopyItem();

   std::string GetHash() const;
   std::string GetSession() const;
   Address GetAddress() const;
   std::string GetUID() const;
   void SetUID( const std::string& aszUID );
   CollectionObject* GetObject() const;

   void AddResident( const Identifier& aAddrAddress );
   int RemoveResident( const Identifier& aAddrAddress,
                       unsigned int aiRemoveType = 0 );

   void SetMetaTag( const std::string& aszKey,
                    const std::string& aszVal,
                    MetaTag::Type atagType = MetaTag::Public );
   std::string GetMetaTag(const std::string& aszKey, MetaTag::Type atagType) const;
   std::vector<Tag> GetMetaTags(MetaTag::Type atagType) const;

   // This expects that the input is valid. Does not verify that
   // the value is an allowed value.
   bool SetAttribute( const std::string& aszKey,
                      const std::string& aszValue );

   std::string GetAttribute(const std::string& aszKey);
   std::vector<Tag> GetIdentifyingAttributes() const;

   void SetAddressBook( std::shared_ptr<AddressBook> aptBook );

private:
   bool m_bNeedHash;

   // I didn't want to do this. But here I am.
   CollectionObject* m_pBase;

   std::shared_ptr<AddressBook> m_ptAddressBook;

   void itemChanged();

   std::map<std::string, MetaTag> m_mapMetaTags;
   std::map<std::string, CardInstanceField> m_mapIdentifyingTraits;
public:
   static std::shared_ptr<CopyItem> CreateCopyItem( CollectionObject* aoConstructor,
                                                    const Identifier& aAddrParentIdentifier );
};

