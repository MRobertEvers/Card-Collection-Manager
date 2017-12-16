#pragma once
#include<string>
#include<vector>

#include "../rapidxml-1.13\rapidxml_print.hpp"
#include "../rapidxml-1.13\rapidxml.hpp"
#include "../rapidxml-1.13\rapidxml_utils.hpp"

#include "SourceObject.h"
#include "CollectionObject.h"

using namespace std;

typedef unsigned int CollectionObjectID;

// CollectionSource
//  Maintains an unprocessed set of all objects defined in the library file.
class CollectionSource
{
public:
   CollectionSource();
   ~CollectionSource();

   // LoadLib
   //  Loads an xml library of objects defined like so.
   //  <Card_Database>
   //    <cards>
   //      <card>
   //        <Key>Value</Key>
   //      </card>
   //      ...
   //    </cards>
   //  </Card_Database>
   // Parses the objects into Objects that are stored by this class.
   // aszFilePath: The full path the file with the form defined above.
   void LoadLib(string aszFilePath);

   // LoadCard
   //  Return the cache location if aszCardName is found in the database, 
   //  -1 otherwise. Loads a card into the cache if not already loaded.
   int LoadCard(string aszCardName);

   // ClearCache
   //  Deletes all collection objects in the cache.
   void ClearCache();

   // IsLoaded
   //  Returns whether the source is loaded successfully.
   bool IsLoaded();

private:
   bool m_bIsLoaded;
   vector<SourceObject> m_vecCardDataBuffer;
   vector<CollectionObject> m_vecCardCache;

   // Used for caching searches over 5 chars.
   vector<pair<string, vector<SourceObject>>> m_lstSearchCache;

   void loadCard(rapidxml::xml_node<char> * xmlNode_Card);
   
   SourceObject* getNewSourceObject();

   int findInBuffer(string aszName, bool abCaseSensitive);
   int findInCache(string aszName, bool abCaseSensitive);

   void resetBuffer();
   void finalizeBuffer();

   char* m_AllCharBuff;
   unsigned int m_iAllCharBuffSize;

private:
   static const unsigned int ms_iMaxBufferSize = 5000000;
};

