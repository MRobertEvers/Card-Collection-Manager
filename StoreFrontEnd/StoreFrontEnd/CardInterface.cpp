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

std::vector<std::string>
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
CardInterface::GetMetaTag( const string& aszKey, const string& aszUID ) const
{
   auto ptSF = StoreFrontEnd::Server();
   return ptSF->GetMetaTag( m_szName, aszUID, aszKey );
}

string
CardInterface::GetAttribute( const string& aszKey ) const
{
   auto ptSF = StoreFrontEnd::Server();
   return ptSF->GetCommonAttribute( m_szName, aszKey );
}

string 
CardInterface::SetMetaTag( const string & aszKey, const string& aszVal, const string & aszUID ) const
{
   auto ptSF = StoreFrontEnd::Server();
   return ptSF->SetMetaTag( m_szName, aszUID, aszKey, aszVal );
}

string
CardInterface::SetAttributes( const string & aszKey, const string& aszUID,
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
