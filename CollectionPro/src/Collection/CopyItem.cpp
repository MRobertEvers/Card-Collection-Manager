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
   : m_pBase( aptClass )
{
   // Set the chain ID and session here. 
   // If one is set later, it will just overwrite this...
   Config* config = Config::Instance();
   SetUID( config->GetHexID( Addresser::GetRandom() ));

   m_Fields = std::make_unique<CardFieldCollection>( m_pBase->GetIdentifyingTraits() );

   itemChanged(); // called by setParent.

   m_bNeedHash = true;
}


CopyItem::~CopyItem()
{
   m_mapMetaTags.clear();
}

CopyItem& 
CopyItem::operator =( const CopyItem & aCopy )
{
   this->m_mapMetaTags = aCopy.m_mapMetaTags;
   this->m_pBase = aCopy.m_pBase;
   this->m_bNeedHash = aCopy.m_bNeedHash;

   AddressBook* addrBook = new AddressBook( aCopy.m_ptAddressBook->GetAddress() );
   this->m_ptAddressBook = shared_ptr<AddressBook>( addrBook );
   this->m_Fields = std::make_unique<CardFieldCollection>( *aCopy.m_Fields.get() );
   return *this;
}

CopyItem::CopyItem( const CopyItem& aCopy )
{
   *this = aCopy;
}

// Returns the hash. Hashes on parent, PUBLIC (so not the parent TAG) metatags, and the idattrs.
string 
CopyItem::GetHash() const
{
   if( m_bNeedHash )
   {
      CopyItem* self = const_cast<CopyItem*> (this);
      string szHashString = m_ptAddressBook->GetParent();

      auto iter_Tags = m_Fields->begin();
      for (; iter_Tags != m_Fields->end(); ++iter_Tags)
      {
         // This requires that the tags have an ordering.
         // This ordering can be determined, by the order
         // of the MetaTag object in the collection object.
         szHashString += iter_Tags->second.GetValue();
      }
      
      vector<Tag> lstMetaList = this->GetMetaTags( MetaTag::Public );
      vector<Tag>::iterator iter_MetaTags = lstMetaList.begin();
      for (; iter_MetaTags != lstMetaList.end(); ++iter_MetaTags)
      {
         szHashString += iter_MetaTags->first + iter_MetaTags->second;
      }
         
      // Calculate the hash.
      string szHash = Config::Instance()->GetHash(szHashString);
      self->SetMetaTag(MetaTag::GetHashKey(), szHash, MetaTag::Hidden );
      self->m_bNeedHash = false;
   }

   return GetMetaTag( MetaTag::GetHashKey(), MetaTag::Hidden );
}

string 
CopyItem::GetSession() const
{
   return GetMetaTag( MetaTag::GetSessionKey(), MetaTag::Tracking );
}

string 
CopyItem::GetUID() const
{
   return GetMetaTag( MetaTag::GetUIDKey(), MetaTag::Tracking );
}

void 
CopyItem::SetUID( const string & aszUID )
{
   SetMetaTag( MetaTag::GetUIDKey(), aszUID, MetaTag::Tracking );
}

CollectionObject* 
CopyItem::GetObject() const
{
   return m_pBase;
}

Address 
CopyItem::GetAddress() const
{
   return m_ptAddressBook->GetAddress();
}

bool 
CopyItem::IsVirtual()
{
   return m_ptAddressBook->IsVirtual();
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


void 
CopyItem::SetMetaTag( const string& aszKey, 
                      const string& aszVal, 
                      MetaTag::Type atagType )
{
   auto iter_tag = m_mapMetaTags.find( aszKey );
   if( iter_tag == m_mapMetaTags.end() )
   {
      m_mapMetaTags.emplace( aszKey, MetaTag( aszKey, aszVal, atagType ));
   }
   else
   {
      iter_tag->second.SetVal( aszVal, atagType );
   }

   if( MetaTag::DetermineMetaTagType( aszKey ) == MetaTag::Public )
   {
      m_bNeedHash = true;
      itemChanged();
   }
}

string
CopyItem::GetMetaTag( const string& aszKey, MetaTag::Type atagType ) const
{
   auto oFound = m_mapMetaTags.find( aszKey );
   if( oFound != m_mapMetaTags.end() )
   {
      return oFound->second.GetVal( atagType );
   }
   else
   {
      return Config::NotFoundString;
   }
}

vector<Tag>
CopyItem::GetMetaTags( MetaTag::Type atagType ) const
{
   vector<Tag> lstRetVal;
   auto iter_Tags = m_mapMetaTags.cbegin();
   for( ; iter_Tags != m_mapMetaTags.cend(); ++iter_Tags )
   {
      if( iter_Tags->second.CanView( atagType ) )
      {
         auto oPair = make_pair( iter_Tags->second.GetKey(),
                                 iter_Tags->second.GetVal() );
         lstRetVal.push_back( oPair );
      }
   }

   return lstRetVal;
}

bool 
CopyItem::SetAttributes( const std::vector<Tag>& avecAttrs )
{
   return m_bNeedHash = m_Fields->SetAttributes( avecAttrs );
}

bool
CopyItem::SetAttribute( const string& aszKey,
                        const string& aszValue )
{
   return m_bNeedHash = m_Fields->SetAttribute( aszKey, aszValue );
}

shared_ptr<CopyItem>
CopyItem::CreateCopyItem( CollectionObject* aoConstructor,
                          const Identifier& aAddrParentIdentifier )
{
   auto newCopy = shared_ptr<CopyItem>(new CopyItem( aAddrParentIdentifier, aoConstructor ));

   newCopy->SetAddressBook( shared_ptr<AddressBook>( 
      new AddressBook( aAddrParentIdentifier ) )
   );

   return newCopy;
}


string 
CopyItem::GetAttribute(const string& aszKey)
{
   auto szAttr = GetObject()->GetCommonTrait( aszKey );
   if( szAttr != "" )
   {
      return szAttr;
   }

   return m_Fields->GetAttribute( aszKey );
}

vector<Tag>
CopyItem::GetIdentifyingAttributes() const
{
   return m_Fields->GetIdentifyingAttributes();
}

void 
CopyItem::SetAddressBook( shared_ptr<AddressBook> aptBook )
{
   m_ptAddressBook = aptBook;
}

string 
CopyItem::ToString()
{
   return CollectionObject::ToCardLine(
      GetAddress(),
      GetObject()->GetName(),
      GetIdentifyingAttributes(),
      GetMetaTags(MetaTag::Any)
   );
}

void 
CopyItem::itemChanged()
{
   long lChangeTime = time(nullptr);
   stringstream ss;
   ss << hex << lChangeTime ;
   SetMetaTag( MetaTag::GetSessionKey(), ss.str(), MetaTag::Tracking );
}
