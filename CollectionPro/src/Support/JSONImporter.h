#pragma once
#include <vector>
#include <string>
#include <algorithm>

#include "../rapidxml-1.13/rapidxml_print.hpp"
#include "../rapidxml-1.13/rapidxml.hpp"
#include "../rapidxml-1.13/rapidxml_utils.hpp"
#include "../json/json.hpp"

#include "../Collection/CollectionSource.h"
#include "../Support/StringHelper.h"

// Converts the MTGJSON to XML.
class JSONImporter
{
private:
   // CardAttributes is used to convert allsets json to xml.
   class CardDetails
   {
   public:
      CardDetails(){};
      ~CardDetails(){};

      void AddAttr( const std::string& aszKey,
                    const std::string& aszValue )
      {
         Keys[m_iAttrCount] = aszKey;
         Vals[m_iAttrCount] = aszValue;
         m_iAttrCount++;
      }

      unsigned int GetAttrCount()
      {
         return m_iAttrCount;
      }
      
      std::string Keys[35];
      std::string Vals[35];
      rapidxml::xml_node<>* XMLNode;
      rapidxml::xml_node<>* XMLChildNodes[35];
   private:
      unsigned int m_iAttrCount = 0;
   };

   struct sortFunc
   {
   public:
      bool operator()( std::basic_string<char, std::char_traits<char>, std::allocator<char>> szOne,
                       std::basic_string<char, std::char_traits<char>, std::allocator<char>> szTwo ) const
      {
         std::string szOneLow = szOne;
         szOneLow = StringHelper::convertToSearchString(szOneLow);
         transform(szOneLow.begin(), szOneLow.end(), szOneLow.begin(), ::tolower);

         std::string szTwoLow = szTwo;
         szTwoLow = StringHelper::convertToSearchString(szTwoLow);
         transform(szTwoLow.begin(), szTwoLow.end(), szTwoLow.begin(), ::tolower);

         return szOneLow < szTwoLow;
      }
   };

public:
   JSONImporter();
   ~JSONImporter();

   void ImportJSON( const std::string& aszFileName );

private:
   void importJSON( std::ifstream& aifstream );
   void importJSON( const nlohmann::json& ajson );

   void appendXMLDecl( rapidxml::xml_document<>* axdoc );

   void buildXMLDoc( const std::map<std::string, CardDetails*, sortFunc>& mapDetails );

   bool processSet( nlohmann::json& ajsonSet,
                    std::map<std::string, CardDetails*, sortFunc>* rmapResult );

   bool processCard( nlohmann::json& ajsonCard,
                     const std::string& aszSet,
                     CardDetails*& rptResult );

   bool updateCard(  nlohmann::json& ajsonCard,
                     const std::string& aszSet,
                     CardDetails* rptResult );

   bool updateAttr( const std::vector<std::string>& aszKeys,
                    const std::string& aszKey,
                    const std::string& aszVal, 
                    CardDetails* rptResult );

   bool ensureUniqueAttr( std::string& aszDelimd, 
                          const std::string& aszNewVal );

};


