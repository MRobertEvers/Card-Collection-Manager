#include "vcdCDBIListItemData.h"
#include <algorithm>
#include "StoreFront.h"


vcdCDBIListItemData::vcdCDBIListItemData(const wxString& aszCardName, DataStyle aDataStyle)
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


vcdCDBIListItemData::~vcdCDBIListItemData()
{
}

int 
vcdCDBIListItemData::GetNumber() const
{
   return m_iNumber;
}

wxString 
vcdCDBIListItemData::GetHash() const
{
   return m_szHash;
}

wxString 
vcdCDBIListItemData::GetName() const
{
   return m_szName;
}

wxString 
vcdCDBIListItemData::GetManaCost() const
{
   return m_szManaCost;
}

wxString 
vcdCDBIListItemData::GetCardType() const
{
   return m_szCardType;
}

wxString 
vcdCDBIListItemData::GetSet() const
{
   return m_szSet;
}

wxString 
vcdCDBIListItemData::GetMetaTag(const wxString& aszKey) const
{
   StringInterface parser;
   return parser.FindTagInList(m_vecMetaTags, aszKey.ToStdString());
}

wxString 
vcdCDBIListItemData::GetAttribute(const wxString& aszKey) const
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
vcdCDBIListItemData::parseLongName(const wxString& aszName)
{
   StringInterface parser;

   unsigned int Count;
   string Name;

   parser.ParseCardLine( aszName.ToStdString(), Count, Name,
                         m_vecIdentifiers, m_vecMetaTags );

   m_iNumber = Count;
   m_szName = Name;
   m_szSet = parser.FindTagInList(m_vecIdentifiers, "set");
}

void 
vcdCDBIListItemData::getItemData()
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