#pragma once

#include "../src/Collection/CollectionSource.h"
#include "../src/Collection/CollectionFactory.h"

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
   vector<string> GetCollectionMetaData(const string& aszCollection);
   vector<string> GetCollectionList(const string& aszCollection, int aiVisibility);
   vector<string> GetAllCardsStartingWith(const string& aszColID, const Collection::Query& aszSearch);

   // Card Accessors
   //
   void SetAttribute(const string& aszCardName, const string& aszUID, const string& aszKey, const string& aszVal);
   vector<pair<string, string>> GetMetaTags(const string& aszCardName, const string& aszUID);
   vector<pair<string, string>> GetIdentifyingAttributes(const string& aszCardName, const string& aszUID);
   string GetCardString(const string& aszCardname, const string& aszUID);

   // Source Accessors
   //
   vector<string> GetAllCardsStartingWith(const string& aszSearch);
   string GetCardPrototype(const string& aszCardName);
   void ImportCollectionSource();

   // Config accessors
   //
   vector<pair<string, string>> GetPairedAttributes();
   string GetImagesPath();
   string GetSourceFilePath();
   string GetImportSourceFilePath();

   // String Interface
   string CollapseCardLine(const string& aszCard, bool abIncludeCount);
private:
   CollectionFactory * m_ColFactory; // This will be the main interaction.
   CollectionSource* m_ColSource; // This can be a vector in the future
};

