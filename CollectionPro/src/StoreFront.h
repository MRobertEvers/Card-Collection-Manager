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
   string CreateNewCollection(string aszCollectionName, string aszParent);
   string LoadCollection(string aszCollectionFile);
   vector<string> GetLoadedCollections();

   // Collection Accessors
   //
   void SaveCollection(string aszCollectionName);
   void SubmitBulkChanges(string aszCollection, vector<string> alstChanges);
   vector<string> GetCollectionMetaData(string aszCollection);
   vector<string> GetCollectionList(string aszCollection, int aiVisibility);

   // Card Accessors
   //
   void SetAttribute(string aszCardName, string aszUID, string aszKey, string aszVal);
   vector<pair<string, string>> GetMetaTags(string aszCardName, string aszUID);
   vector<pair<string, string>> GetIdentifyingAttributes(string aszCardName, string aszUID);
   string GetCardString(string aszCardname, string aszUID);

   // Source Accessors
   //
   vector<string> GetAllCardsStartingWith(string aszSearch);
   string GetCardPrototype(string aszCardName);
   void ImportCollectionSource();

   // Config accessors
   //
   vector<pair<string, string>> GetPairedAttributes();
   string GetImagesPath();
   string GetSourceFilePath();
   string GetImportSourceFilePath();

private:
   CollectionFactory * m_ColFactory; // This will be the main interaction.
   CollectionSource* m_ColSource; // This can be a vector in the future
};

