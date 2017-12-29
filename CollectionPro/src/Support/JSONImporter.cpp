#include "JSONImporter.h"

#include<map>
#include<unordered_set>

#include "Config.h"
#include "StringHelper.h"
#include "../StringInterface.h"

using namespace std;

JSONImporter::JSONImporter()
{
}


JSONImporter::~JSONImporter()
{
}


void 
JSONImporter::ImportJSON( const std::string& aszFileName )
{
   // Verify the file is good before really beginning the function.
   std::ifstream file(aszFileName);
   if (!file.good())
   {
      return;
   }
   
   importJSON(file);
}


void 
JSONImporter::importJSON( std::ifstream& aifstream ) 
{
   nlohmann::json j;
   aifstream >> j;

   importJSON(j);
}

void 
JSONImporter::importJSON( const nlohmann::json& ajson )
{
   // Prepare a data structure to help us load the json file
   auto mapCardDetails = new map<string, CardDetails*, JSONImporter::sortFunc>();

   // Populate the map of all cards and their data.
   // The json file should be a JSON object with each of its keys a set.
   // ...and each set a JSON object.
   auto iter_set  = ajson.begin();
   for(; iter_set != ajson.end(); ++iter_set )
   {
      nlohmann::json json_set = iter_set.value();
      
      // Process the set.
      processSet(json_set, mapCardDetails);
   }

   buildXMLDoc(*mapCardDetails);

   
   for( auto& card : *mapCardDetails )
   {
      delete card.second;
   }

   mapCardDetails->clear();
}

void 
JSONImporter::appendXMLDecl( rapidxml::xml_document<>* axdoc )
{
   rapidxml::xml_node<>* decl = axdoc->allocate_node(rapidxml::node_declaration);
   decl->append_attribute(axdoc->allocate_attribute("version", "1.0"));
   decl->append_attribute(axdoc->allocate_attribute("encoding", "utf-8"));
   axdoc->append_node(decl);
}

void 
JSONImporter::buildXMLDoc( const map<string, CardDetails*, sortFunc>& mapDetails )
{
   Config* config = Config::Instance();
   rapidxml::xml_document<>* xmlCardDoc = new rapidxml::xml_document<>;

   // Include the XML declaration header.
   appendXMLDecl(xmlCardDoc);

   // Add the 'Card_Database' outermose element.
   rapidxml::xml_node<>* xmlNode_root = xmlCardDoc->allocate_node(rapidxml::node_element, "Card_Database");
   xmlCardDoc->append_node(xmlNode_root);

   // Create the 'Cards' element.
   rapidxml::xml_node<>* xmlNode_Cards = xmlCardDoc->allocate_node(rapidxml::node_element, "cards");
   xmlNode_root->append_node(xmlNode_Cards);

   for( auto& pairKey : mapDetails )
   {
      CardDetails* details = pairKey.second;
      rapidxml::xml_node<>* xmlNode_Card = xmlCardDoc->allocate_node(rapidxml::node_element, "card");
      xmlNode_Cards->append_node(xmlNode_Card);

      unsigned int iTraits = details->GetAttrCount();
      for( int i = 0; i < iTraits; i++ )
      {
         details->Keys[i] = StringHelper::Str_Clean(details->Keys[i]);
         details->Vals[i] = StringHelper::Str_Clean(details->Vals[i]);
         
         auto xmlNode_CardAttribute = 
               xmlCardDoc->allocate_node( rapidxml::node_element,
                                          details->Keys[i].c_str() ,
                                          details->Vals[i].c_str() );
         xmlNode_Card->append_node(xmlNode_CardAttribute);
      }
   }

   // Save to file
   std::ofstream file_stored(config->GetSourceFile());
   file_stored << *xmlCardDoc;
   file_stored.close();
   xmlCardDoc->clear();
   delete xmlCardDoc;
}

bool 
JSONImporter::processSet( nlohmann::json& ajsonSet,
                             std::map<std::string, CardDetails*, sortFunc>* rmapResult )
{
   bool bRetval = true;

   // 'code' contains the set code name
   auto jsonValIter = ajsonSet.find("code");
   if( jsonValIter == ajsonSet.end() )
   {
      bRetval &= false;
      return bRetval;
   }
   string szSetName = jsonValIter->get<string>();

   // 'cards' contains a json list of cards.
   jsonValIter = ajsonSet.find("cards");
   if( jsonValIter == ajsonSet.end() )
   {
      bRetval &= false;
      return bRetval;
   }

   auto jsonListCards = jsonValIter.value();

   // jsonCardsList is a json-list of json objects... same as JSON, but no 'key'
   auto iter_cards = jsonListCards.begin();
   for(; iter_cards != jsonListCards.end(); ++iter_cards )
   {
      nlohmann::json json_card = iter_cards.value();
      
      // We have to get the name of card ahead of time to check if
      // we have already processed it.
      jsonValIter = json_card.find("name");
      if( jsonValIter == json_card.end() )
      {
         bRetval &= false;
         continue;
      }

      string szCardName = jsonValIter->get<string>();

      auto ptExisting = rmapResult->find( szCardName );
      if( ptExisting == rmapResult->end() )
      {
         CardDetails* Card;
         if( processCard( json_card, szSetName, Card ) )
         {
            rmapResult->insert(make_pair(szCardName, Card));
         }
      }
      else
      {
         // Update any values that are "identifying"
         updateCard(json_card, szSetName, ptExisting->second);
      }

   }

   return bRetval;
}  

bool 
JSONImporter::processCard( nlohmann::json& ajsonCard,
                              const string& aszSet,
                              CardDetails*& rptResult )
{
   Config* config = Config::Instance();
   CardDetails* ptRetval = new CardDetails();

   // Start with the set attr.
   ptRetval->AddAttr("set", aszSet);

   // Iterate over all keys in the card json object.
   auto iter_cardAttr = ajsonCard.begin();
   for(; iter_cardAttr != ajsonCard.end(); ++iter_cardAttr )
   {
      string szKey = iter_cardAttr.key();
      string szValue = "";
      
      if( config->IsValidKey( szKey ) )
      {
         // If the value is an array, transform it to a delimited str.
         if( iter_cardAttr->is_array() )
         {
            auto attr_vals = iter_cardAttr.value();
            auto iter_array = attr_vals.begin();
            for (; iter_array != attr_vals.end(); ++iter_array )
            {
               szValue += iter_array->get<string>();
               szValue += "::";
            }
            szValue = szValue.substr(0, szValue.size()-2);
         }
         else if( !iter_cardAttr->is_null() )
         {
            if( iter_cardAttr->is_number() )
            {
               szValue = to_string(iter_cardAttr->get<int>());
            }
            else
            {
               szValue = iter_cardAttr->get<string>();
            }
         }

         ptRetval->AddAttr(szKey, szValue);
      }
   }
   
   rptResult = ptRetval;
   return true;
}

bool 
JSONImporter::updateCard( nlohmann::json& ajsonCard,
                             const string& aszSet,
                             CardDetails* rptDetails )
{
   bool bRetval = true;
   Config* config = Config::Instance();
   unordered_set<string> setPairedKeys;

   // Get the list of attrs that need to be updated if present.
   auto vecIdAttrs = config->GetIdentifyingAttributes();

   // Copy card details to a vector so they can be searched.
   vector<string> vecCardKeys( rptDetails->Keys, rptDetails->Keys + 35 );

   // First update the set attr
   updateAttr(vecCardKeys, "set", aszSet, rptDetails);
   if( config->IsPairedKey( "set" ) )
   {
      setPairedKeys.insert("set");
   }

   // Now do the remaining attrs.
   for( auto& szAttr : vecIdAttrs )
   {
      // Find the additional value if there is one.
      auto jsonValIter = ajsonCard.find( szAttr ); 
      if( jsonValIter == ajsonCard.end() )
      {
         bRetval &= false;
         continue;
      }
      auto jsonVal = jsonValIter.value();
      
      // TODO lists not supported as ID trait
      if( !jsonVal.is_null() )
      {
         string szNewValue;
         if( jsonVal.is_number() )
         {
            szNewValue = to_string(jsonVal.get<int>());
         }
         else
         {
            szNewValue = jsonVal.get<string>();
         }

         updateAttr(vecCardKeys, szAttr, szNewValue, rptDetails);
         if( config->IsPairedKey( szAttr ) )
         {
            setPairedKeys.insert(szAttr);
         }
      }
   }

   // Populate paired keys if they are also idattrs, and
   // a value had not been found.
   unordered_set<string> setHandledKeys;
   for( auto& szAttr : vecIdAttrs )
   {
      if( setPairedKeys.find( szAttr ) != setPairedKeys.end() )
      {
         // Get a list of all of its pairs.
         auto vecPairedToThisAttr = config->GetPairedKeys(szAttr);
         for( auto& szPairAttr : vecPairedToThisAttr )
         {
            // We only need to get this value if this key hasn't already
            // been updated.
            if( setPairedKeys.find( szPairAttr ) == setPairedKeys.end() )
            {
               updateAttr(vecCardKeys, szPairAttr, " ", rptDetails);
            }
         }
      }
   }

   return bRetval;
}

// Returns true if the key was updated, false if the key was added
bool 
JSONImporter::updateAttr( const vector<string>& avecKeys,
                             const string& aszKey,
                             const string& aszVal,
                             CardDetails* rptDetails )
{
   auto iter_find = find(avecKeys.begin(), avecKeys.end(), aszKey);
   if( iter_find != avecKeys.end() )
   {
      int iSetKeyInd = distance(avecKeys.begin(), iter_find);
      ensureUniqueAttr(rptDetails->Vals[iSetKeyInd], aszVal);
      return true;
   }
   else
   {
      rptDetails->AddAttr(aszKey, aszVal);
      return false;
   }
}

// Modifies the delimd string to contain the new val.
// This makes '-' and illigal character for an Identifying trait.
bool 
JSONImporter::ensureUniqueAttr( std::string& rszDelimd,
                                   const std::string& aszNewVal )
{
   auto vecExistingVals = StringHelper::Str_Split(rszDelimd, "::");
   
   // If any value contains <string>-<number> then we must parse out string
   unsigned int iHighestCopy = 0;
   for( auto& szExistVal : vecExistingVals )
   {
      if( szExistVal.find(aszNewVal) == string::npos )
      {
         continue;
      }
      iHighestCopy++;

      auto vecPossibleSplit = StringHelper::Str_Split(szExistVal, "-");
      if( ( vecPossibleSplit.size() > 1 ) &&
          ( isdigit( vecPossibleSplit[1][0] ) ) )
      {
         szExistVal = vecPossibleSplit[0];
      }
   }

   if( iHighestCopy > 0 )
   {
      int iIter = 1;
      for( auto& szCopy : vecExistingVals )
      {
         if( szCopy.find(aszNewVal) != string::npos )
         {
            szCopy = szCopy + "-" + to_string(iIter);
            iIter++;
         }
      }
      
      vecExistingVals.push_back(aszNewVal + "-" + to_string(iIter));
   }
   else
   {
      vecExistingVals.push_back(aszNewVal);
   }

   StringInterface stringHelper;
   stringHelper.ListToDelimStr( vecExistingVals.cbegin(),
                                vecExistingVals.cend(),
                                rszDelimd, "", "::" );
   return true;
}