#pragma once
#include<vector>

typedef std::pair<std::string, std::string> Tag;

enum TagHelperType : int
{
   Key = 0x0,
   Value = 0x1
};

class Config
{
public:
   Config();
   ~Config();

   std::string GetImportSourceFile();
   std::string GetSourceFile();
   std::string GetSourceFolder();
   std::string GetOverheadFolerName();
   std::string GetHistoryFolderName();
   std::string GetMetaFolderName();
   std::string GetImagesFolder();
   
   std::string GetCollectionsDirectory();
   std::string GetCollectionsFolderName();

   char GetKeyCode(std::string aszFullKey);
   std::string GetFullKey(char aiKeyCode);

   std::string GetHash(std::string& aszHashingString);

   std::vector<std::string> GetPairedKeys(const std::string& aszKey);
   std::vector<Tag> GetPairedKeysList();
   std::vector<std::string> GetIdentifyingAttributes();
   std::vector<std::string> GetStaticAttributes();
   std::vector<std::string> GetPerCollectionMetaTags();

   std::string GetHexID( unsigned long aulValue );

   bool IsPairedKey(const std::string& aszKey);
   bool IsValidKey(const std::string& aszKey);
   bool IsStaticAttribute(const std::string& aszAttrs);
   bool IsIdentifyingAttributes(const std::string& aszAttrs);

   bool IsLoaded();

private:
   std::multimap<std::string, std::string> m_maplstPairedKeys;
   std::map<std::string, char> m_mapKeyCodes;
   std::vector<std::string> m_lstIdentifyingAttributes;
   std::vector<std::string> m_lstStaticAttributes;
   std::vector<std::string> m_lstPerCollectionMetaTags;

   std::string m_szImportSource;
   std::string m_szSourceFile;
   std::string m_szSourceFolder;

   std::string m_szCollectionsFolder;
   std::string m_szHistoryFolder;
   std::string m_szMetaFolder;
   std::string m_szImagesFolder;
   std::string m_szOverheadFolder;

   void initDefaultSettings();
   void initConfigSettings(std::ifstream& asConfig);

   bool m_bIsFileConfig;

public:
   static Config* Instance();
   static void SetTestMode(bool abMode);
   static Config TestInstance();

   static const char* TrackingTagId;
   static const char* IgnoredTagId;
   static const char* MetaFileExtension;
   static const char* HistoryFileExtension;
   static const char* OverheadFileExtension;
   static const char* HashKey;
   static const char* NotFoundString;
   static const char* CollectionDefinitionKey;

private:
   static bool ms_bTestMode;
   static Config* ms_pConfig;
   static Config* ms_pTestConfig;
};