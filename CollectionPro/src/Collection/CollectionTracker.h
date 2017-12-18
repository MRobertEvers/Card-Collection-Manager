#pragma once
#include <map>
#include <vector>
#include <memory>

#include "CollectionDeltaClass.h"

class Collection;
class CopyItem;

typedef std::pair<std::shared_ptr<CopyItem>, CopyItem> SnapShot;
typedef std::vector<SnapShot> CopyList;
typedef std::vector<std::shared_ptr<CopyItem>> RealCopyList;

class CollectionTracker
{
public:
   CollectionTracker(Collection* aptrTCollection);
   ~CollectionTracker();

   // Track
   // Call Track, then call "CalculateChanges". This will return
   // the differences between the last two tracks.
   void Track();

   CollectionDeltaClass CalculateChanges();

private:
   Collection* m_ptrTrackedCollection;

   std::map<unsigned int, CopyList> m_mapOld;
   std::map<unsigned int, CopyList> m_mapNew;
};

