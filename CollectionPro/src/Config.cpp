#include "stdafx.h"
#include "Config.h"
#include "Support/MD5.h"
#include "rapidxml-1.13\rapidxml_print.hpp"
#include "rapidxml-1.13\rapidxml.hpp"
#include "rapidxml-1.13\rapidxml_utils.hpp"

#include <sstream>
#include <fstream>
#include <sstream>
#include <iostream>
#include <set>
#include <map>

/*
const char * const CollectionObject::LstUniqueTraits[] = { "manaCost", "colors", "name", "power",
"toughness", "loyalty", "text" };

const char * const CollectionObject::LstNonUniqueTraits[] = { "set", "multiverseid" };
*/

using namespace std;

Config* Config::ms_pConfig = nullptr;
Config* Config::ms_pTestConfig = nullptr;
const char* Config::MetaFileExtension = "meta";
const char* Config::HistoryFileExtension = "history";
const char* Config::OverheadFileExtension = "data";
const char* Config::TrackingTagId = "__";
const char* Config::IgnoredTagId = "_";
const char* Config::HashKey = "__hash";
const char* Config::NotFoundString = "NF";
const char* Config::CollectionDefinitionKey = ":";
bool Config::ms_bTestMode = false;

#include <stdio.h>
Config::Config()
{
   m_bIsFileConfig = false;

   ifstream file(".\\Config\\Config.xml");
   if (!file.good())
   {
      initDefaultSettings();
   }
   else
   {
      initConfigSettings(file);
      m_bIsFileConfig = true;
   }
   file.close();
}


Config::~Config()
{
}

string Config::GetSourceFilePath()
{
   return GetSourceDirectory() + "\\" + GetSourceFileName();
}

string 
Config::GetSourceFileName()
{
   return m_szSourceFile;
}

string 
Config::GetSourceDirectory()
{
   return ".\\Config\\" + m_szSourceFolder;
}

string Config::GetImportSourceFilePath()
{
   return GetSourceDirectory() + "\\" + GetImportSourceFileName();
}

string 
Config::GetImportSourceFileName()
{
   return m_szImportSource;
}

string 
Config::GetOverheadFolerName()
{
   return m_szOverheadFolder;
}

string 
Config::GetHistoryFolderName()
{
   return m_szHistoryFolder;
}

string Config::GetMetaFolderName()
{
   return m_szMetaFolder;
}

string Config::GetImagesDirectory()
{
   return ".\\Config\\" + m_szImagesFolder;
}

string Config::GetCollectionsDirectory()
{
   return ".\\" + GetCollectionsFolderName();
}

string Config::GetCollectionsFolderName()
{
   return m_szCollectionsFolder;
}

string 
Config::GetCollectionsMetaDirectory()
{
   return GetCollectionsDirectory() + "\\" + GetMetaFolderName();
}

string 
Config::GetCollectionsHistoryDirectory()
{
   return GetCollectionsDirectory() + "\\" + GetHistoryFolderName();
}

string 
Config::GetCollectionsOverheadDirectory()
{
   return GetCollectionsDirectory() + "\\" + GetOverheadFolerName();
}

// Returns a numerical key code
// -1 if not found.
char
Config::GetKeyCode(string aszFullKey)
{
   auto iter_char = m_mapKeyCodes.find(aszFullKey);
   if( iter_char != m_mapKeyCodes.end() ) {
      return iter_char->second;
   }

   return -1;
}

string 
Config::GetFullKey(char aiKeyCode)
{
   auto iter_KeyMappings = m_mapKeyCodes.begin();
   for (; iter_KeyMappings != m_mapKeyCodes.end(); ++iter_KeyMappings)
   {
      if( iter_KeyMappings->second == aiKeyCode )
      {
         return iter_KeyMappings->first;
      }
   }

   return Config::NotFoundString;
}

string 
Config::GetHash(string& aszHashingString)
{
   MD5* md5Hasher = new MD5(aszHashingString);
   string szResult = md5Hasher->hexdigest();
   delete md5Hasher;
   return szResult;
}

vector<string> 
Config::GetPairedKeys( const string& aszKey )
{
   auto iters_val_range = m_maplstPairedKeys.equal_range(aszKey);
   set<string> setRetval;
   for( auto iter_keys = iters_val_range.first;
        iter_keys != iters_val_range.second;
        ++iter_keys ) 
   {
      setRetval.insert(iter_keys->first);
      setRetval.insert(iter_keys->second);
   }
   return vector<string>(setRetval.begin(), setRetval.end());
}

vector<pair<string, string>>
Config::GetPairedKeysList()
{
   return vector<pair<string,string>>(m_maplstPairedKeys.begin(), m_maplstPairedKeys.end());
}
vector<string> 
Config::GetIdentifyingAttributes()
{
   return m_lstIdentifyingAttributes;
}
vector<string> 
Config::GetStaticAttributes()
{
   return m_lstStaticAttributes;
}

vector<string>
Config::GetPerCollectionMetaTags()
{
   return m_lstPerCollectionMetaTags;
}

const function<string(const Tag&)> 
Config::GetTagHelper(TagHelperType aiMode) const {
   if( aiMode == Key ) {
      return [](Tag atag)->std::string { return atag.first; };;
   }
   else {
      return [](Tag atag)->std::string { return atag.second; };;
   }
}

string Config::GetHexID( unsigned long aulValue )
{
   stringstream stream;
   stream << setfill( '0' ) << setw( 3 ) << hex;
   
   stream << aulValue;

   return string( stream.str().substr(0, 3) );
}

bool Config::IsIdentifyingAttributes(const string& aszAttrs)
{
   return find( m_lstIdentifyingAttributes.begin(), 
                m_lstIdentifyingAttributes.end(),
                aszAttrs ) != m_lstIdentifyingAttributes.end();
}

bool Config::IsPairedKey(const string& aszKey)
{
   for( auto& pair : m_maplstPairedKeys )
   {
      if( pair.first == aszKey || pair.second == aszKey )
      {
         return true;
      }
   }
   return false;
}

bool Config::IsValidKey(const string& aszKey)
{
   // A valid key is either a static attr or identifying attr
   return IsStaticAttribute(aszKey) ||
          IsIdentifyingAttributes(aszKey);
}

bool Config::IsStaticAttribute(const string& aszAttr)
{
   return find( m_lstStaticAttributes.begin(),
                m_lstStaticAttributes.end(),
                aszAttr) != m_lstStaticAttributes.end();
}

bool 
Config::IsLoaded()
{
   return m_bIsFileConfig;
}

Config* Config::Instance()
{
   if (ms_pConfig == nullptr)
   {
      ms_pConfig = new Config();
   }

   if( ms_pTestConfig == nullptr && ms_bTestMode )
   {
      ms_pTestConfig = new Config(TestInstance());
   }

   if( ms_bTestMode )
   {
      return ms_pTestConfig;
   }

   return ms_pConfig;
}

void
Config::SetTestMode( bool abMode )
{
   ms_bTestMode = abMode;
}

Config 
Config::TestInstance()
{
   Config cTest;
   cTest.m_lstIdentifyingAttributes.clear();
   cTest.m_mapKeyCodes.clear();
   cTest.m_maplstPairedKeys.clear();
   cTest.m_lstPerCollectionMetaTags.clear();
   cTest.m_lstStaticAttributes.clear();
   cTest.m_szSourceFolder = "Test";
   cTest.m_szSourceFile = "TestSource.xml";

   cTest.m_lstStaticAttributes.push_back("name");
   cTest.m_lstIdentifyingAttributes.push_back("set");
   cTest.m_lstIdentifyingAttributes.push_back("mud");
   cTest.m_lstIdentifyingAttributes.push_back("solo");

   cTest.m_maplstPairedKeys.insert(make_pair("set","mud"));

   cTest.m_mapKeyCodes.insert(make_pair("name", 1));
   cTest.m_mapKeyCodes.insert(make_pair("set", 2));
   cTest.m_mapKeyCodes.insert(make_pair("mud", 3));
   cTest.m_mapKeyCodes.insert(make_pair("solo", 4));

   return cTest;
}

void Config::initDefaultSettings()
{
   m_lstStaticAttributes.push_back("manaCost");
   m_lstStaticAttributes.push_back("colors");
   m_lstStaticAttributes.push_back("name");
   m_lstStaticAttributes.push_back("names");
   m_lstStaticAttributes.push_back("power");
   m_lstStaticAttributes.push_back("toughness");
   m_lstStaticAttributes.push_back("loyalty");
   m_lstStaticAttributes.push_back("text");
   m_lstStaticAttributes.push_back("cmc");
   m_lstStaticAttributes.push_back("colorIdentity");

   m_lstIdentifyingAttributes.push_back("set");
   m_lstIdentifyingAttributes.push_back("multiverseid");

   m_maplstPairedKeys.insert(make_pair("set", "multiverseid"));

   m_mapKeyCodes.insert(make_pair("set", 1));
   m_mapKeyCodes.insert(make_pair("multiverseid", 2));
   m_mapKeyCodes.insert(make_pair("manaCost", 3));
   m_mapKeyCodes.insert(make_pair("colors", 4));
   m_mapKeyCodes.insert(make_pair("name", 5));
   m_mapKeyCodes.insert(make_pair("toughness", 6));
   m_mapKeyCodes.insert(make_pair("loyalty", 7));
   m_mapKeyCodes.insert(make_pair("text", 8));
   m_mapKeyCodes.insert(make_pair("cmc", 9));
   m_mapKeyCodes.insert(make_pair("colorIdentity", 10));

   m_lstPerCollectionMetaTags.push_back("Generalization");

   m_szImportSource = "AllSets.json";
   m_szSourceFile = "MtGSource.xml";
   m_szSourceFolder = "Source";

   m_szCollectionsFolder = "Collections";
   m_szHistoryFolder = "Data";
   m_szMetaFolder = "Data";
   m_szOverheadFolder = "Data";
}

void Config::initConfigSettings(ifstream& asConfig)
{
   rapidxml::xml_document<> doc;
   stringstream buffer;
   buffer << asConfig.rdbuf();
   asConfig.close();

   cout << "Parsing Doc" << endl;
   string content(buffer.str());
   doc.parse<0>(&content[0]);
   cout << "Parse Done" << endl;

   rapidxml::xml_node<> *xmlNode_Config = doc.first_node();

   rapidxml::xml_node<> *xmlNode_Source = xmlNode_Config->first_node("Source");
   rapidxml::xml_node<> *xmlNode_ImportDefinitions = xmlNode_Source->first_node("ImportDefinitions");
   rapidxml::xml_node<> *xmlNode_ValidKeys = xmlNode_ImportDefinitions->first_node("ValidKeys");
   rapidxml::xml_node<> *xmlNode_ValidKey = xmlNode_ValidKeys->first_node("Key");
   while (xmlNode_ValidKey != 0)
   {
      string szKey = xmlNode_ValidKey->value();
      m_lstStaticAttributes.push_back(szKey);

      xmlNode_ValidKey = xmlNode_ValidKey->next_sibling();
   }

   rapidxml::xml_node<> *xmlNode_IdentifyingKeys = xmlNode_ImportDefinitions->first_node("IdentifyingKeys");
   xmlNode_ValidKey = xmlNode_IdentifyingKeys->first_node("Key");
   while (xmlNode_ValidKey != 0)
   {
      string szKey = xmlNode_ValidKey->value();
      m_lstIdentifyingAttributes.push_back(szKey);

      xmlNode_ValidKey = xmlNode_ValidKey->next_sibling();
   }

   rapidxml::xml_node<> *xmlNode_PairedKeys = xmlNode_ImportDefinitions->first_node("PairedKeys");
   rapidxml::xml_node<> *xmlNode_Pair = xmlNode_PairedKeys->first_node("Pair");
   while (xmlNode_Pair != 0)
   {
      rapidxml::xml_node<> *xmlNode_Key = xmlNode_Pair->first_node();

      string szKey = xmlNode_Key->value();

      xmlNode_Key = xmlNode_Key->next_sibling();

      string szValue = xmlNode_Key->value();

      m_maplstPairedKeys.insert(make_pair(szKey, szValue));

      xmlNode_Pair = xmlNode_Pair->next_sibling();
   }

   rapidxml::xml_node<> *xmlNode_CodeKeys = xmlNode_ImportDefinitions->first_node("CodeKeys");
   xmlNode_Pair = xmlNode_CodeKeys->first_node("Pair");
   while (xmlNode_Pair != 0)
   {
      rapidxml::xml_node<> *xmlNode_Key = xmlNode_Pair->first_node();

      string szKey = xmlNode_Key->value();

      xmlNode_Key = xmlNode_Key->next_sibling();

      string szValue = xmlNode_Key->value();
      if( isdigit(szValue[0]) )
      {
         int iNumVals = stoi(szValue);
         m_mapKeyCodes.insert(make_pair(szKey, iNumVals));
      }
      else
      {
         Debug::Log("Config", "Failed to load key code " + szValue);
      }

      xmlNode_Pair = xmlNode_Pair->next_sibling();
   }

   rapidxml::xml_node<> *xmlNode_Collections = xmlNode_Config->first_node("Collection");
   rapidxml::xml_node<> *xmlNode_CollectionDefinitions = xmlNode_Collections->first_node("CollectionDefinitions");
   rapidxml::xml_node<> *xmlNode_CollectionMetaKeys = xmlNode_CollectionDefinitions->first_node("CollectionMetaKeys");

   rapidxml::xml_node<> *xmlNode_MetaKey = xmlNode_CollectionMetaKeys->first_node("Key");
   while (xmlNode_MetaKey != 0)
   {
      string szKey = xmlNode_MetaKey->value();
      m_lstPerCollectionMetaTags.push_back(szKey);

      xmlNode_MetaKey = xmlNode_MetaKey->next_sibling();
   }

   rapidxml::xml_node<> *xmlNode_Node = xmlNode_Source->first_node("SourceFolder");
   m_szSourceFolder = xmlNode_Node->value();

   xmlNode_Node = xmlNode_Source->first_node("ImportFile");
   m_szImportSource = xmlNode_Node->value();

   xmlNode_Node = xmlNode_Source->first_node("SourceFile");
   m_szSourceFile = xmlNode_Node->value();

   xmlNode_Node = xmlNode_Collections->first_node("CollectionFolder");
   m_szCollectionsFolder = xmlNode_Node->value();

   xmlNode_Node = xmlNode_Collections->first_node("HistoryFolder");
   m_szHistoryFolder = xmlNode_Node->value();

   xmlNode_Node = xmlNode_Collections->first_node("OverheadFolder");
   m_szOverheadFolder = xmlNode_Node->value();

   xmlNode_Node = xmlNode_Collections->first_node("MetaFolder");
   m_szMetaFolder = xmlNode_Node->value();

   xmlNode_Node = xmlNode_Config->first_node("Images");
   xmlNode_Node = xmlNode_Node->first_node("ImagesFolder");
   m_szImagesFolder = xmlNode_Node->value();
}

