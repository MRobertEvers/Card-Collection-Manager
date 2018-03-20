#pragma once

#include "../src/Collection/CollectionSource.h"
#include "../src/Collection/CollectionFactory.h"
#include "../src/Collection/Query.h"

using namespace std;

class __declspec(dllexport) StoreFront
{
public:
   StoreFront();
   ~StoreFront();

   // Self Test
   //
   bool SelfTest();

   // Program Management
   //
   bool ConfigIsLoaded();

   // Collection Manager Accessors
   //
   string CreateNewCollection(const string& aszCollectionName, const string& aszParent);
   string LoadCollection(const string& aszCollectionFile);
   vector<string> GetLoadedCollections();

   // Collection Accessors
   //
   void SaveCollection(const string& aszCollectionName);
   void SubmitBulkChanges(const string& aszCollection, vector<string> alstChanges);
   string GetCollectionName(const string& aszCollectionID);
   string GetCollectionID(const string& aszCollectionName);
   vector<string> GetAllCardsStartingWith(const string& aszColID, const Query& aszSearch);
   map<unsigned long, vector<string>> GetHistoryLines( const string& aszColID, unsigned int aiStart );
   map<unsigned long, vector<string>> GetHistoryLines( const string& aszColID, unsigned int aiStart, unsigned int aiEnd );
   vector<pair<string, string>> GetPeekValues( const string& aszColID );

   // Card Accessors
   //
   void SetAttribute(const string& aszCardName, const string& aszUID, const string& aszKey, const string& aszVal);
   vector<pair<string, string>> GetMetaTags(const string& aszCardName, const string& aszUID);
   string GetMetaTag( const string& aszCardName, const string& aszUID, const string& aszTagKey );
   string GetCommonAttribute(const string& aszCardName, const string& aszAttribute);
   string GetIdentifyingAttribute( const string& aszCardName, const string& aszUID, const string& aszTrait );
   vector<pair<string, string>> GetIdentifyingAttributes(const string& aszCardName, const string& aszUID);
   map<string, vector<string>> GetIdentifyingAttributeOptions(const string& aszCardName);
   string GetDefaultIdentifyingAttributeValue( const string& aszCardname, const string aszKey );
   string GetCardString(const string& aszCardname, const string& aszUID);
   string GetMetaTagHash( const string& aszCardName, const string& aszUID );

   // Source Accessors
   //
   vector<string> GetAllCardsStartingWith(const Query& aszSearch);
   string GetCardPrototype(const string& aszCardName);
   void ImportCollectionSource();

   // Config accessors
   //
   vector<pair<string, string>> GetPairedAttributes();
   string GetImagesDirectory();
   string GetCollectionsDirectory();
   string GetCollectionsMetaDirectory();
   string GetCollectionsHistoryDirectory();
   string GetCollectionsOverheadDirectory();
   string GetImportSourceFileName();
   string GetSourcePath();
   string GetSourceFileName();
   string GetSourceFilePath();
   string GetImportSourceFilePath();
   string GetConfigDirectory();
   string GetImageFilePath( const string& aszCardName, const string& aszSet );

   // String Interface
   string CollapseCardLine(const string& aszCard, bool abIncludeCount);

private:
   CollectionFactory * m_ColFactory; // This will be the main interaction.
   CollectionSource* m_ColSource; // This can be a vector in the future
};

