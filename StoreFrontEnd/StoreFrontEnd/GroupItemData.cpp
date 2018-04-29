#include "GroupItemData.h"
#include "StoreFrontEnd.h"

#include <algorithm>

GroupItemData::GroupItemData()
{

}

GroupItemData::GroupItemData(const wxString& aszCardName, DataStyle aDataStyle)
{
   if( aDataStyle == LONG_NAME )
   {
      parseLongName(aszCardName);
   }
   else
   {
      m_szName = aszCardName;
   }
}


GroupItemData::~GroupItemData()
{
}

int 
GroupItemData::GetNumber() const
{

   return m_iNumber;
}

wxString 
GroupItemData::GetHash() const
{
   auto ptSF = StoreFrontEnd::Server();
   return ptSF->GetMetaTagHash(m_szName.ToStdString(), GetFirstUID().ToStdString());
}

wxString 
GroupItemData::GetFirstUID() const
{
   if( m_vecUIDs.size() > 0 )
   {
      return m_vecUIDs[0];
   }
   else
   {
      return "";
   }
}

wxString 
GroupItemData::GetName( ) const
{
   return m_szName;
}

wxString 
GroupItemData::GetManaCost( ) const
{
   auto ptSF = StoreFrontEnd::Server();
   auto szManaCost = ptSF->GetCommonAttribute( m_szName.ToStdString(), "manaCost" );
   szManaCost.erase(std::remove( szManaCost.begin(), szManaCost.end(), '{'),
                      szManaCost.end());
   szManaCost.erase(std::remove( szManaCost.begin(), szManaCost.end(), '}'),
                      szManaCost.end());
   return szManaCost;
}

wxString 
GroupItemData::GetCardType( ) const
{
   auto ptSF = StoreFrontEnd::Server();
   return ptSF->GetCommonAttribute( m_szName.ToStdString(), "type" );
}

wxString 
GroupItemData::GetSet( ) const
{
   auto ptSF = StoreFrontEnd::Server();
   return ptSF->GetIdentifyingAttribute( m_szName.ToStdString(), GetFirstUID().ToStdString(), "set" );
}

wxString 
GroupItemData::GetMetaTag(const wxString& aszKey, const wxString& aszUID) const
{
   auto ptSF = StoreFrontEnd::Server();
   return ptSF->GetMetaTag( m_szName.ToStdString(), aszUID.ToStdString(), aszKey.ToStdString() );
}

wxString 
GroupItemData::GetAttribute(const wxString& aszKey) const
{
   auto ptSF = StoreFrontEnd::Server();
   return ptSF->GetCommonAttribute( m_szName.ToStdString(), aszKey.ToStdString() );
}

void 
GroupItemData::parseLongName(const wxString& aszName)
{
   vector<pair<string, string>> vecIDs;
   vector<pair<string, string>> vecMeta;
   unsigned int Count;
   string ParentGroupName;

   StringInterface::ParseCardLine( aszName.ToStdString(), Count, ParentGroupName,
                         vecIDs, vecMeta );

   m_iNumber = Count;
   m_szName = ParentGroupName;

   if( vecMeta.size() > 0 )
   {
      for( auto& szMeta : vecMeta )
      {
         if( szMeta.first == StringInterface::GetUIDKey() )
         {
            m_vecUIDs.push_back( szMeta.second );
         }
      }
   }
}