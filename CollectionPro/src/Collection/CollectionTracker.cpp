#include "CollectionTracker.h"

#include <functional>

#include "CollectionObject.h"
#include "CopyItem.h"
#include "Collection.h"
#include "../Support/TryGet.h"

CollectionTracker::CollectionTracker(Collection* aptrTCollection)
{
   m_ptrTrackedCollection = aptrTCollection;
}

CollectionTracker::~CollectionTracker()
{
}

void 
CollectionTracker::Track()
{
   Location colAddress;

   m_mapOld = m_mapNew;
   m_mapNew.clear();

   colAddress = m_ptrTrackedCollection->GetIdentifier();
   for( auto& itemPair : m_ptrTrackedCollection->GetCollectionItems() )
   {
      auto item = itemPair.first;
      auto lstCList = itemPair.second;
      for ( auto iter_Copy  = lstCList.begin(); 
            iter_Copy != lstCList.end();
            iter_Copy++ )
      {
         // Map a copy of the current state so that it may be used to track.
         CopyItem copyOfCopy = CopyItem(**iter_Copy);
         
         // This points to the actual copy. When we come back to track changes,
         // if the item at the pointer is different from the copy of it, then it
         // has changed.
         auto copyOfPointerToCopy = *iter_Copy;

         SnapShot copySnapshot = std::make_pair(copyOfPointerToCopy, copyOfCopy);
         m_mapNew[item].push_back(copySnapshot);
      }
   }

}

CollectionDeltaClass 
CollectionTracker::CalculateChanges()
{
   std::map<CollectionObject*, CopyList> mapOld(m_mapOld);
   std::map<CollectionObject*, CopyList> mapNew(m_mapNew);
   std::map<CollectionObject*, CopyList>::iterator iter_FoundItem;

   std::map<CollectionObject*, CopyList> mapAdded;
   std::map<CollectionObject*, CopyList> mapChanged;
   std::map<CollectionObject*, CopyList> mapChangedPrevState;
   std::map<CollectionObject*, CopyList> mapRemoved;

   Location colAddress;
   CollectionSource* colSource;

   CopyList::iterator iter_CopyCurrent;
   CopyList::iterator iter_CopyOld;

   std::map<CollectionObject*, CopyList>::iterator iter_Current;
   for( iter_Current  = mapNew.begin(); 
        iter_Current != mapNew.end();
        iter_Current++ )
   {
      CollectionObject* iCurrentItemType = iter_Current->first;

      // lstSnapshots is a list of all the copys for item
      // 'iCurrentItemType' as they were at the time of
      // last track.
      CopyList* lstNewSnapshots = &iter_Current->second;

      iter_FoundItem = mapOld.find(iCurrentItemType);
      if (iter_FoundItem != mapOld.end())
      {
         CopyList* lstOldSnapshots = &iter_FoundItem->second;
         // Excellent, this type of thing exists both before
         // and after.
         for (iter_CopyCurrent  = lstNewSnapshots->begin();
              iter_CopyCurrent != lstNewSnapshots->end();
              iter_CopyCurrent++)
         {
            // Try to find if this snapshot's memory location corresponds
            // to a snapshot's memory location in the old list.
            // If so, then compare the snapshots to see what changed.
            auto cCopyCurrentState = iter_CopyCurrent->first;
            int iFound = -1;
            for( size_t i = 0; i < lstOldSnapshots->size(); i++ )
            {
               if( cCopyCurrentState == lstOldSnapshots->at( i ).first )
               {
                  iFound = i;
               }
            }

            if (iFound != -1 )
            {
               CopyItem cSnapshotCopy = lstOldSnapshots->at(iFound).second;
               if (cCopyCurrentState->GetHash() != cSnapshotCopy.GetHash())
               {
                  // If this item has been changed since last track.
                  // Otherwise, do nothing.
                  mapChanged[iCurrentItemType].
                     push_back(*iter_CopyCurrent);

                  mapChangedPrevState[iCurrentItemType].
                     push_back(lstOldSnapshots->at(iFound));
               }

               // Erase the item from mapOld.
               iter_FoundItem->
                  second.erase(iter_FoundItem->second.begin() + iFound);
            }
            else
            {
               // This item has been added.
               mapAdded[iCurrentItemType].push_back(*iter_CopyCurrent);
            }
         }
      }
      else
      {
         // All these items have been added since last track.
         for (iter_CopyCurrent  = lstNewSnapshots->begin();
              iter_CopyCurrent != lstNewSnapshots->end();
              iter_CopyCurrent++)
         {
            mapAdded[iCurrentItemType].push_back(*iter_CopyCurrent);
         }
      }
   }

   // Anything left in mapOld has been removed since last track.
   std::map<CollectionObject*, CopyList>::iterator iter_Old;
   for (iter_Old  = mapOld.begin();
        iter_Old != mapOld.end();
        iter_Old++)
   {
      for (iter_CopyOld  = iter_Old->second.begin();
           iter_CopyOld != iter_Old->second.end();
           iter_CopyOld++)
      {
         mapRemoved[iter_Old->first].push_back(*iter_CopyOld);
      }
   }

   // All items have been accounted for.
   // We have added, removed, and before+afters
   // Create the strings for each
   std::vector<std::string> lstAdds;
   std::vector<std::string> lstChanges;
   std::vector<std::string> lstRemoves;
   std::string szNote = "";

   colAddress = m_ptrTrackedCollection->GetIdentifier();
   colSource = m_ptrTrackedCollection->m_ptrCollectionSource;

   CopyList::iterator iter_CurrentSnapshot;
   std::map<CollectionObject*, CopyList>::iterator iter_Items;
   for ( iter_Items  = mapAdded.begin();
         iter_Items != mapAdded.end();
         iter_Items++ )
   {
      auto item = iter_Items->first;
      for (iter_CurrentSnapshot  = iter_Items->second.begin();
           iter_CurrentSnapshot != iter_Items->second.end();
           iter_CurrentSnapshot++)
      {
         szNote = "+ " + CollectionObject::ToCardLine(
            iter_CurrentSnapshot->first->GetAddress(),
            item->GetName(),
            iter_CurrentSnapshot->first->GetIdentifyingAttributes(),
            iter_CurrentSnapshot->first->GetMetaTags(MetaTagType::Visible),
            colAddress);
         lstAdds.push_back(szNote);
      }
   }

   for (iter_Items  = mapRemoved.begin();
        iter_Items != mapRemoved.end();
        iter_Items++)
   {
      auto item = iter_Items->first;
      for (iter_CurrentSnapshot  = iter_Items->second.begin();
           iter_CurrentSnapshot != iter_Items->second.end();
           iter_CurrentSnapshot++)
      {
		 CopyItem* oldSnapshot = &iter_CurrentSnapshot->second;
         szNote = "- " + CollectionObject::ToCardLine(
            oldSnapshot->GetAddress(),
            item->GetName(),
            oldSnapshot->GetIdentifyingAttributes(),
            oldSnapshot->GetMetaTags(MetaTagType::Visible),
            colAddress);
         lstRemoves.push_back(szNote);
      }
   }

   for (iter_Items = mapChanged.begin();
        iter_Items != mapChanged.end();
        iter_Items++)
   {
      CopyList* lstSnapshots = &iter_Items->second;
      CopyList* lstOldSnapshots = &mapChangedPrevState.at( iter_Items->first );

      auto item = iter_Items->first;
      for( int i = 0; i < lstSnapshots->size(); i++ )
      {
         CopyItem* currentSnapshot = &lstSnapshots->at(i).second;
         CopyItem* oldSnapshot = &lstOldSnapshots->at(i).second;
         szNote = "% " + CollectionObject::ToCardLine(
            oldSnapshot->GetAddress(),
            item->GetName(),
            oldSnapshot->GetIdentifyingAttributes(),
            oldSnapshot->GetMetaTags(MetaTagType::Visible),
            colAddress);
         szNote += "-> " + CollectionObject::ToCardLine(
            currentSnapshot->GetAddress(),
            item->GetName(),
            currentSnapshot->GetIdentifyingAttributes(),
            currentSnapshot->GetMetaTags(MetaTagType::Visible),
            colAddress);
         lstChanges.push_back(szNote);
      }
   }

   CollectionDeltaClass deltas;
   deltas.Additions = lstAdds;
   deltas.Changes = lstChanges;
   deltas.Removals = lstRemoves;

   return deltas;
}

