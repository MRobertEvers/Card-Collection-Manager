#include "CardInterface.h"
#include "StoreFrontEnd.h"
#include "CollectionInterface.h"
#include <algorithm>

using namespace std;


CardInterface::CardInterface( const string& aszCardLine, CollectionInterface* apParent )
   : m_pCollection(apParent)
{
   vector<pair<string, string>> vecIDs;
   vector<pair<string, string>> vecMeta;
   unsigned int Count;
   string ParentGroupName;

   StringInterface::ParseCardLine( aszCardLine, Count, ParentGroupName,
                                   vecIDs, vecMeta );

   m_iCount = Count;
   m_szName = ParentGroupName;

   if( vecMeta.size() > 0 )
   {
      for( auto& szMeta : vecMeta )
      {
         if( szMeta.first == StringInterface::GetUIDKey() )
         {
            m_vecRepresentingUIDs.push_back( szMeta.second );
         }
      }
   }
}

CardInterface::CardInterface( const string & aszName, const string & aszUID, CollectionInterface * apParent )
   : m_pCollection(apParent)
{
   m_iCount = 1;
   m_szName = aszName;
   m_vecRepresentingUIDs.push_back( aszUID );
}


CardInterface::~CardInterface()
{
}


int
CardInterface::GetNumber() const
{

   return m_iCount;
}

string
CardInterface::GetHash() const
{
   auto ptSF = StoreFrontEnd::Server();
   return ptSF->GetMetaTagHash( m_szName, GetFirstUID() );
}

string
CardInterface::GetFirstUID() const
{
   if( m_vecRepresentingUIDs.size() > 0 )
   {
      return m_vecRepresentingUIDs[0];
   }
   else
   {
      return "";
   }
}

vector<string>
CardInterface::GetRepresentingUIDs() const
{
   return m_vecRepresentingUIDs;
}

string
CardInterface::GetName() const
{
   return m_szName;
}

string 
CardInterface::GetMultiverseID() const
{
   auto ptSF = StoreFrontEnd::Server();
   return ptSF->GetIdentifyingAttribute( m_szName, GetFirstUID(), "multiverseid" );
}

string
CardInterface::GetManaCost() const
{
   auto ptSF = StoreFrontEnd::Server();
   auto szManaCost = ptSF->GetCommonAttribute( m_szName, "manaCost" );
   szManaCost.erase( remove( szManaCost.begin(), szManaCost.end(), '{' ),
                     szManaCost.end() );
   szManaCost.erase( remove( szManaCost.begin(), szManaCost.end(), '}' ),
                     szManaCost.end() );
   return szManaCost;
}

string 
CardInterface::GetCMC() const
{
   auto ptSF = StoreFrontEnd::Server();
   return ptSF->GetCommonAttribute( m_szName, "cmc" );
}

string
CardInterface::GetCardType() const
{
   auto ptSF = StoreFrontEnd::Server();
   return ptSF->GetCommonAttribute( m_szName, "type" );
}

string
CardInterface::GetSet() const
{
   auto ptSF = StoreFrontEnd::Server();
   return ptSF->GetIdentifyingAttribute( m_szName, GetFirstUID(), "set" );
}

string 
CardInterface::GetMetaTag( const string & aszKey ) const
{
   return GetMetaTag(aszKey, GetFirstUID());
}

string
CardInterface::GetMetaTag( const string& aszKey, const string& aszUID ) const
{
   auto ptSF = StoreFrontEnd::Server();
   return ptSF->GetMetaTag( m_szName, aszUID, aszKey );
}

vector<pair<string, string>>
CardInterface::GetMetaTags() const
{
   auto ptSF = StoreFrontEnd::Server();
   return ptSF->GetMetaTags( m_szName, GetFirstUID() );
}

string
CardInterface::GetAttribute( const string& aszKey ) const
{
   auto ptSF = StoreFrontEnd::Server();
   return ptSF->GetCommonAttribute( m_szName, aszKey );
}

vector<string> 
CardInterface::GetAttributeOptions( const string & aszKey ) const
{
   vector<string> vecRetval;
   auto ptSF = StoreFrontEnd::Server();

   auto mapOpts = ptSF->GetIdentifyingAttributeOptions( m_szName );
   auto iter_attr = mapOpts.find(aszKey);

   if( iter_attr != mapOpts.end() )
   {
      vecRetval = iter_attr->second;
   }

   return vecRetval;
}

vector<string> 
CardInterface::GetPairedAttributes( const string & aszKey ) const
{
   auto ptSF = StoreFrontEnd::Server();
   auto vecTags = ptSF->GetPairedAttributes();

   vector<string> vecRetval;
   for( auto& attr : vecTags )
   {
      if( attr.first == aszKey )
      {
         vecRetval.push_back( attr.second );
      }
      else if( attr.second == aszKey )
      {
         vecRetval.push_back( attr.first );
      }
   }

   return vecRetval;
}

vector<pair<string, string>>
CardInterface::GetAttributes() const
{
   auto ptSF = StoreFrontEnd::Server();
   return ptSF->GetIdentifyingAttributes( m_szName, GetFirstUID() );
}

string 
CardInterface::SetMetaTag( const string & aszKey, const string& aszVal, const string & aszUID ) const
{
   auto ptSF = StoreFrontEnd::Server();
   return ptSF->SetMetaTag( m_szName, aszUID, aszKey, aszVal );
}

std::string
CardInterface::SetMetaTags( const std::string & aszUID, const std::vector<std::pair<std::string, std::string>>& avecMeta ) const
{
   auto ptSF = StoreFrontEnd::Server();
   return ptSF->SetMetaTags( m_szName, aszUID, avecMeta );
}

string
CardInterface::SetAttributes( const string& aszUID,
                              const vector<pair<string, string>>& avecAttrs ) const
{
   auto ptSF = StoreFrontEnd::Server();
   return ptSF->SetAttributes( m_szName, aszUID, avecAttrs );
}

string 
CardInterface::SetAttribute( const string & aszKey, const string& aszVal, const string & aszUID ) const
{
   auto ptSF = StoreFrontEnd::Server();
   return ptSF->SetAttribute( m_szName, aszUID, aszKey, aszVal );
}

CollectionInterface* 
CardInterface::GetCollection()
{
   return m_pCollection;
}
