#include "Collection.h"

#include <ctime>
#include <iomanip>
#include <algorithm>

#include "../Support/ListHelper.h"
#include "../Support/StringHelper.h"
#include "../Addressing/Addresser.h"
#include "../Support/Config.h"
#include "AddAction.h"
#include "RemoveAction.h"
#include "ChangeAction.h"
#include "ReplaceAction.h"
#include "AddFromAction.h"
#include "CollectionIO.h"
#include "../StringInterface.h"

using namespace std;

// An ID will be given to the collection if there is a parent.
Collection::Collection(string aszName,
   CollectionSource* aoSource,
   string aszID) {
   m_ptrCollectionTracker = new CollectionTracker(this);
   m_ptrCollectionDetails = new CollectionDetails();
   m_ptrTransactionManager = new TransactionManager(this);

   m_ptrCollectionDetails->SetName(aszName);
   m_ptrCollectionDetails->SetFileName(aszName, true);
   m_ptrCollectionDetails->SetChildrenCount(0);
   m_ptrCollectionDetails->AssignAddress(aszID);

   m_ptrCollectionSource = aoSource;

   m_ptrCollectionTracker->Track();
}


Collection::~Collection() {
   delete m_ptrCollectionTracker;
   delete m_ptrCollectionDetails;
   delete m_ptrTransactionManager;
}

string Collection::GetName() {
   return m_ptrCollectionDetails->GetName();
}

Location Collection::GetIdentifier() {

   return Location(*m_ptrCollectionDetails->GetAddress());
}

unsigned int
Collection::GetChildCount() {
   return m_ptrCollectionDetails->GetChildrenCount();
}

void
Collection::ChildAdded() {
   m_ptrCollectionDetails->IncrementChildCount();
}

void
Collection::AddItem(string aszName,
   vector<Tag> alstAttrs,
   vector<Tag> alstMetaTags,
   bool abCloseTransaction) {
   AddAction addAction;
   addAction.SetIDs(alstAttrs);
   addAction.SetMeta(alstMetaTags);
   addAction.SetName(aszName);

   m_ptrTransactionManager->IncludeAction(addAction);

   m_ptrTransactionManager->FinalizeTransaction(abCloseTransaction);
}

void
Collection::AddItemFrom(
   string aszName,
   string aszIdentifyingHash,
   const Location& aAddress,
   bool abCloseTransaction) {
   AddFromAction afAction;
   afAction.SetHash(aszIdentifyingHash);
   afAction.SetName(aszName);
   //   afAction.SetResi(aAddress);

   m_ptrTransactionManager->IncludeAction(afAction);

   m_ptrTransactionManager->FinalizeTransaction(abCloseTransaction);
}

void
Collection::RemoveItem(string aszName,
   string aszIdentifyingHash,
   bool abCloseTransaction) {
   RemoveAction rmAction;
   rmAction.SetUID(aszIdentifyingHash);
   rmAction.SetName(aszName);

   m_ptrTransactionManager->IncludeAction(rmAction);

   m_ptrTransactionManager->FinalizeTransaction(abCloseTransaction);
}

void
Collection::ChangeItem(string aszName,
   string aszIdentifyingHash,
   vector<Tag> alstChanges,
   vector<Tag> alstMetaChanges,
   bool abCloseTransaction) {
   ChangeAction chAction;
   chAction.SetIDs(alstChanges);
   chAction.SetMeta(alstMetaChanges);
   chAction.SetUID(aszIdentifyingHash);
   chAction.SetName(aszName);

   m_ptrTransactionManager->IncludeAction(chAction);

   m_ptrTransactionManager->FinalizeTransaction(abCloseTransaction);
}

void
Collection::ReplaceItem(
   string aszName,
   string aszIdentifyingHash,
   string aszNewName,
   vector<Tag> alstIdChanges,
   vector<Tag> alstMetaChanges,
   bool abCloseTransaction) {
   ReplaceAction rpAction;
   rpAction.SetIDs(alstIdChanges);
   rpAction.SetMeta(alstMetaChanges);
   rpAction.SetNewCard(aszNewName);
   rpAction.SetUID(aszIdentifyingHash);
   rpAction.SetName(aszName);

   m_ptrTransactionManager->IncludeAction(rpAction);

   m_ptrTransactionManager->FinalizeTransaction(abCloseTransaction);
}

vector<string>
Collection::GetMetaData() {
   vector<string> lstRetval;
   lstRetval.push_back("Name=\"" + GetName() + "\"");

   lstRetval.push_back("ID=\"" + GetIdentifier().GetFullAddress() + "\"");

   // For Each tagged item, produces "LongName : MetaData"
   for(auto metaData : m_lstTaggedItems) {
      string szMetaData = metaData.first + ": {" +
         metaData.second.first + "=\"" +
         metaData.second.second + "\" }";
      lstRetval.push_back(szMetaData);
   }

   return lstRetval;
}

void
Collection::SaveCollection() {
   m_ptrCollectionDetails->SetTimeStamp();

   saveOverhead();

   saveHistory();

   saveMeta();

   saveCollection();
}

bool
Collection::InitializeCollection() {
   m_ptrCollectionDetails->SetInitialized(true);
   return true;
}

// Loads the data file of the collection.
bool
Collection::InitializeCollection( string aszFileName,
                                  vector<string>& rlstInitializeLines ) 
{
   if( !m_ptrCollectionDetails->IsInitialized() ) 
   {
      vector<string> lstCardLines;
      vector<string> lstFileLines;
      CollectionIO loader;
      string szName;

      if( !loader.GetFileLines(aszFileName, lstFileLines) )
      {
         return false;
      }

      m_ptrCollectionDetails->SetFile(aszFileName);

      loader.GetNameAndCollectionLines(lstFileLines, szName, lstCardLines);
      m_ptrCollectionDetails->SetName(szName);

      loadOverheadFile(rlstInitializeLines);
      m_ptrCollectionDetails->SetProcessLines(rlstInitializeLines);

      m_ptrCollectionDetails->SetInitialized(true);
   }

   return true;
}

void
Collection::LoadCollection( const string& aszFileName,
                            CollectionFactory* aoFactory )
{
   map<int, list<CopyItem*>> mapNewlyAddedItems;
   map<int, list<CopyItem*>> mapExistingItems;
   vector<string> lstDummyList;
   vector<string> lstFileLines;
   vector<string> lstCardLines;
   CollectionIO loader;
   string szName;

   // Loads the name, etc. if necessary. Usually this is already done by now.
   InitializeCollection(aszFileName, lstDummyList);

   loader.GetFileLines(aszFileName, lstFileLines);
   loader.GetNameAndCollectionLines(lstFileLines, szName, lstCardLines);

   // If the cardline is shorthand, expand it.
   for( auto& szCardLine : lstCardLines ) {
      expandAdditionLine(szCardLine);
   }

   // Store off all the existing copies in mapExistingItems.
   loader.CaptureUnlistedItems(GetIdentifier(),
      m_ptrCollectionSource,
      mapExistingItems,
      mapNewlyAddedItems);

   // Load the collection based on the file.
   LoadChanges(lstCardLines);
   loadMetaTagFile();

   // Store off all the copies that aren't in mapExistingItems.
   // Store them in mapNewlyAddedItems.
   loader.CaptureUnlistedItems(GetIdentifier(),
      m_ptrCollectionSource,
      mapNewlyAddedItems,
      mapExistingItems);

   // Consolodate local items that match exactly in the newly
   // added and existing items.
   loader.ConsolodateLocalItems(GetIdentifier(),
      m_ptrCollectionSource,
      mapNewlyAddedItems,
      mapExistingItems);

   // If we have two identical copies with unlike session dates,
   // use the later ones.
   loader.RejoinAsyncedLocalItems(GetIdentifier(),
      m_ptrCollectionSource,
      m_ptrCollectionDetails->GetTimeStamp(),
      mapNewlyAddedItems,
      mapExistingItems);

   // Now Verify Borrowed Cards (i.e. Parent != this) that were just
   //  loaded exist. Two things can happen in this case. 
   // If the claimed collection exists, then try to find the referenced item
   //  and use that instead, if that fails, delete the item.
   // If the claimed collection does not exist, then try to find an identical
   //  copy that may have been created by another collection and use that.
   //  If that fails, use the one created.
   loader.ConsolodateBorrowedItems(GetIdentifier(),
      m_ptrCollectionSource,
      aoFactory);

   // Make sure the remaining items are registered.
   loader.RegisterRemainingInList(m_lstItemCacheIndexes,
      mapExistingItems);

   // Now this collection is COMPLETELY LOADED. Since other collections can
   //  reference this collection, without this collection being loaded,
   //  those other collections may have created copies of card in this 
   //  collection already; if that is the case, use those copies. Additionally,
   //  check that all the copies referenced by the other collections still
   //  exist, if not, delete those copies.
   loader.ReleaseUnfoundReferences(GetIdentifier(),
      m_ptrCollectionSource);

   IsLoaded = (GetIdentifier().GetMain() != "");

   if( IsLoaded ) {
      m_ptrCollectionTracker->Track();

      if( GetName() == "" ) {
         m_ptrCollectionDetails->SetName(aszFileName);
      }
   }
}

// Returns all the copies impacted by this function.
void
Collection::LoadChanges(vector<string> lstLines) {
   vector<string>::iterator iter_Lines = lstLines.begin();
   for( ; iter_Lines != lstLines.end(); ++iter_Lines ) {
      loadInterfaceLine(*iter_Lines);
   }
}

vector<string> 
Collection::QueryCollection(Query aiQueryParms)
{
   struct ItemData
   {
   public:
      ItemData() {};
      ItemData(const ItemData& other)
      {
         Name = other.Name;
         Hash = other.Hash;
         Count = other.Count;
         Front = other.Front;
         Item = other.Item;
         Copy = other.Copy;
         UIDs = vector<string>(other.UIDs);
      }
      string Name;
      string Hash;
      unsigned int Count = 1;
      bool Front = true;
      CollectionObject* Item;
      CopyItem* Copy;
      vector<string> UIDs;
   };

   // 1. Count Each item with the same hash if collapsed, otherwise, just add the item and continue.
   // <hash, (card name, count)>
   // Its multi because some hashes clash between cards.
   // These are unused if this is not a collapsed search.
   multimap<string, ItemData> mapSeenHashes;
   map<string, ItemData> mapCardHashes;
   for( auto iItem : getCollection() )
   {
      auto item = m_ptrCollectionSource->GetCardPrototype(iItem);
      ItemData cardData;
      cardData.Name = item->GetName();
      cardData.Item = item.Value();

      // 1. If we are only looking for certain card names, restrict here
      // so we can save time by skipping this iteration.
      if( aiQueryParms.GetSearch() != "" )
      {
         string szSearch = aiQueryParms.GetSearch();
         transform(szSearch.begin(), szSearch.end(), szSearch.begin(), ::tolower);
         string szName = cardData.Name;
         transform(szName.begin(), szName.end(), szName.begin(), ::tolower);

         auto iFindName = szName.find(szSearch);
         cardData.Front = iFindName == 0;
         if( iFindName == string::npos )
         {
            continue;
         }
      }

      // 1. Count each occurence of an identical hash within a card if collapsing,
      // otherwise add it.
      mapCardHashes.clear();
      auto lstCopies = item->FindCopies(GetIdentifier(), All);
      for( auto copy : lstCopies )
      {
         // Look for copies that match.
         cardData.Hash = copy->GetHash(aiQueryParms.GetHashType());
         auto iter_Counted = mapCardHashes.find(cardData.Hash);
         if( ( aiQueryParms.GetCollapsed() ) &&
             ( iter_Counted != mapCardHashes.end() ) )
         {
            // If we are collapsing, and there is a match, count it. Otherwise, add card data.
            iter_Counted->second.Count++; 
            iter_Counted->second.UIDs.push_back(copy->GetUID());
         }
         else
         {
            cardData.Copy = copy.get();
            cardData.UIDs.push_back(copy->GetUID());
            mapCardHashes.insert(make_pair(cardData.Hash, cardData));
         }
      }

      mapSeenHashes.insert(mapCardHashes.begin(), mapCardHashes.end());
   }

   vector<string> lstFront;
   vector<string> lstBack;
   // 3. Create the list of strings, shorten them if necessary.
   for( auto card : mapSeenHashes )
   {
      string szLine;
      ItemData& cardData = card.second;

      // Build the strings
      if( aiQueryParms.GetIncludeCount() )
      {
         // ToCardLine does not include the count if its 0.
         cardData.Count = 0;
      }

      // UIDs replaces the meta tags with UIDs.
      vector<Tag> vecMeta;
      if( aiQueryParms.GetUIDs() )
      {
         cardData.Count = 0;
         for( auto& szUID : cardData.UIDs )
         {
            vecMeta.push_back(make_pair(CopyItem::GetUIDKey(),szUID));
         }
      }
      else
      {
         vecMeta = cardData.Copy->GetMetaTags(aiQueryParms.GetMetaType());
      }

      szLine = CollectionObject::ToCardLine(
         cardData.Copy->GetAddress(),
         cardData.Name,
         cardData.Copy->GetIdentifyingAttributes(),
         vecMeta,
         *this->m_ptrCollectionDetails->GetAddress(),
         cardData.Count
         );

      if( aiQueryParms.GetShort() )
      {
         collapseCardLine(szLine);
      }

      if( cardData.Front )
      {
         lstFront.push_back(szLine);
      }
      else
      {
         lstBack.push_back(szLine);
      }
   }

   vector<string> lstRetVal;
   for( auto& szLine : lstFront )
   {
      lstRetVal.push_back(szLine);
   }

   for( auto& szLine : lstBack )
   {
      lstRetVal.push_back(szLine);
   }

   return lstRetVal;
}

vector<string>
Collection::GetCollectionList( MetaTagType atagType,
                               bool abCollapsed,
                               CopyItem::HashType ahashType ) 
{
   vector<string> lstRetVal;
   
   // <hash, (card name, count)>
   // Its multi because some hashes clash between cards.
   multimap<string, pair<string, unsigned int>> mapSeenHashes;
   map<string, pair<string, unsigned int>> mapCardHashes;

   vector<int> lstCol = getCollection();
   for( auto iItem : lstCol ) 
   {
      auto item = m_ptrCollectionSource->GetCardPrototype(iItem);
      auto lstCopies = item->FindCopies(GetIdentifier(), All);
      mapCardHashes.clear();

      for( auto copy : lstCopies )
      {
         string szHash = copy->GetHash(ahashType);
         auto iter_Counted = mapCardHashes.find(szHash);
         if( (!abCollapsed) || (iter_Counted == mapCardHashes.end()) )
         {
            string szRep = item->CopyToString( copy.get(), atagType,
                                               GetIdentifier() );
            auto pairCard = make_pair(szRep, 1);
            mapCardHashes.insert(make_pair(szHash, pairCard));
         }
         else if( iter_Counted != mapCardHashes.end() )
         {
            iter_Counted->second.second++;
         }
      }

      mapSeenHashes.insert(mapCardHashes.begin(), mapCardHashes.end());
   }

   if( abCollapsed )
   {
      vector<string> lstNewRetVal;
      for( auto& pCard : mapSeenHashes )
      {
         string szCard = pCard.second.first;
         int iCount = pCard.second.second;

         string szLine = "x" + to_string(iCount) + " " + szCard;
         lstNewRetVal.push_back(szLine);
      }

      lstRetVal.clear();
      lstRetVal = lstNewRetVal;
   }

   return lstRetVal;
}

// Returns each item in this collection with a list of its UIDs.
vector<string>
Collection::GetShortList() 
{
   StringInterface stringIface;

   // <hash, (card name, UID List)>
   multimap<string, pair<string, vector<string>>> mapSeenHashes;
   map<string, pair<string, vector<string>>> mapCardHashes;

   vector<int> lstCol = getCollection();
   for( auto& iItem : lstCol )
   {
      auto item = m_ptrCollectionSource->GetCardPrototype(iItem);
      auto lstCopies = item->FindCopies(GetIdentifier(), All);
      mapCardHashes.clear();

      for( auto& copy : lstCopies ) 
      {
         string szHash = copy->GetHash();
         string szCopyUID = copy->GetUID();

         auto iter_Counted = mapCardHashes.find(szHash);
         if( iter_Counted == mapCardHashes.end() )
         {
            auto vecOnlyUID = vector<string> { szCopyUID };
            string szRep = item->CopyToString(copy.get(), None);
            auto pairCardAndUID = make_pair(szRep, vecOnlyUID);

            auto pairHashUID = make_pair(szHash, pairCardAndUID);
            mapCardHashes.insert(pairHashUID);
         }
         else
         {
            iter_Counted->second.second.push_back(szCopyUID);
         }
      }

      mapSeenHashes.insert(mapCardHashes.begin(), mapCardHashes.end());
   }

   string szUIDKey = CopyItem::GetUIDKey();
   vector<string> vecNewRetval;
   vector<Tag> vecUIDPairs;
   for( auto& pCard : mapSeenHashes )
   {
      int iCount = pCard.second.second.size();
      string szCard = pCard.second.first;

      string szLine = "x" + to_string(iCount) + " " + szCard;

      vecUIDPairs.clear();
      for( auto& szUID : pCard.second.second )
      {
         vecUIDPairs.push_back(make_pair(szUIDKey, szUID));
      }

      szLine = stringIface.ToCardLine(szLine, vector<Tag>(), vecUIDPairs);
      vecNewRetval.push_back(szLine);
   }

   return vecNewRetval;
}

vector<int>
Collection::getCollection() {
   if( m_ptrCollectionSource->IsSyncNeeded(GetIdentifier()) ) {
      m_lstItemCacheIndexes = m_ptrCollectionSource->
         GetCollectionCache(GetIdentifier());

      m_ptrTransactionManager->TransactionsAsynced();
   }

   return m_lstItemCacheIndexes;
}

CopyItem*
Collection::addItem(
   const string& aszName,
   const vector<Tag>& alstAttrs,
   const vector<Tag>& alstMetaTags) {
   CopyItem* cItem;
   int iCache = m_ptrCollectionSource->LoadCard(aszName);
   if( iCache == -1 ) {
      // TODO Could Not Load.
      return nullptr;
   }

   auto item = m_ptrCollectionSource->GetCardPrototype(aszName);

   cItem = item->AddCopy(GetIdentifier(), alstAttrs, alstMetaTags).get();

   registerItem(iCache);

   // Notify other collections they may need to sync since this may have
   //  been borrowed by other collections.
   // There should never be other copies with that hash not in resident. 
   // They are removed at load time.
   m_ptrCollectionSource->NotifyNeedToSync(GetIdentifier());

   return cItem;
}

void
Collection::addItemFrom(
   const string& aszName,
   const string& aszIdentifyingHash,
   const Identifier& aResiAddress) {
   TryGet<CollectionObject> item;

   int iCache = m_ptrCollectionSource->LoadCard(aszName);
   item = m_ptrCollectionSource->GetCardPrototype(iCache);
   if( !item.Good() ) { return; }

   auto cItem = item->FindCopy(aszIdentifyingHash);
   if( !cItem.Good() ) { return; }
   auto copy = cItem.Value()->get();

   copy->AddResident(GetIdentifier());

   registerItem(iCache);
}

void
Collection::removeItem(const string& aszName,
   const string& aszUID) {
   // The copy is already verified to exist at this point
   int iCache = m_ptrCollectionSource->LoadCard(aszName);

   TryGet<CollectionObject> item = m_ptrCollectionSource->GetCardPrototype(iCache);

   item->RemoveCopy(GetIdentifier(), aszUID);

   // Remove any items from the cache that are no longer in this collection.
   int iRemainingCopies = item->FindCopies(GetIdentifier(), All).size();
   if( iRemainingCopies == 0 ) {
      unregisterItem(iCache);
   }

   // Notify other collections they may need to sync since this may have been 
   //  borrowed by other collections.
   m_ptrCollectionSource->NotifyNeedToSync(GetIdentifier());
}

void
Collection::changeItem(const string& aszName,
   const string& aszUID,
   const vector<Tag>& alstChanges,
   const vector<Tag>& alstMetaChanges) {
   int iCache = m_ptrCollectionSource->LoadCard(aszName);

   TryGet<CollectionObject> item = m_ptrCollectionSource->GetCardPrototype(iCache);
   CopyItem* cItem = item->FindCopy(aszUID).Value()->get();
   if( cItem == nullptr ) { return; }

   modifyItem(cItem, alstChanges, alstMetaChanges);

   // Notify other collections they may need to sync since this may have been
   //  borrowed by other collections.
   m_ptrCollectionSource->NotifyNeedToSync(GetIdentifier());
}


void
Collection::replaceItem(const string& aszName,
   const string& aszUID,
   const string& aszNewName,
   const vector<Tag>& alstIdChanges,
   const vector<Tag>& alstMetaChanges) {
   int iCache = m_ptrCollectionSource->LoadCard(aszName);
   int iNewCache = m_ptrCollectionSource->LoadCard(aszNewName);

   TryGet<CollectionObject> item = m_ptrCollectionSource->GetCardPrototype(iCache);
   TryGet<CollectionObject> newItem = m_ptrCollectionSource->GetCardPrototype(iNewCache);
   auto cItem = item->FindCopy(aszUID);
   if( !cItem.Good() ) { return; }

   removeItem(item->GetName(), aszUID);
   addItem(newItem->GetName(), alstIdChanges, alstMetaChanges);
}

void
Collection::registerItem(int aiCacheIndex) {
   int iFound = ListHelper::List_Find(aiCacheIndex, m_lstItemCacheIndexes);

   if( iFound == -1 ) {
      m_lstItemCacheIndexes.push_back(aiCacheIndex);
   }
}

void
Collection::unregisterItem(int aiCacheIndex) {
   vector<int> lstNewCacheIndexes;
   for( auto iCache : m_lstItemCacheIndexes ) {
      if( iCache != aiCacheIndex ) {
         lstNewCacheIndexes.push_back(iCache);
      }
   }

   m_lstItemCacheIndexes = lstNewCacheIndexes;
}

void
Collection::modifyItem(
   CopyItem* aptCopy,
   const vector<Tag>& alstChanges,
   const vector<Tag>& alstMetaChanges) {
   for( size_t i = 0; i < alstChanges.size(); i++ ) {
      aptCopy->
         SetIdentifyingAttribute(alstChanges[i].first, alstChanges[i].second);
   }

   for( size_t i = 0; i < alstMetaChanges.size(); i++ ) {
      MetaTagType mTagType = CopyItem::DetermineMetaTagType(alstMetaChanges[i].first);
      aptCopy->
         SetMetaTag(alstMetaChanges[i].first, alstMetaChanges[i].second, mTagType);
   }
}

// This should only be called during initial loading.
void Collection::loadMetaTagFile() {
   string szFileName;
   string szPlainHash;
   CollectionIO ioHelper;
   vector<string> lstMetaLines;
   TryGet<CollectionObject> item;

   szFileName = ioHelper.GetMetaFile(m_ptrCollectionDetails->GetFileName());
   ioHelper.GetFileLines(szFileName, lstMetaLines);

   for( size_t i = 0; i < lstMetaLines.size(); i++ ) {
      CollectionObject::PseudoIdentifier sudoItem;
      CollectionObject::ParseCardLine(lstMetaLines[i], sudoItem);

      vector<Tag> lstMetaTags = sudoItem.MetaTags;

      // Clear the meta so the hash may be obtained.
      sudoItem.MetaString = "";
      sudoItem.MetaTags.clear();

      int iRealCard = m_ptrCollectionSource->LoadCard(sudoItem.Name);
      if( iRealCard == -1 ) { continue; }

      item = m_ptrCollectionSource->GetCardPrototype(iRealCard);

      szPlainHash = item->GenerateHash(GetIdentifier(),
         sudoItem.Identifiers,
         sudoItem.MetaTags);

      // Gets the first matching item resident in this collection.
      auto matchingCopy = item->FindCopy(szPlainHash, Hash);
      if( matchingCopy.Good() ) {
         MetaTagType mTagType;
         auto copy = matchingCopy.Value()->get();
         for( size_t t = 0; t < lstMetaTags.size(); t++ ) {
            mTagType = CopyItem::DetermineMetaTagType(lstMetaTags[t].first);
            copy->SetMetaTag(lstMetaTags[t].first,
               lstMetaTags[t].second,
               mTagType, false);
         }
      }
   }
}

void Collection::loadOverheadFile(vector<string>& rlstUnprocessedLines) {
   // This should only be called during initial loading.
   string szFileName;
   CollectionIO ioHelper;
   vector<string> lstCollectionLines;

   szFileName = ioHelper.GetOverheadFile(m_ptrCollectionDetails->GetFileName());
   ioHelper.GetFileLines(szFileName, lstCollectionLines);

   auto iter_Lines = lstCollectionLines.cbegin();
   for( ; iter_Lines != lstCollectionLines.cend(); ++iter_Lines ) {
      // If the line is not an overhead data line, then it is a process
      // line.
      if( !loadOverheadLine(*iter_Lines) ) {
         rlstUnprocessedLines.push_back(*iter_Lines);
      }
   }
}

// Returns true if the line could be processed by the collections
// Only returns true for data lines.
bool Collection::loadOverheadLine(const string& aszLine) {
   string szDefKey(Config::CollectionDefinitionKey);
   if( aszLine.size() < 2 ) { return true; }
   if( aszLine.substr(0, szDefKey.size()) == szDefKey ) {
      // The line is a data line.
      loadCollectionDataLine(aszLine);
      return true;
   }
   else {
      return false;
   }
}

// Expects the input to be of the form : Key = "Value"
void
Collection::loadCollectionDataLine(const string& aszData) {
   string szBaseLine = aszData.substr(2);
   vector<string> lstSplitLine = StringHelper::Str_Split(szBaseLine, "=");

   if( lstSplitLine.size() != 2 ) { return; }

   vector<string>::iterator iter_Lines = lstSplitLine.begin();
   for( ; iter_Lines != lstSplitLine.end(); ++iter_Lines ) {
      *iter_Lines = StringHelper::Str_Trim(*iter_Lines, ' ');
   }

   string szKey = lstSplitLine.at(0);
   string szValue = lstSplitLine.at(1);
   szValue = StringHelper::Str_Trim(szValue, '\"');

   if( szKey == "Name" ) {
      m_ptrCollectionDetails->SetName(szValue);
   }
   else if( szKey == "ID" ) {
      m_ptrCollectionDetails->AssignAddress(szValue);
   }
   else if( szKey == "CC" ) {
      m_ptrCollectionDetails->SetChildrenCount(stoi(szValue));
   }
   else if( szKey == "Session" ) {
      tm tm{};
      istringstream str_stream(szValue);
      str_stream >> get_time(&tm, "%Y-%m-%d_%T");
      time_t time = mktime(&tm);
      m_ptrCollectionDetails->SetTimeStamp(time);
   }
}

// May return null depending on input
void
Collection::loadInterfaceLine(const string& aszLine) {
   if( aszLine.size() <= 2 ) { return; }

   string szTrimmedLine = StringHelper::Str_Trim(aszLine, ' ');

   string szLoadDirective = szTrimmedLine.substr(0, 1);

   if( szLoadDirective == "-" ) // REMOVE
   {
      szTrimmedLine = szTrimmedLine.substr(1);
      // Of the form ([] meaning optional)
      // Sylvan Card Name [{ set="val" color="val2" } ][: { metatag1="val" metatag2="val2" }]
      loadRemoveLine(szTrimmedLine);
   }
   else if( szLoadDirective == "%" ) // CHANGE
   {
      szTrimmedLine = szTrimmedLine.substr(1);
      // Of the form
      // Sylvan Card Name [{ set="val" color="val2" } ][: { metatag1="val" metatag2="val2" }] ->
      //   Another Card Name [{ set="val" color="val2" } ][: { metatag1="val" metatag2="val2" }]
      loadDeltaLine(szTrimmedLine);
   }
   else // ADD
   {
      if( szLoadDirective == "+" ) {
         szTrimmedLine = szTrimmedLine.substr(1);
      }
      // Of the form
      // Sylvan Card Name [{ set="val" color="val2" } ][: { metatag1="val" metatag2="val2" }]
      loadAdditionLine(szTrimmedLine);
   }
}

void Collection::loadAdditionLine(const string& aszLine) {
   string szID = "";
   string szLine = aszLine;
   Address aParentAddress;
   CollectionObject::PseudoIdentifier sudoItem;
   bool bThisIsParent = true;

   // Convert the line to the official form if needed.
   expandAdditionLine(szLine);

   CollectionObject::ParseCardLine(szLine, sudoItem);

   int iFoundAddress = ListHelper::List_Find(string("Parent"),
                                             sudoItem.MetaTags,
                                             Config::Instance()->GetTagHelper());
   if( iFoundAddress != -1 ) 
   {
      aParentAddress = Address(sudoItem.MetaTags.at(iFoundAddress).second);
      int iFoundHash = ListHelper::List_Find( string(Config::HashKey),
                                              sudoItem.MetaTags,
                                              Config::Instance()->GetTagHelper() );
      szID = sudoItem.MetaTags.at(iFoundHash).second;
      bThisIsParent = !(aParentAddress == GetIdentifier());
   }

   // If the ID is specified, then we assume the card already exists.
   if( !bThisIsParent && // This is not the parent
      szID != "" )       // and the id was specified.
   {
      for( size_t i = 0; i < sudoItem.Count; i++ )
      {
         //  AddItemFrom(sudoItem.Name, szID, aParentAddress);
      }
   }
   // If the parent was not specified, or this was designated the parent
   // without an ID, then add it as a new card.
   else
   {
      for( size_t i = 0; i < sudoItem.Count; i++ )
      {
         AddItem(sudoItem.Name, sudoItem.Identifiers, sudoItem.MetaTags);
      }
   }
}

// This needs "Card Name : { __hash="hashval" }" All other values are irrelevant.
void Collection::loadRemoveLine(const string& aszLine) {
   CollectionObject::PseudoIdentifier sudoItem;
   CollectionObject::ParseCardLine(aszLine, sudoItem);

   for( size_t i = 0; i < sudoItem.Count; i++ ) {
      string szUID;
      int iUID = ListHelper::List_Find(CopyItem::GetUIDKey(),
         sudoItem.MetaTags,
         Config::Instance()->GetTagHelper());
      if( iUID != -1 ) {
         szUID = sudoItem.MetaTags[iUID].second;
         RemoveItem(sudoItem.Name, szUID);
      }
      else { break; }
   }
}
void Collection::loadDeltaLine(const string& aszLine) {
   vector<string> lstOldNew = StringHelper::Str_Split(aszLine, "->");

   CollectionObject::PseudoIdentifier sudoOldItem;
   CollectionObject::ParseCardLine(lstOldNew[0], sudoOldItem);

   CollectionObject::PseudoIdentifier sudoNewItem;
   CollectionObject::ParseCardLine(lstOldNew[1], sudoNewItem);

   int iCache;
   int iUID = ListHelper::List_Find(CopyItem::GetUIDKey(),
      sudoOldItem.MetaTags,
      Config::Instance()->GetTagHelper());
   if( iUID != -1 &&
      (iCache = m_ptrCollectionSource->LoadCard(sudoOldItem.Name)) != -1 ) {
      string szUID; TryGet<CollectionObject> itemOld; CopyItem* cItem;

      szUID = sudoOldItem.MetaTags[iUID].second;
      itemOld = m_ptrCollectionSource->GetCardPrototype(iCache);
      cItem = itemOld->FindCopy(szUID)->get();

      for( size_t i = 0; i < sudoOldItem.Count; i++ ) {
         int iNewCache;
         if( sudoOldItem.Name == sudoNewItem.Name ) {
            ChangeItem(sudoOldItem.Name,
               szUID,
               sudoNewItem.Identifiers,
               sudoNewItem.MetaTags);
         }
         else if( (iNewCache = m_ptrCollectionSource->LoadCard(sudoNewItem.Name)) != -1 ) {
            TryGet<CollectionObject> itemNew = m_ptrCollectionSource->
               GetCardPrototype(iNewCache);
            ReplaceItem(sudoOldItem.Name,
               szUID,
               sudoNewItem.Name,
               sudoNewItem.Identifiers,
               sudoNewItem.MetaTags);
         }
      }

   }
}

// Modifies the string to the long hand version if it is a shorthand,
// otherwise, this does nothing.
// Takes the form Name [id]
// Can also take the form Name [id1=val1,id2=val2]
// Can also take the form Name [val1,val2]
void
Collection::expandAdditionLine(string& rszLine) 
{
   string szKey, szDetails, szName, szId, szCount;

   int iDetEnd = rszLine.find_first_of(']');
   int iNameStart = rszLine.find_first_of(' ');
   if( iNameStart == string::npos ) 
   {
      iNameStart = 0;
   }

   if( iDetEnd == rszLine.size() - 1 )
   {
      int iDetStart = rszLine.find_first_of('[');
      szCount = rszLine.substr(0, iNameStart);
      szName = rszLine.substr(iNameStart, iDetStart - iNameStart);
      szName = StringHelper::Str_Trim(szName, ' ');
      szId = rszLine.substr(iDetStart + 1, rszLine.size() - iDetStart - 2);
      auto card = m_ptrCollectionSource->GetCardPrototype(szName);

      auto vecVals = StringHelper::Str_Split(szId, ",");

      // Find the key for each unpair value.
      for( auto& szPair : vecVals ) {
         bool bGoodVal = false;

         // Check if the dets are solo.
         bool bNeedMatch = szId.find_first_of('=') == string::npos;
         if( bNeedMatch ) {
            if( card.Good() ) {
               bGoodVal = card->MatchIdentifyingTrait(szPair, szKey);
            }

            if( bGoodVal ) {
               szPair = szKey + "=" + szPair;
            }
         }
      }

      // Now we have to put quotes around each of the "values"
      for( auto& szPair : vecVals ) {
         auto vecSplitPair = StringHelper::Str_Split(szPair, "=");
         if( vecSplitPair.size() == 2 ) {
            string szKeyVal = vecSplitPair[0];
            string szValVal = vecSplitPair[1];

            // Make sure we dont double down on "
            szValVal = StringHelper::Str_Trim(szValVal, '"');
            szPair = szKeyVal + "=\"" + szValVal + "\"";
         }
      }

      // Wrap szID with { }
      szDetails = "{ ";
      for( auto& szPair : vecVals ) {
         szDetails += szPair;
         szDetails += " ";
      }
      szDetails += "}";

      rszLine = szCount + " " + szName + " " + szDetails;
   }
}

// This will ignore any meta tags.
void
Collection::collapseCardLine(string& rszLine)
{
  
}

void
Collection::saveHistory() {
   m_ptrCollectionTracker->Track();
   CollectionDeltaClass cdc = m_ptrCollectionTracker->CalculateChanges();

   vector<string> lstHistoryLines;
   for( size_t i = 0; i < cdc.Additions.size(); i++ ) {
      lstHistoryLines.push_back(cdc.Additions[i]);
   }

   for( size_t i = 0; i < cdc.Removals.size(); i++ ) {
      lstHistoryLines.push_back(cdc.Removals[i]);
   }

   for( size_t i = 0; i < cdc.Changes.size(); i++ ) {
      lstHistoryLines.push_back(cdc.Changes[i]);
   }

   if( lstHistoryLines.size() > 0 ) {
      string szTimeString = "";
      time_t now = time(0);
      struct tm timeinfo;
      localtime_s(&timeinfo, &now);
      char str[26];
      asctime_s(str, sizeof str, &timeinfo);
      str[strlen(str) - 1] = 0;

      CollectionIO ioHelper;
      ofstream oHistFile;
      string szHistFile = ioHelper.GetHistoryFile(m_ptrCollectionDetails->GetFileName());
      oHistFile.open(szHistFile, ios_base::app);

      oHistFile << "[" << str << "] " << endl;

      vector<string>::iterator iter_histLines = lstHistoryLines.begin();
      for( ; iter_histLines != lstHistoryLines.end(); ++iter_histLines ) {
         oHistFile << *iter_histLines << endl;
      }

      oHistFile.close();
   }
}

void Collection::saveMeta() {
   ofstream oMetaFile;
   CollectionIO ioHelper;

   vector<string> lstMetaLines = GetCollectionList(Persistent, false);

   oMetaFile.open(ioHelper.GetMetaFile(m_ptrCollectionDetails->GetFileName()));

   vector<string>::iterator iter_MetaLine = lstMetaLines.begin();
   for( ; iter_MetaLine != lstMetaLines.end(); ++iter_MetaLine ) {
      if( iter_MetaLine->find_first_of(':') != string::npos ) {
         oMetaFile << *iter_MetaLine << endl;
      }
   }

   oMetaFile.close();
}

void
Collection::saveOverhead() {
   ofstream oColFile;
   CollectionIO ioHelper;
   oColFile.open(ioHelper.GetOverheadFile(m_ptrCollectionDetails->GetFileName()));

   tm otm;
   time_t time = m_ptrCollectionDetails->GetTimeStamp();
   localtime_s(&otm, &time);

   oColFile << Config::CollectionDefinitionKey
      << " ID=\"" << GetIdentifier().GetFullAddress() << "\"" << endl;

   oColFile << Config::CollectionDefinitionKey
      << " CC=\"" << m_ptrCollectionDetails->GetChildrenCount() << "\"" << endl;

   oColFile << Config::CollectionDefinitionKey
      << " Session=\"" << put_time(&otm, "%F_%T") << "\"" << endl;

   for( auto szLine : m_ptrCollectionDetails->GetProcessLines() ) {
      oColFile << szLine << endl;
   }

   oColFile.close();
}

void Collection::saveCollection() {
   // Group lists only by id. When loading, these lists are only
   // used to create a template card. We only need the base details.
   vector<string> lstLines = GetCollectionList(None, true,
      CopyItem::HashType::Ids);

   // Convert the lines to shorthand
   for( auto& szLine : lstLines ) {
      collapseCardLine(szLine);
   }

   ofstream oColFile;
   oColFile.open(m_ptrCollectionDetails->GetFile());

   oColFile << "\"" << m_ptrCollectionDetails->GetName() << "\"" << endl;

   vector<string>::iterator iter_Line = lstLines.begin();
   for( ; iter_Line != lstLines.end(); ++iter_Line ) {
      oColFile << *iter_Line << endl;
   }

   oColFile.close();
}
