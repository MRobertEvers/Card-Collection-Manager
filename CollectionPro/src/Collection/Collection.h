#pragma once
#include "CollectionSource.h"
#include "CollectionDetails.h"
#include "CollectionTracker.h"
#include "CollectionQueryHelper.h"
#include "TransactionManager.h"

using namespace std;

class CollectionFactory;

// Collection
//  Maintains a list of Collection Objects.
class Collection
{
public:
   Collection( string aszName,
               CollectionSource* aoSource,
               string aszID = "" );
   ~Collection();

   string GetName();
   Location  GetIdentifier();
   unsigned int GetChildCount();
   void ChildAdded();

   void AddItem( string aszName,
                 vector<Tag> alstAttrs = vector<Tag>(),
                 vector<Tag> alstMetaTags = vector<Tag>(),
                 bool abCloseTransaction = true );

   void AddItemFrom( string aszName,
                     string aszUID,
                     const Location& aAddress,
                     bool abCloseTransaction = true );

   void RemoveItem( string aszName,
                    string aszUID,
                    bool abCloseTransaction = true );

   void ChangeItem( string aszName,
                    string aszUID,
                    vector<Tag> alstIdChanges,
                    vector<Tag> alstMetaChanges,
                    bool abCloseTransaction = true );

   void ReplaceItem( string aszName,
                     string aszUID,
                     string aszNewName,
                     vector<Tag> alstIdChanges,
                     vector<Tag> alstMetaChanges,
                     bool abCloseTransaction = true );

   vector<string> GetMetaData();

   void SaveCollection();

   bool InitializeCollection();
   bool InitializeCollection( string aszFileName,
                              vector<string>& rlstInitializeLines );
   void LoadCollection( const string& aszFileName,
                        CollectionFactory* aoFactory );
   void LoadChanges(vector<string> aszLines);

   vector<string> QueryCollection( Query aiQueryParms );

   bool IsLoaded = false;

private:
   friend class CollectionQueryHelper;
   friend class CollectionTracker;
   friend class TransactionManager;

   CollectionDetails* m_ptrCollectionDetails;
   CollectionSource* m_ptrCollectionSource;
   CollectionTracker* m_ptrCollectionTracker;
   CollectionQueryHelper* m_ptrCollectionQueryHelper;
   TransactionManager* m_ptrTransactionManager;

   vector<pair<string, Tag>> m_lstTaggedItems;

   vector<int> m_lstItemCacheIndexes;
   vector<int> getCollection();

   // These all locate by name and hash for a second 
   // time so we dont risk dangling references.
   CopyItem* addItem(
      const string& aszName,
      const vector<Tag>& alstAttrs,
      const vector<Tag>& alstMetaTags);

   void addItemFrom(
      const string& aszName,
      const string& aszUID,
      const Identifier& aAddress);

   void removeItem(
      const string& aszName,
      const string& aszUID);

   void changeItem(
      const string& aszName,
      const string& aszUID,
      const vector<Tag>& alstChanges,
      const vector<Tag>& alstMetaChanges);

   void replaceItem(
      const string& aszName,
      const string& aszUID,
      const string& aszNewName,
      const vector<Tag>& alstChanges,
      const vector<Tag>& alstMetaChanges);

   void registerItem(int aiCacheIndex);
   void unregisterItem(int aiCacheIndex);

   void modifyItem(
      CopyItem* aptCopy,
      const vector<Tag>& alstChanges,
      const vector<Tag>& alstMetaChanges);

   void loadMetaTagFile();

   void loadOverheadFile(vector<string>& rlstUnprocessedLines);
   bool loadOverheadLine(const string& aszLine);
   void loadCollectionDataLine(const string& aszData);

   void loadInterfaceLine(const string& aszLine);

   void loadAdditionLine(const string& aszLine);
   void loadRemoveLine(const string& aszLine);
   void loadDeltaLine(const string& aszLine);

   // TODO: move this to the collection source.
   void expandAdditionLine(string& aszLine);

private:
   void saveHistory();
   void saveMeta();
   void saveOverhead();
   void saveCollection();
};


