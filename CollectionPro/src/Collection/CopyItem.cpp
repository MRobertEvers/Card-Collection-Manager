#include "../stdafx.h"
#include "CopyItem.h"
#include "CollectionObject.h"
#include "../Config.h"
#include "../Addressing/Addresser.h"

#include <time.h>
#include <sstream>
#include <cstdlib>

using namespace std;

CopyItem::CopyItem( const Identifier& aAddrParentIdentifier, CollectionObject* aptClass )
   : m_ptCollectionObject( aptClass )
{
   Addresser addr;
   Config* config = Config::Instance();

   // Set the chain ID and session here. 
   // If one is set later, it will just overwrite this...
   setUID(config->GetHexID(addr.GetRandom()));
   
   // itemChanged(); called by setParent.

   m_bNeedHash = true;
}


CopyItem::~CopyItem()
{
   m_lstMetaTags.clear();
}

CopyItem::CopyItem( const Identifier& aAddrParentIdentifier,
                    const std::vector<Tag>& alstMetaTags,
                    CollectionObject* aptClass )
   : CopyItem( aAddrParentIdentifier, aptClass)
{
   for ( auto& attr : alstMetaTags )
   {
      SetMetaTag( attr.first, attr.second, 
                  DetermineMetaTagType(attr.first), 
                  false );
   }
}

CopyItem::CopyItem( const CopyItem& aCopy )
{
   this->m_lstIdentifyingTags = aCopy.m_lstIdentifyingTags;
   this->m_lstMetaTags = aCopy.m_lstMetaTags;
   this->m_ptCollectionObject = aCopy.m_ptCollectionObject;
   this->m_bNeedHash = aCopy.m_bNeedHash;

   AddressBook* addrBook = new AddressBook(aCopy.m_ptAddressBook->GetAddress());
   this->m_ptAddressBook = std::shared_ptr<AddressBook>( addrBook );
}

// Returns the hash. Hashes on parent, PUBLIC (so not the parent TAG) metatags, and the idattrs.
string 
CopyItem::GetHash(HashType aiHashType)
{
   auto oMetaHash = m_lstMetaTags.find(Config::HashKey);

   if ( ( oMetaHash == m_lstMetaTags.end() ) ||
        ( m_bNeedHash ) || 
        ( aiHashType != HashType::Default ) )
   {
      string szHashString = m_ptAddressBook->GetParent();
      if( ( aiHashType & HashType::Ids ) == Ids )
      {
         auto iter_Tags = m_lstIdentifyingTags.begin();
         for (; iter_Tags != m_lstIdentifyingTags.end(); ++iter_Tags)
         {
           // This requires that the tags have an ordering.
           // This ordering can be determined, by the order
           // of the MetaTag object in the collection object.
           szHashString += iter_Tags->second;
         }
      }

      // Only iterate of public metatags
      if( ( aiHashType & HashType::Meta ) == Meta )
      {
         vector<Tag> lstMetaList = this->GetMetaTags(Public);
         vector<Tag>::iterator iter_MetaTags = lstMetaList.begin();
         for (; iter_MetaTags != lstMetaList.end(); ++iter_MetaTags)
         {
            szHashString += iter_MetaTags->first + iter_MetaTags->second;
         }
      }
      
      // Calculate the hash.
      string szHash = Config::Instance()->GetHash(szHashString);
      
      if( aiHashType == HashType::Default )
      {
         SetMetaTag(Config::HashKey, szHash, Hidden, false);
      }

      m_bNeedHash = false;
      return szHash;
   }
   else
   {
      return oMetaHash->second.GetVal();
   }
}

std::string CopyItem::GetSession() const
{
   return GetMetaTag( GetSessionKey(), Tracking );
}

std::string CopyItem::GetUID() const
{
   return GetMetaTag( GetUIDKey(), Tracking );
}

CollectionObject* 
CopyItem::GetObject() const
{
   return m_ptCollectionObject;
}

bool 
CopyItem::IsVirtual() const
{
   return m_ptAddressBook->IsVirtual();
}

string 
CopyItem::GetParent() const
{
   return m_ptAddressBook->GetParent();
}

Address 
CopyItem::GetAddress() const
{
   return m_ptAddressBook->GetAddress();
}

// Returns true if the location is specifically within the address of this copy.
bool CopyItem::IsParent(const Location& aAddrParent) const
{
   // We use 'IsSpecifiedBy' instead of 'ContainsLocation' because we don't want
   // Copies that are in a subset of some subaddress to be automaticall included
   // in this result.
   // If the location is designated by this copies address,
   // this copies lies within that location.
   return m_ptAddressBook->IsParent( aAddrParent );
}

// Sets the parent address AND adds it to residents
void CopyItem::SetParent( const Identifier& aAddrTestAddress )
{
   string szParent = aAddrTestAddress.GetFullAddress();
   
   m_ptAddressBook->SetParent( aAddrTestAddress );

   setMetaTag( GetAddressKey(), szParent, Public, false );
   itemChanged();
}

// This will detect if the adding 'resident' is a subset of the parent,
// if so, it will adjust the parent address.
void CopyItem::AddResident(const Identifier& aAddrAddress)
{
   return m_ptAddressBook->AddResident( aAddrAddress );
}

// Returns the number of collection chains this copy is referenced in.
// Main collection counts as one.
int 
CopyItem::RemoveResident( const Identifier& aAddrAddress,
                          unsigned int aiRemoveType )
{
   return m_ptAddressBook->RemoveResident( aAddrAddress, (AddressBook::RemoveAddressType)aiRemoveType );
}

std::vector<Address> 
CopyItem::GetResidentIn() const
{
   return m_ptAddressBook->GetResidentIn();
}

// Returns true if this card is located in aAddrTest or if
// it is referenced in aAddrTest.
bool 
CopyItem::IsResidentIn(const Location& aAddrTest) const
{
   return m_ptAddressBook->IsResidentIn( aAddrTest );
}

// Returns true if any resident references the input location.
bool 
CopyItem::IsReferencedBy(const Location& aAddrTest) const
{
   return m_ptAddressBook->IsReferencedBy( aAddrTest );
}


shared_ptr<CopyItem>
CopyItem::CreateCopyItem( CollectionObject* aoConstructor,
                          const Identifier& aAddrParentIdentifier,
                          const std::vector<Tag>& alstIDAttrs,
                          const std::vector<Tag>& alstMetaTags )
{
   auto newCopy = shared_ptr<CopyItem>(new CopyItem( aAddrParentIdentifier, alstMetaTags, aoConstructor ));

   aoConstructor->SetIdentifyingTraitDefaults(newCopy);

   newCopy->SetAddressBook( std::shared_ptr<AddressBook>( 
      new AddressBook( aAddrParentIdentifier ) )
   );

   // Create a list of all the traits that are being set.
   // This is used so that if "Paired" traits are being set
   // we don't start overwriting ourselves.
   vector<string> vecUpComingTraits;
   for( auto& pairTraits : alstIDAttrs )
   {
      vecUpComingTraits.push_back( pairTraits.first );
   }

   for( auto& IDTag : alstIDAttrs )
   {
      aoConstructor->SetIdentifyingTrait( newCopy, IDTag.first, IDTag.second, vecUpComingTraits );
   }

   return newCopy;
}

MetaTagType CopyItem::DetermineMetaTagType(string aszTagKey)
{
   MetaTagType mTagType = Public;
   if( ( aszTagKey.size() > 0 ) && 
       ( aszTagKey[0] == *Config::IgnoredTagId ) )
   {
      mTagType = Ignored;
   }

   if( ( mTagType == Ignored ) &&
       ( aszTagKey.size() > 1 ) &&
       ( aszTagKey[1] == *Config::TrackingTagId ) )
   {
      mTagType = Tracking;
   }
   return mTagType;
}

std::string CopyItem::GetUIDKey()
{
   return Config::TrackingTagId + std::string("UID");
}

std::string CopyItem::GetSessionKey()
{
   return Config::TrackingTagId + std::string("Session");
}

std::string CopyItem::GetHashKey()
{
   return Config::TrackingTagId + std::string("hash");
}

std::string CopyItem::GetAddressKey()
{
   return "Address";
}

std::string 
CopyItem::MakeIgnoredTag( const std::string aszTag )
{
   return Config::IgnoredTagId + aszTag;
}

std::string 
CopyItem::MakeTrackingTag( const std::string aszTag )
{
   return Config::TrackingTagId + aszTag;
}

void 
CopyItem::SetMetaTag( const std::string& aszKey,
                      const std::string& aszVal,
                      MetaTagType atagType,
                      bool bTimeChange )
{
   if( aszKey == GetAddressKey() )
   {
      m_ptAddressBook->SetParent( Address(aszVal) );
   }
   else
   {
      setMetaTag( aszKey, aszVal, atagType, bTimeChange );
   }
}

string CopyItem::GetMetaTag(const std::string& aszKey, MetaTagType atagType) const
{
   auto oFound = m_lstMetaTags.find(aszKey);
   if( oFound != m_lstMetaTags.end() )
   {
      return oFound->second.GetVal(atagType);
   }
   else
   {
      return Config::NotFoundString;
   }
}

vector<Tag> CopyItem::GetMetaTags(MetaTagType atagType) const
{
   vector<Tag> lstRetVal;
   auto iter_Tags = m_lstMetaTags.cbegin();
   for (; iter_Tags != m_lstMetaTags.cend(); ++iter_Tags)
   {
      if (iter_Tags->second.CanView(atagType))
      {
         auto oPair = make_pair( iter_Tags->second.GetKey(),
                                 iter_Tags->second.GetVal() );
         lstRetVal.push_back(oPair);
      }
   }

   return lstRetVal;
}

bool
CopyItem::SetIdentifyingAttribute( const std::string& aszKey,
                                   const std::string& aszValue,
                                   bool bTimeChange )
{
   if (bTimeChange) { itemChanged(); }
   auto oFound = m_lstIdentifyingTags.find(aszKey);

   if( oFound == m_lstIdentifyingTags.end() )
   {
      m_lstIdentifyingTags.insert( std::make_pair( aszKey, aszValue ) );
   }
   else
   {
      oFound->second = aszValue;
   }

   return true;
}

string CopyItem::GetIdentifyingAttribute(string aszKey)
{
   auto oFound = m_lstIdentifyingTags.find(aszKey);
   if( oFound != m_lstIdentifyingTags.end() )
   {
      return oFound->second;
   }
   else
   {
      return Config::NotFoundString;
   }
}

vector<Tag>
CopyItem::GetIdentifyingAttributes() const
{
   return vector<Tag>(m_lstIdentifyingTags.begin(), m_lstIdentifyingTags.end());
}

void 
CopyItem::SetAddressBook( std::shared_ptr<AddressBook> aptBook )
{
   m_ptAddressBook = aptBook;
}

void 
CopyItem::itemChanged()
{
   long lChangeTime = time(nullptr);
   stringstream ss;
   ss << hex << lChangeTime ;
   SetMetaTag(GetSessionKey(), ss.str(), MetaTagType::Tracking, false);
}

// Does not update session.
void CopyItem::setUID(string aszNewID)
{
   SetMetaTag(GetUIDKey(), aszNewID, MetaTagType::Tracking, false);
}

void 
CopyItem::setMetaTag( const std::string& aszKey,
                      const std::string& aszVal,
                      MetaTagType atagType,
                      bool bTimeChange )
{
   if (bTimeChange) { itemChanged(); }

   auto oFound = m_lstMetaTags.find(aszKey);
   if( oFound == m_lstMetaTags.end() )
   {
      MetaTag newMeta(aszKey, aszVal, atagType);
      auto oNewPair = make_pair(aszKey, newMeta);
      m_lstMetaTags.insert(oNewPair);
   }
   else if (oFound->second.CanView(atagType))
   {
      // Delete the tag if the value is empty.
      if( aszVal == "" )
      {
         m_lstMetaTags.erase(oFound);
      }
      else
      {
         oFound->second.SetVal(aszVal);
      }
   }
   m_bNeedHash = true;
}
