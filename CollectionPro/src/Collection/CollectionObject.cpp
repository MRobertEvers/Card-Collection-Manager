#include "../stdafx.h"
#include "CollectionObject.h"
#include "CopyItem.h"
#include "../Config.h"
#include "../StringInterface.h"
#include "LedgerBook.h"

#include <algorithm>

using namespace std;

CollectionObject::
PseudoIdentifier::PseudoIdentifier() {
}

CollectionObject::
PseudoIdentifier::PseudoIdentifier( unsigned int aiCount,
                                    string aszName,
                                    string aszDetails,
                                    string aszMeta ) {
   Count = aiCount;
   Name = aszName;
   DetailString = aszDetails;
   MetaString = aszMeta;

   StringInterface::ParseTagString(aszDetails, Identifiers);
   StringInterface::ParseTagString(aszMeta, MetaTags);
}

CollectionObject::
PseudoIdentifier::~PseudoIdentifier() {
}


CollectionObject::CollectionObject( const string& aszItemName, 
                                    const vector<Tag>& alstCommon,
                                    const vector<CardVariantField>& alstRestrictions )
{
   m_szName          = aszItemName;
   m_lstCommonTraits = map<string,string>(alstCommon.begin(), alstCommon.end());

   for( auto trait : alstRestrictions ) {
      m_lstIdentifyingTraits.insert(make_pair(trait.GetKeyName(), trait));
   }
}


CollectionObject::~CollectionObject()
{
   m_vecCopies.clear();
}

std::shared_ptr<CopyItem>
CollectionObject::CreateCopy( const Identifier& aAddrColID,
                              const vector<Tag>& alstAttrs,
                              const vector<Tag>& alstMetaTags )
{
   auto pCopy = CopyItem::CreateCopyItem( this, aAddrColID );

   pCopy->SetAttributes( alstAttrs );
  
   for( auto& attr : alstMetaTags )
   {
      pCopy->SetMetaTag( attr.first, attr.second );
   }

   return pCopy;
}

shared_ptr<CopyItem>
CollectionObject::AddCopy( const Location& aAddrColID,
                           const vector<Tag>& alstAttrTags,
                           const vector<Tag>& alstMetaTags )
{
   return AddCopy( CreateCopy( aAddrColID, alstAttrTags, alstMetaTags ) );
}

shared_ptr<CopyItem> 
CollectionObject::AddCopy( const shared_ptr<CopyItem>& aCopy )
{
   m_vecCopies.push_back( aCopy );
   ledgerCopy( aCopy );
   return aCopy;
}

bool 
CollectionObject::RemoveCopy( const Location& aAddrColID,
                              const string aszUniqueID )
{
   auto copy = FindCopy( aszUniqueID );
   bool bRes = copy->RemoveResident( aAddrColID );
   if( copy->IsVirtual() )
   {
      DeleteCopy( copy.Value() );
   }

   return bRes;
}

string 
CollectionObject::GenerateHash( const Identifier& aAddrIdentifier,
                                const vector<Tag>& alstAttrs,
                                const vector<Tag>& alstMetaTags )
{
   auto hashCopy = CreateCopy( aAddrIdentifier, alstAttrs, alstMetaTags );
   return hashCopy->GetHash();
}


void 
CollectionObject::DeleteCopy( shared_ptr<CopyItem> aptCopy )
{
   auto iter_found = find( m_vecCopies.begin(), 
                           m_vecCopies.end(), 
                           aptCopy );

   if ( iter_found != m_vecCopies.end() )
   {
      m_vecCopies.erase(iter_found);
   }
}

TryGetCopy<shared_ptr<CopyItem>>
CollectionObject::FindCopy( const string& aszUID ) const
{
   TryGetCopy<shared_ptr<CopyItem>> pRetval;
   for( auto& copy : m_vecCopies )
   {
      if( copy->GetUID() == aszUID )
      {
         pRetval.Set( copy );
         return pRetval;
      }
   }

   return pRetval;
}

// Returns each copy
vector<shared_ptr<CopyItem>> 
CollectionObject::FindCopies( const Location& aCollection ) const
{
   vector<shared_ptr<CopyItem>> pRetval;
   for( auto& copy : m_vecCopies )
   {
      if( aCollection.IsSuperset( copy->GetAddress() ) )
      {
         pRetval.push_back( copy );
      }
   }

   return pRetval;
}


string 
CollectionObject::GetName() const
{
   return m_szName;
}

string 
CollectionObject::GetCommonTrait(const string& aszTrait) const
{
   auto iter_trait = m_lstCommonTraits.find(aszTrait);
   if( iter_trait != m_lstCommonTraits.end() )
   {
      return iter_trait->second;
   }
   else
   {
      return "";
   }
}

void 
CollectionObject::ledgerCopy( shared_ptr<CopyItem> aptCopy )
{
   auto ptLedger = shared_ptr<LedgerBook>( 
      new LedgerBook( aptCopy->GetAddress(),
                      aptCopy ) 
      );

   aptCopy->SetAddressBook( ptLedger );
}

const std::map<string, CardVariantField>& 
CollectionObject::GetIdentifyingTraits() const
{
   return m_lstIdentifyingTraits;
}

bool
CollectionObject::ParseCardLine( const string& aszLine,
                                 PseudoIdentifier& rPIdentifier )
{
   string szName, szDetails, szMeta;
   unsigned int iCount;

   bool bGoodParse = StringInterface::ParseCardLine( aszLine, iCount, szName,
                                                     szDetails, szMeta );
   if( bGoodParse )
   {
      // Output the details
      rPIdentifier = PseudoIdentifier(iCount, szName, szDetails, szMeta);
   }
   else
   {
      return false;
   }

   return true;
}

// This function is agnostic of the card object. It assumes that the 
// parent is the input parent ID. If the parent is also specified in the 
// metatags, they should be the same.
string
CollectionObject::ToCardLine( const Identifier& aAddrParentID,
                              const string& aszName,
                              const vector<Tag>& alstAttrs,
                              const vector<Tag>& alstMetaTags,
                              const Identifier& aAddrCompareID,
                              const unsigned int aiCount)
{
   Config* config = Config::Instance();
   string szAddressKey = MetaTag::GetAddressKey();
   multimap<string, string> mapMetaTags(alstMetaTags.begin(), alstMetaTags.end());

   // Don't specify the parent if the parent is requesting the
   // the string.
   bool bExcludeParent = true;

   // If the parent is empty and the compare is default(empty) then the
   // parent will not be included.
   bExcludeParent = aAddrCompareID.IsEmpty() || aAddrCompareID.Compare( aAddrParentID ) == 0;

   // Check if the parent is specified in the metatags.
   auto iter_found = mapMetaTags.find(szAddressKey);

   // Remove the parent from meta tags if there is one.
   // Otherwise, add the parent.
   if( bExcludeParent )
   {
      if( iter_found != mapMetaTags.end() ) 
      {
         mapMetaTags.erase(iter_found);
      }
   }
   else if( iter_found == mapMetaTags.end() )
   {
      // Add the parent if it wasn't already found in the metatags.
      auto pairParent = make_pair(szAddressKey, aAddrParentID.ToString());
      mapMetaTags.insert(pairParent);
   }

   string szRetVal;
   if( aiCount > 0 )
   {
      szRetVal += "x" + to_string(aiCount) + " ";
   }

   szRetVal += StringInterface::ToCardLine( aszName, alstAttrs,
                                            vector<Tag>(mapMetaTags.begin(), mapMetaTags.end()) );

   return szRetVal;
}

