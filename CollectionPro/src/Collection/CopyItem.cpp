#include "../stdafx.h"
#include "CopyItem.h"
#include "CollectionObject.h"
#include "../Config.h"
#include "../Addressing/Addresser.h"

#include <time.h>
#include <sstream>
#include <cstdlib>

using namespace std;

CopyItem::CopyItem( const Identifier& aAddrParentIdentifier,
                    CollectionObject* aptClass )
   : m_ptCollectionObject( aptClass )
{
   Addresser addr;
   Config* config = Config::Instance();

   SetParent(aAddrParentIdentifier);

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
   : CopyItem(aAddrParentIdentifier, aptClass)
{
   for ( auto attr : alstMetaTags )
   {
      SetMetaTag( attr.first, attr.second, 
                  DetermineMetaTagType(attr.first), 
                  false );
   }
}

// Returns the hash. Hashes on parent, PUBLIC (so not the parent TAG) metatags, and the idattrs.
string CopyItem::GetHash(HashType aiHashType)
{
   auto oMetaHash = m_lstMetaTags.find(Config::HashKey);

   if ( ( oMetaHash == m_lstMetaTags.end() ) ||
        ( m_bNeedHash ) || 
        ( aiHashType != HashType::Default ) )
   {
      string szHashString = m_Address.GetFullAddress();
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

bool 
CopyItem::IsVirtual() const
{
   return m_Address.GetFullAddress() == "";
}

string CopyItem::GetParent() const
{
   return m_Address.GetFullAddress();
}

Address CopyItem::GetAddress() const
{
   return m_Address;
}

// Returns true if the location is specifically within the address of this copy.
bool CopyItem::IsParent(const Location& aAddrParent) const
{
   // We use 'IsSpecifiedBy' instead of 'ContainsLocation' because we don't want
   // Copies that are in a subset of some subaddress to be automaticall included
   // in this result.
   // If the location is designated by this copies address,
   // this copies lies within that location.
   return aAddrParent.IsSpecifiedBy(m_Address);
}

// Sets the parent address AND adds it to residents
void CopyItem::SetParent( const Identifier& aAddrTestAddress )
{
   string szParent = aAddrTestAddress.GetFullAddress();
   setParent( szParent );
   setMetaTag( GetAddressKey(), szParent, Public, false );
   
   int iFamily = findFamilyMember(aAddrTestAddress);
   if( iFamily != -1 )
   {
      m_lstResidentIn[iFamily] = aAddrTestAddress.ToAddress();
   }
   else
   {
      AddResident(aAddrTestAddress);
   }

   itemChanged();
}

// This will detect if the adding 'resident' is a subset of the parent,
// if so, it will adjust the parent address.
void CopyItem::AddResident(const Identifier& aAddrAddress)
{
   m_Address.MergeIdentifier(aAddrAddress);

   bool AddedToRef = false;
   for( int i = 0; i < m_lstResidentIn.size(); i++ )
   {
      auto address = &m_lstResidentIn[i];
      AddedToRef |= address->MergeIdentifier( aAddrAddress );
   }

   if( !AddedToRef )
   {
      m_lstResidentIn.push_back(aAddrAddress.ToAddress());
   }
}

// Returns the number of collection chains this copy is referenced in.
// Main collection counts as one.
int 
CopyItem::RemoveResident( const Identifier& aAddrAddress,
                          RemoveAddressType aiRemoveType )
{
   Address removeAddress(aAddrAddress.GetFullAddress());
   if( aiRemoveType == Family )
   {
      removeAddress = aAddrAddress.GetBase();
   }

   m_Address.ExtractIdentifier(removeAddress);

   for( int i = 0; i < m_lstResidentIn.size(); i++ )
   {
      if( m_lstResidentIn.at(i).ExtractIdentifier( removeAddress ) )
      {
         if( m_lstResidentIn.at(i).IsEmpty() )
         {
            // This is OK because we stop after this.
            m_lstResidentIn.erase( m_lstResidentIn.begin() + i );
         }
         break;
      }
   }

   // If there are no residents left, this copy item becomes virtual.
   if( m_Address.IsEmpty() )
   {
      m_Address = Address();
   }
 
   return ( m_Address.IsEmpty() ? 0 : 1 ) + m_lstResidentIn.size();
}

std::vector<Address> CopyItem::GetResidentIn() const
{
   return m_lstResidentIn;
}

// Returns true if this card is located in aAddrTest or if
// it is referenced in aAddrTest.
bool CopyItem::IsResidentIn(const Location& aAddrTest) const
{
   bool isResident = aAddrTest.IsSpecifiedBy(m_Address);

   if( !isResident )
   {
      isResident |= IsReferencedBy(aAddrTest);
   }

   return isResident;
}

// Returns true if any resident references the input location.
bool CopyItem::IsReferencedBy(const Location& aAddrTest) const
{
   bool isResident = false;

   for( auto resident : m_lstResidentIn )
   {
      isResident |= aAddrTest.IsSpecifiedBy(resident);
      if( isResident ){ break; }
   }

   return isResident;
}


shared_ptr<CopyItem>
CopyItem::CreateCopyItem( CollectionObject* aoConstructor,
                          const Identifier& aAddrParentIdentifier,
                          const std::vector<Tag>& alstIDAttrs,
                          const std::vector<Tag>& alstMetaTags )
{
   auto newCopy = shared_ptr<CopyItem>(new CopyItem( aAddrParentIdentifier, alstMetaTags, aoConstructor ));

   aoConstructor->SetIdentifyingTraitDefaults(newCopy);

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
      setParent( aszVal );
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

vector<Tag> CopyItem::GetIdentifyingAttributes() const
{
   return vector<Tag>(m_lstIdentifyingTags.begin(), m_lstIdentifyingTags.end());
}


void CopyItem::itemChanged()
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

// Does not update session
void CopyItem::setParent(string aszNewParent)
{
   Address newAddress( aszNewParent );

   m_Address = newAddress;
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

int
CopyItem::findFamilyMember( const Identifier& aId ) const
{
   for( int i = 0; i < 0; i++ )
   {
      auto address = m_lstResidentIn.at(i);
      if( address.GetMain() == aId.GetMain() )
      {
         return i;
      }
   }

   return -1;
}