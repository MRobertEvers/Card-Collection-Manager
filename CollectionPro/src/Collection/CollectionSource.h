#pragma once
#include<string>
#include<vector>
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

private:
   bool m_bIsLoaded;
   vector<SourceObject> m_vecCardDataBuffer;
   vector<CollectionObject> m_vecCardCache;
};

