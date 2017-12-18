#include "CollectionIO.h"

#include "../Support/ListHelper.h"

using namespace std;

CollectionIO::CollectionIO()
{
}


CollectionIO::~CollectionIO()
{
}

bool CollectionIO::GetFileLines( string aszFileName,
                                 vector<string>& rlstFileLines )
{
   // Load the collection
   ifstream in(aszFileName);
   if( !in.good() )
   {
      return false;
   }

   stringstream buffer;
   buffer << in.rdbuf();
   in.close();
   string contents(buffer.str());

   rlstFileLines = StringHelper::SplitIntoLines(contents);
   return true;
}

// Returns the name of a collection and all the lines in it.
bool 
CollectionIO::GetNameAndCollectionLines(
      vector<string>  alstAllLines, 
      string&         rlstIOLines,
      vector<string>& rlstCollectionLines)
{
   // There must be one line with quotations. (Exactly One)
   // This is the name. All others will be interpreted as cards or thrown away.
   vector<string>::iterator iter_Lines = alstAllLines.begin();
   for (; iter_Lines != alstAllLines.end(); ++iter_Lines)
   {
      int iOne = iter_Lines->find_first_of('\"');
      int iTwo =  iter_Lines->find_last_of('\"');
      if( iOne != iTwo && iOne == 0 )
      {
         rlstIOLines = iter_Lines->substr(iOne+1, iTwo-iOne-1);
      }
      else
      {
         rlstCollectionLines.push_back(*iter_Lines);
      }
   }

   return true;
}

// Captures all copyitems in local to the input Location,
// not in rlstAlreadyCapturedItems. Puts the captured copyitems
// in rlstAdditionalItems.
bool
CollectionIO::CaptureUnlistedItems(
   Location aAddrColID,
   CollectionSource* aptCollectionSource,
   map<int, list<CopyItem*>>& rlstAdditionalItems,
   map<int, list<CopyItem*>>& rlstAlreadyCapturedItems)
{
   // Identify already loaded cards in this collection.
   auto lstAllPossibleCacheItems = aptCollectionSource->GetCollectionCache(aAddrColID);
   for( auto iCacheItem : lstAllPossibleCacheItems )
   {
      // Get all the new copies for this card.
      auto itemProto = aptCollectionSource->GetCardPrototype(iCacheItem);
      auto lstLocals = itemProto->FindCopies(aAddrColID, Local);

      // If there could be an existing copy, verify there isn't. Otherwise just add it.
      if( rlstAlreadyCapturedItems.find( iCacheItem ) != rlstAlreadyCapturedItems.end() )
      {
         auto lstExistingLocal = rlstAlreadyCapturedItems.at(iCacheItem);
         for( auto copy : lstLocals )
         {
            if( ListHelper::List_Find( copy.get(),
                                       lstExistingLocal.begin(),
                                       lstExistingLocal.end()    ) == -1 )
            {
               rlstAdditionalItems[iCacheItem].push_back(copy.get());
            }
         }
      }
      else
      {
         for( auto copy : lstLocals )
         {
            rlstAdditionalItems[iCacheItem].push_back(copy.get());
         }
      }

   }

   return true;
}

// Looks at each element in the two input lists.
// If there are matching items among them, reduce them to 1.
bool
CollectionIO::ConsolodateLocalItems(
   Location aAddrColID,
   CollectionSource* aptCollectionSource,
   map<int, list<CopyItem*>>& rlstPotentialDuplicates,
   map<int, list<CopyItem*>>& rlstNonDuplicates)
{
   const static function<string(CopyItem*&)> fnSimpleExtractor =
      [&](CopyItem*& item)->string { return item->GetHash(); };
   vector<CopyItem*> lstSearchItems;

   for( auto card : rlstPotentialDuplicates )
   {
      int iCard = card.first;

      // Check if this card class exists in nonDups.
      if( rlstNonDuplicates.find( card.first ) != rlstNonDuplicates.end() )
      {
         auto item = aptCollectionSource->GetCardPrototype( iCard );
         auto lstNewItems = &card.second;

         auto lstOldItems = &rlstNonDuplicates.at(iCard);
         lstSearchItems = vector<CopyItem*>(lstOldItems->begin(), lstOldItems->end());
         for( auto copy : *lstNewItems )
         {
            string szHash = copy->GetHash();
            int iNewItemIsInOld = ListHelper::List_Find_Non_Const(
                                                szHash, lstSearchItems,
                                                fnSimpleExtractor);
            if (-1 != iNewItemIsInOld)
            {
               CopyItem* foundItem = lstSearchItems[iNewItemIsInOld];
               lstOldItems->remove(foundItem);
               lstNewItems->remove(copy);
               item->DeleteCopy( foundItem );
            }
         }
      }
   }
  
   return true;
}

// Takes a two lists of copies. If there are copies that have the
// same UID, check which one has the later modification date.
// Delete the one with the earlier date.
bool CollectionIO::RejoinAsyncedLocalItems(
   Location aAddrColID,
   CollectionSource* aptCollectionSource,
   unsigned long aulNewItemTS,
   map<int, list<CopyItem*>>& rlstPotentialDuplicates,
   map<int, list<CopyItem*>>& rlstNonDuplicates )
{
   const static function<string( CopyItem*& )> fnChainIDExtractor =
      [&]( CopyItem*& item )->string { return item->GetSession(); };
   vector<CopyItem*> lstSearchItems;

   for( auto card : rlstPotentialDuplicates )
   {
      int iCard = card.first;

      // Check if this card class exists in nonDups.
      if( rlstNonDuplicates.find( card.first ) != rlstNonDuplicates.end() )
      {
         auto item = aptCollectionSource->GetCardPrototype( iCard );
         auto lstNewItems = &card.second;

         auto lstOldItems = &rlstNonDuplicates.at( iCard );
         lstSearchItems = vector<CopyItem*>( lstOldItems->begin(), lstOldItems->end() );
         for( auto newCopy : *lstNewItems )
         {
            string szUID = newCopy->GetUID();
            int iFound = ListHelper::List_Find_Non_Const( szUID, lstSearchItems,
                                                          fnChainIDExtractor );
            if( -1 != iFound )
            {
               CopyItem* oldCopy = lstSearchItems[iFound];

               string szSessionNew = newCopy->GetSession();
               string szSessionOld = oldCopy->GetSession();

               // If there is a match, check the Session to see which one is newer.
               unsigned long lSessNew = stoul( szSessionNew, nullptr, 16 );
               unsigned long lSessOld = stoul( szSessionOld, nullptr, 16 );

               if( lSessNew >= lSessOld )
               { 
                  // Use the new
                  // Get all the residents in the old copy.
                  // These are collections that were loaded before this
                  // collection and referenced this card.
                  for( auto resident : oldCopy->GetResidentIn() )
                  {
                     newCopy->AddResident(resident);
                  }

                  lstOldItems->remove( oldCopy );
                  item->DeleteCopy( oldCopy );
               }
               else
               { 
                  // Use the old
                  lstNewItems->remove( newCopy );
                  item->DeleteCopy( newCopy );
               }
            }
         }
      }
   }

   // There should be no cards with matching UIDs left.
   // If there are cards still in the old list,
   // they were either 
   // 1. Removed,
   // 2. Added without this collection loaded.
   // The solution to this is to accept the old card
   // if it has a newer time stamp than when this collection
   // was last modified. Otherwise, delete the card.
   //
   // This doesn't always capture the case that should happen,
   // Suppose B < A. B is loaded, and item 'a' is removed.
   // Then, B is unloaded, A is loaded and some modifications
   // are made to A; A's timestamp is now later than B. 
   // The next time A and B are both loaded, the item will be
   // added back to B.
   for( auto card : rlstNonDuplicates )
   {
      // Check if this card class exists in nonDups.
      auto item = aptCollectionSource->GetCardPrototype( card.first );
      auto lstOldItems = &card.second;

      for( auto oldCopy : *lstOldItems )
      {
         string szSessionOld = oldCopy->GetSession();

         // If there is a match, check the Session to see which one is newer.
         unsigned long lSessOld = stoul(szSessionOld, nullptr, 16);

         // aulNewItemTS is the time that the collection in loading was
         // last modified. If this collection 
         if( lSessOld <= aulNewItemTS )
         { 
            // Remove the 'old' item.
            item->RemoveCopy( aAddrColID, oldCopy->GetUID() );
         }
      }
   }

   return true;
}

// Looks for items that this collection claims to have borrowed.
// If the borrowed from collection exists, make sure the copy
// still exists, and use that. If the copy no long exists, 
// delete the copy that we are 'borrowing'. If the
// collection doesn't exist, look to see of any other loaded
// collections reference the copy, if so, use that reference
// and delete the newly created borrowed copy.
bool
CollectionIO::ConsolodateBorrowedItems(
   Location aAddrColID,
   CollectionSource* aptCollectionSource,
   CollectionFactory* aptCollFactory)
{
   // Used to filter out already used existing copies
   const static function<string(const shared_ptr<CopyItem>&)> fnExtractor
      = [aAddrColID](const shared_ptr<CopyItem>& item)->string
      {
         if (!item->IsResidentIn(aAddrColID)) { return item->GetUID(); }
         else { return ""; }
      };

   string szItemParent;
   string szUID;

   vector<int> lstCacheIndexes = aptCollectionSource->GetCollectionCache(aAddrColID);

   for( auto iCard : lstCacheIndexes )
   {
      auto itemPrototype = aptCollectionSource->GetCardPrototype(iCard);

      // Already created 'borrowed' copies will not show up here because the aAddrColID will
      // not be referenced in a resident of another copy, unless this collection put it there.
      // Since this collection did not put itself as a resident anywhere yet, this FindCopies
      // will only return newly created borrowed items.
      auto lstBorrowedItems = itemPrototype->FindCopies(aAddrColID, CollectionObjectType::Borrowed);
      for( auto borrowedCopy : lstBorrowedItems )
      {
         szItemParent = borrowedCopy->GetParent();
         szUID = borrowedCopy->GetUID();

         // The aoFactory is used to check if the collection that 
         // we are borrowing from is loaded.
         if( aptCollFactory->CollectionExists( szItemParent ) )
         {
            // Erase the borrowed copy if the collection exists.
            itemPrototype->DeleteCopy(borrowedCopy.get());

            // LstCopies contains all the copies of the card in the collection
            // that we borrowed from. Note this is different from below because
            // the borrowed copy will not show up in this list.
            auto lstCopies = itemPrototype->FindCopies( borrowedCopy->GetAddress(), 
                                                        CollectionObjectType::Local );

            // Look for a copy with a matching UID that is not already 
            // resident in this collection.
            int iUnused = ListHelper::List_Find( szUID, 
                                                 lstCopies,
                                                 fnExtractor );
            if (iUnused != -1)
            {
               lstCopies[iUnused]->AddResident(aAddrColID);
            }
         }
         else // Collection we borrowed from is not loaded.
         { 
            // Check if any other collection referenced the unverified copy.
            // Get a list of all other cards that supposedly belong to this collection
            auto lstCopies = itemPrototype->FindCopies( borrowedCopy->GetAddress(), 
                                                        CollectionObjectType::Local );
            
            // Look for a copy with a matching UID that is not already 
            // resident in this collection.
            int iUnused = ListHelper::List_Find( szUID,
                                                 lstCopies,
                                                 fnExtractor );
            if( iUnused != -1 )
            {
               itemPrototype->DeleteCopy(borrowedCopy.get());
               lstCopies[iUnused]->AddResident(aAddrColID);
            }
         }
      }
   }
   
   return true;
}

bool 
CollectionIO::RegisterRemainingInList( std::vector<int>& alstRegistry,
                              std::map<int, std::list<CopyItem*>>& amapNewItems )
{
   for( auto item : amapNewItems )
   {
      int iItem = item.first;
      int iFound = ListHelper::List_Find(iItem, alstRegistry);
      if( iFound == -1 )
      {
         alstRegistry.push_back(iItem);
      }
   }

   return true;
}

// This only be called when we know that all cards in the aAddrColID
// are there rightfully, and that there are no cards that should exist
// outside this collection if they reference this collection.
bool
CollectionIO::ReleaseUnfoundReferences(
   Location aAddrColID,
   CollectionSource* aptCollectionSource)
{
   const static function<string( shared_ptr<CopyItem>& )> fnChainIDExtractor =
      [&]( shared_ptr<CopyItem>& item )->string { return item->GetSession(); };

   auto lstAllCards = aptCollectionSource->GetCollectionCache(aAddrColID);
   for( auto iCard : lstAllCards )
   {
      auto card = aptCollectionSource->GetCardPrototype( iCard ).Value();

      // Get a list of all cards that say this collection is
      // their location.
      auto lstLocals = card->FindCopies( aAddrColID, Local );
      for( auto copy : lstLocals )
      {
         if( !copy->IsReferencedBy( aAddrColID ) )
         {
            card->DeleteCopy( copy.get() );
         }
      }
   }

   return true;
}

bool CollectionIO::CollectionFileExists(string aszFileName)
{
   string szFullFileName = GetCollectionFile(aszFileName);
   ifstream f(szFullFileName.c_str());
   return f.good();
}

string CollectionIO::GetCollectionFile(string aszCollectionName)
{
   return Config::Instance()->GetCollectionsDirectory() + "\\" +
      StringHelper::Str_Replace(aszCollectionName, ' ', '_') + ".txt";
}

string CollectionIO::GetMetaFile(string aszCollectionName)
{
   Config* config = Config::Instance();
   return config->GetCollectionsDirectory() + "\\" + 
      config->GetMetaFolderName() + "\\" +
      StringHelper::Str_Replace(aszCollectionName, ' ', '_') + "." + 
      string(Config::MetaFileExtension) + ".txt";
}

string CollectionIO::GetHistoryFile(string aszCollectionName)
{
   Config* config = Config::Instance();
   return config->GetCollectionsDirectory() + "\\" +
      config->GetHistoryFolderName() + "\\" +
      StringHelper::Str_Replace(aszCollectionName, ' ', '_') + "." + 
      string(Config::HistoryFileExtension) + ".txt";
}

std::string 
CollectionIO::GetOverheadFile( std::string aszCollectionName )
{
   Config* config = Config::Instance();
   return config->GetCollectionsDirectory() + "\\" +
      config->GetHistoryFolderName() + "\\" +
      StringHelper::Str_Replace(aszCollectionName, ' ', '_') + "." + 
      string(Config::OverheadFileExtension) + ".txt";
}