#pragma once
#include "CollectionSource.h"

using namespace std;

// Collection
//  Maintains a list of Collection Objects.
class Collection
{
public:
   Collection();
   ~Collection();

private:
   // Stores a list of all the types of collection objects in this collection.
   vector<CollectionObjectID> m_vecCollectionIDs;
};

