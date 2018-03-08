#include "../stdafx.h"
#include <algorithm>
#include "CollectionObject.h"
#include "CopyItem.h"

#include "../Config.h"
#include "../StringInterface.h"


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
                                    const vector<TraitItem>& alstRestrictions )
{
   m_szName          = aszItemName;
   m_lstCommonTraits = map<string,string>(alstCommon.begin(), alstCommon.end());

   for( auto trait : alstRestrictions ) {
      m_lstIdentifyingTraits.insert(make_pair(trait.GetKeyName(), trait));
   }
}


CollectionObject::~CollectionObject()
{
   m_lstCopies.clear();
}

std::shared_ptr<CopyItem>
CollectionObject::CreateCopy( const Identifier& aAddrColID,
                              const vector<Tag>& alstAttrs,
                              const vector<Tag>& alstMetaTags ) const
{
   return std::shared_ptr<CopyItem>(CopyItem::CreateCopyItem( this, aAddrColID, alstAttrs, alstMetaTags ));
}

shared_ptr<CopyItem>
CollectionObject::AddCopy( const Location& aAddrColID,
                           const vector<Tag>& alstAttrTags,
                           const vector<Tag>& alstMetaTags )
{
   auto newCopy = CreateCopy(aAddrColID, alstAttrTags, alstMetaTags);
   return AddCopy( newCopy );
}

shared_ptr<CopyItem> 
CollectionObject::AddCopy( const shared_ptr<CopyItem>& aCopy )
{
   m_lstCopies.push_back( aCopy );
   return aCopy;
}

bool 
CollectionObject::RemoveCopy( const Location& aAddrColID,
                              const string aszUniqueID )
{
   shared_ptr<CopyItem> ptCopy;
   auto nCopy = FindCopy( aszUniqueID );

   if( nCopy.Good() )
   {
      ptCopy = *nCopy.Value();
      int iRefCnt = ptCopy->RemoveResident( (const Location)aAddrColID );

      if( iRefCnt == 0 )
      {
         DeleteCopy( ptCopy );
      }
      return true;
   }
   return false;
}

string 
CollectionObject::GenerateHash( const Identifier& aAddrIdentifier,
                              const vector<Tag>& alstAttrs,
                              const vector<Tag>& alstMetaTags ) const
{
   auto hashCopy = CreateCopy( aAddrIdentifier, alstAttrs, alstMetaTags );
   return hashCopy->GetHash();
}

void 
CollectionObject::DeleteCopy( const string& aszUniqueHash )
{
   auto copy = FindCopy( aszUniqueHash );
   if( copy.Good() )
   {
      DeleteCopy( *copy.Value() );
   }
}

void 
CollectionObject::DeleteCopy( shared_ptr<CopyItem> aptCopy )
{
   auto iter_found = find( m_lstCopies.begin(), 
                           m_lstCopies.end(), 
                           aptCopy );

   if ( iter_found != m_lstCopies.end() )
   {
      m_lstCopies.erase(iter_found);
   }
}

string 
CollectionObject::CopyToString( CopyItem const* aptItem,
                                const MetaTagType& aAccessType,
                                const Identifier& aAddrCompareID ) const
{
   return ToCardLine( aptItem->GetAddress(), GetName(),
                      aptItem->GetIdentifyingAttributes(),
                      aptItem->GetMetaTags(aAccessType),
                      aAddrCompareID );
}

TryGetCopy<shared_ptr<CopyItem>>
CollectionObject::FindCopy( const string& aszUID, 
                            FindType aiType  ) const
{
   return FindCopy( aszUID, aiType, m_lstCopies );
}

TryGetCopy<shared_ptr<CopyItem>> 
CollectionObject::FindCopy( const string& aszUID, FindType aiType,
                            const vector<shared_ptr<CopyItem>>& avecCopies ) const
{
   TryGetCopy<shared_ptr<CopyItem>> oRetval;
   bool match = false;
   for( auto ptCopy : avecCopies )
   {
      match |= (aiType & UID)  > 0 && ptCopy->GetUID() == aszUID;
      match |= (aiType & Hash) > 0 && ptCopy->GetHash() == aszUID;
      if( match )
      {
         oRetval.Set( shared_ptr<CopyItem>( ptCopy ) );
         break;
      }
   }

   return oRetval;
}

// Returns each copy
vector<shared_ptr<CopyItem>> 
CollectionObject::FindCopies( const Identifier& aCollection,
                              CollectionObjectType aSearchType ) const
{
   vector<shared_ptr<CopyItem>> copies;
   for( auto iSub : aCollection.GetAddresses() )
   {
      Location location = Location(aCollection.GetMain(), iSub);

      auto newCopies = FindCopies(location, aSearchType);
      for( auto copy : newCopies )
      {
         bool bAlreadyHave = false;
         for( auto alreadyCopy : copies )
         {
            bAlreadyHave |= copy.get() == alreadyCopy.get();
         }

         if( !bAlreadyHave )
         {
            copies.push_back(copy);
         }
      }
   }

   return copies;
}

vector<shared_ptr<CopyItem>> 
CollectionObject::FindCopies( const Location& aCollection,
                            CollectionObjectType aSearchType ) const
{
   vector<shared_ptr<CopyItem>> lstRetVal;
   for( auto copy : m_lstCopies )
   {
      if( ( aSearchType & Local )         &&
          ( copy->IsParent(aCollection) ) )
      {
         lstRetVal.push_back(copy);
      }
      else if( ( aSearchType & Borrowed )        &&
               ( !copy->IsParent(aCollection) )  && 
               ( copy->IsResidentIn(aCollection) ) )
      {
         lstRetVal.push_back(copy);
      }
      else if( ( aSearchType & Virtual )         &&
               ( copy->IsVirtual() )             &&
               ( copy->IsResidentIn(aCollection) ) )
      {
         lstRetVal.push_back(copy);
      }
   }

   return lstRetVal;
}


string 
CollectionObject::GetName() const
{
   return m_szName;
}

string 
CollectionObject::GetProtoType() const
{
   StringInterface strIface;
   string szResult;
   
   // Start with static common traits
   vector<Tag> lstAllCommonTraits( m_lstCommonTraits.begin(),
                                   m_lstCommonTraits.end() );

   // Include identifying traits
   // These appear as <Key>, *Val1::Val2...
   for( auto trait : m_lstIdentifyingTraits )
   {
      auto vals = trait.second.GetAllowedValues();
      strIface.ListToDelimStr(vals.cbegin(), vals.cend(), szResult);
      
      lstAllCommonTraits.push_back(make_pair(trait.second.GetKeyName(), szResult));
   }

   return CollectionObject::ToCardLine(Address(), "", lstAllCommonTraits);
}

string 
CollectionObject::GetCommonTrait(const string& aszTrait) const
{
   /*
   // I want this to be case insensitive so just search through all
   string szSearch = aszTrait;
   transform(szSearch.begin(), szSearch.end(), szSearch.begin(), ::tolower);
   for( auto Trait : m_lstCommonTraits )
   {
      string szTrait = Trait.first;
      transform(szTrait.begin(), szTrait.end(), szTrait.begin(), ::tolower);
      if( szTrait == aszTrait )
      {
         return Trait.second;
      }
   }*/
   
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

std::map<string, TraitItem>
CollectionObject::GetIdentifyingTraits()
{
   return m_lstIdentifyingTraits;
}

// Returns the first trait key that has the input value
bool 
CollectionObject::MatchIdentifyingTrait( const std::string& aszValue, 
                                         std::string& rszKey )
{
   for( auto trait : m_lstIdentifyingTraits )
   {
      auto values = trait.second.GetAllowedValues();
      auto iter_found = find( values.begin(),
                              values.end(),
                              aszValue);
      if( iter_found != values.end() )
      {
         rszKey = trait.second.GetKeyName();
         return true;
      }
   }

   return false;
}

bool 
CollectionObject::SetIdentifyingTrait( CopyItem* aptItem,
                                     const string& aszTraitKey,
                                     const string& aszTraitValue,
                                     bool bSession ) const
{
   return SetIdentifyingTrait( aptItem, aszTraitKey, aszTraitValue, vector<string>(), bSession );
}

bool 
CollectionObject::SetIdentifyingTrait( CopyItem* aptItem,
                                       const string& aszTraitKey,
                                       const string& aszTraitValue,
                                       const vector<string> avecUpComingTraits,
                                       bool bSession ) const
{
      auto iter_found = m_lstIdentifyingTraits.find(aszTraitKey);
   if( iter_found == m_lstIdentifyingTraits.end() ) {
      Debug::Log("CollectionObject", "Treid to set non-existant trait");
      return false; 
   }

   TraitItem trait = iter_found->second;
   auto vecAllowVals = trait.GetAllowedValues();
   auto iter_found_val = find( vecAllowVals.begin(),
                               vecAllowVals.end(),
                               aszTraitValue );
   if( iter_found_val == vecAllowVals.end()) {
      Debug::Log("CollectionObject", "Treid to set trait to unallowed value");
      return false; 
   }

   // Set the trait
   aptItem->SetIdentifyingAttribute( aszTraitKey, aszTraitValue, bSession );
   setCopyPairAttrs( aptItem, aszTraitKey,
                     distance(vecAllowVals.begin(), iter_found_val),
                     avecUpComingTraits );

   return true;
}

// Sets all the ident traits to their defaults.
void 
CollectionObject::SetIdentifyingTraitDefaults( CopyItem* aptItem ) const
{
   // Include default values for IDAttrs NOT specified.
   for( auto IDAttrsPair : m_lstIdentifyingTraits )
   {
      auto IDAttrs = IDAttrsPair.second;
      SetIdentifyingTrait( aptItem, IDAttrs.GetKeyName(), IDAttrs.GetDefaultValue(), false );
   }
}


// Does not update session.
void 
CollectionObject::setCopyPairAttrs( CopyItem* aptItem, const string& aszKey, int iVal ) const
{
   setCopyPairAttrs( aptItem, aszKey, iVal, vector<string>() );
}

void 
CollectionObject::setCopyPairAttrs( CopyItem* aptItem, 
                                    const string& aszKey, int iVal,
                                    vector<string> avecSkipAttrs ) const
{
   vector<string> lstPartners;

   // Find any traits paied with the key.
   vector<Tag> lstPairs = Config::Instance()->GetPairedKeysList();
   for( Tag var : lstPairs )
   {
      string szSearch = "";
      if( var.first == aszKey )
      {
         szSearch = var.second;
      }
      else if( var.second == aszKey )
      {
         szSearch = var.first;
      }

      if( szSearch != "" )
      {
         auto iter_found = find( lstPartners.begin(), lstPartners.end(), szSearch );
         if( iter_found == lstPartners.end() )
         {
            lstPartners.push_back( szSearch );
         }
      }
   }

   // Search for the trait and asign it.
   for( string szKey : lstPartners )
   {
      if( find( avecSkipAttrs.begin(), avecSkipAttrs.end(), szKey ) != avecSkipAttrs.end() )
      {
         continue;
      }
      
      // Verify the trait is an identifying trait.
      auto iter_attr = m_lstIdentifyingTraits.find( szKey );
      if( iter_attr != m_lstIdentifyingTraits.end() )
      {
         TraitItem foundTrait = iter_attr->second;
         aptItem->SetIdentifyingAttribute( szKey, foundTrait.GetAllowedValues().at( iVal ), false );
      }
   }
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
   StringInterface parser;
   Config* config = Config::Instance();
   string szAddressKey = CopyItem::GetAddressKey();
   multimap<string, string> mapMetaTags(alstMetaTags.begin(), alstMetaTags.end());

   // Don't specify the parent if the parent is requesting the
   // the string.
   bool bExcludeParent = true;

   // If the parent is empty and the compare is default(empty) then the
   // parent will not be included.
   bExcludeParent = aAddrCompareID.IsEmpty() || aAddrCompareID == aAddrParentID;

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
      auto pairParent = make_pair(szAddressKey, aAddrParentID.GetFullAddress());
      mapMetaTags.insert(pairParent);
   }

   string szRetVal;
   if( aiCount > 0 )
   {
      szRetVal += "x" + to_string(aiCount) + " ";
   }

   szRetVal += parser.ToCardLine( aszName, alstAttrs,
                                  vector<Tag>(mapMetaTags.begin(), mapMetaTags.end()) );

   return szRetVal;
}

