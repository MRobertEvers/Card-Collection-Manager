#include "CardInterface.h"
#include "StoreFrontEnd.h"
#include <algorithm>

using namespace std;


CardInterface::CardInterface( const string& aszCardLine )
{
   vector<pair<string, string>> vecIDs;
   vector<pair<string, string>> vecMeta;
   unsigned int Count;
   string Name;

   StringInterface::ParseCardLine( aszCardLine, Count, Name,
                                   vecIDs, vecMeta );

   m_iCount = Count;
   m_szName = Name;

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

string
CardInterface::GetName() const
{
   return m_szName;
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