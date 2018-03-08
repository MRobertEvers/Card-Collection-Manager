#pragma once
#include "../rapidxml-1.13\rapidxml_print.hpp"
#include "../rapidxml-1.13\rapidxml.hpp"
#include "../rapidxml-1.13\rapidxml_utils.hpp"

#include "SourceObject.h"
#include "CollectionObject.h"
#include "Query.h"

#include<string>
#include<vector>

using namespace std;

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
   void HotSwapLib( string aszFilePath );

   // LoadCard
   //  Return the cache location if aszCardName is found in the database, 
   //  -1 otherwise. Loads a card into the cache if not already loaded.
   int LoadCard(string aszCardName);

   TryGet<CollectionObject> GetCardPrototype(int aiCacheIndex);
   TryGet<CollectionObject> GetCardPrototype(string aszCardName);

   vector<int> GetCollectionCache( Location aAddrColID,
                                   CollectionObjectType aColItemType );
   vector<shared_ptr<CopyItem>> GetCollection( Location aAddrColID,
                                               CollectionObjectType aColItemType );

   vector<string> GetAllCardsStartingWith(const Query& aszText);

   // ClearCache
   //  Deletes all collection objects in the cache.
   void ClearCache();

   // IsLoaded
   //  Returns whether the source is loaded successfully.
   bool IsLoaded();

   // CollapseCardLine
   //  Utilizes its knowledge of all cards to shrink the identifier
   //  to a more compact unique representation.
   void CollapseCardLine(string& rszCard, bool abIncludeCount = true);
   void ExpandAdditionLine( string& rszLine );

private:
   bool m_bIsLoaded;
   vector<SourceObject> m_vecCardDataBuffer;
   vector<CollectionObject> m_vecCardCache;

   char* m_AllCharBuff;
   unsigned int m_iAllCharBuffSize;

   // Used for caching searches over 5 chars.
   vector<pair<string, vector<SourceObject>>> m_lstSearchCache;

   void loadCard(rapidxml::xml_node<char> * xmlNode_Card);
   int loadCardToCache(unsigned int iDataBuffInd);
   
   SourceObject* getNewSourceObject();

   int findInBuffer(const string& aszName);
   int findInCache(const string& aszName);

   void resetBuffer();
   void finalizeBuffer();

private:
   static const unsigned int ms_iMaxBufferSize = 5000000;
};

