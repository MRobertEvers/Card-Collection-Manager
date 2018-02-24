#include "GroupItemData.h"
#include <algorithm>
#include "StoreFrontEnd.h"

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

   getItemData();
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
   return m_szHash;
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
GroupItemData::GetName() const
{
   return m_szName;
}

wxString 
GroupItemData::GetManaCost() const
{
   return m_szManaCost;
}

wxString 
GroupItemData::GetCardType() const
{
   return m_szCardType;
}

wxString 
GroupItemData::GetSet() const
{
   return m_szSet;
}

wxString 
GroupItemData::GetMetaTag(const wxString& aszKey) const
{
   StringInterface parser;
   return parser.FindTagInList(m_vecMetaTags, aszKey.ToStdString());
}

wxString 
GroupItemData::GetAttribute(const wxString& aszKey) const
{
   StringInterface parser;
   string szValue = parser.FindTagInList(m_vecIdentifiers, aszKey.ToStdString());
   if( szValue == "" )
   {
      auto ptSF = StoreFrontEnd::Instance();
      szValue = ptSF->GetCommonAttribute(m_szName.ToStdString(), aszKey.ToStdString());
   }
   return szValue;
}

void 
GroupItemData::parseLongName(const wxString& aszName)
{
   StringInterface parser;

   unsigned int Count;
   string Name;

   parser.ParseCardLine( aszName.ToStdString(), Count, Name,
                         m_vecIdentifiers, m_vecMetaTags );

   m_iNumber = Count;
   m_szName = Name;
   m_szSet = parser.FindTagInList(m_vecIdentifiers, "set");

   if( m_vecMetaTags.size() > 0 )
   {
      StringInterface szIface;
      for( auto& szMeta : m_vecMetaTags )
      {
         if( szMeta.first == szIface.GetUIDKey() )
         {
            m_vecUIDs.push_back( szMeta.second );
         }
      }

      getItemHash( parser.FindTagInList( m_vecMetaTags, szIface.GetUIDKey() ) );
   }
}

void 
GroupItemData::getItemData()
{
   // m_szName should be populated by now.
   auto ptSF = StoreFrontEnd::Instance();

   // Get the Set options, mana cost, and card type.

   auto mapOptions = ptSF->GetIdentifyingAttributeOptions(m_szName.ToStdString());
   // TODO: This should not be a string literal.
   auto iter_set = mapOptions.find("set");
   if( iter_set != mapOptions.end() )
   {
      for( auto& szOption : iter_set->second )
      {
         m_vecSetOptions.push_back(szOption);
      }

      StringInterface parser;
      if( m_szSet == "" )
      {
         string szVal;
         parser.ListToDelimStr( iter_set->second.begin(), iter_set->second.end(),
                                szVal, "", "," );
         m_szSet = szVal;
      }
   }



   m_szCardType = ptSF->GetCommonAttribute(m_szName.ToStdString(), "type");
   m_szManaCost = ptSF->GetCommonAttribute(m_szName.ToStdString(), "manaCost");
   m_szManaCost.erase(std::remove(m_szManaCost.begin(), m_szManaCost.end(), '{'),
                      m_szManaCost.end());
   m_szManaCost.erase(std::remove(m_szManaCost.begin(), m_szManaCost.end(), '}'),
                      m_szManaCost.end());

}

void 
GroupItemData::getItemHash(const wxString& aszUID)
{
   auto ptSF = StoreFrontEnd::Instance();
   m_szHash = ptSF->GetMetaTagHash( m_szName.ToStdString(), aszUID.ToStdString() );
}