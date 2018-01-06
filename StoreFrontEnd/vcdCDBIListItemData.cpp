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
vcdCDBIListItemData::GetNumber()
{
   return m_iNumber;
}

wxString 
vcdCDBIListItemData::GetName()
{
   return m_szName;
}

wxString 
vcdCDBIListItemData::GetManaCost()
{
   return m_szManaCost;
}

wxString 
vcdCDBIListItemData::GetCardType()
{
   return m_szCardType;
}

void 
vcdCDBIListItemData::parseLongName(const wxString& aszName)
{
   StringInterface parser;

   unsigned int Count;
   string Name;
   string DetailString;
   string MetaString;
   vector<pair<string, string>> Identifiers;
   vector<pair<string, string>> MetaTags;

   parser.ParseCardLine( aszName.ToStdString(), Count, Name,
                         Identifiers, MetaTags );

   m_iNumber = Count;
   m_szName = Name;
   m_szSet = parser.FindTagInList(Identifiers, "set");
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
   }

   m_szCardType = ptSF->GetCommonAttribute(m_szName.ToStdString(), "type");
   m_szManaCost = ptSF->GetCommonAttribute(m_szName.ToStdString(), "manaCost");
   m_szManaCost.erase(std::remove(m_szManaCost.begin(), m_szManaCost.end(), '{'),
                      m_szManaCost.end());
   m_szManaCost.erase(std::remove(m_szManaCost.begin(), m_szManaCost.end(), '}'),
                      m_szManaCost.end());
}