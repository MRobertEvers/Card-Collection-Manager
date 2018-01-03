#pragma once
#include "CollectionSource.h"
#include "CollectionDetails.h"
#include "CollectionTracker.h"
#include "TransactionManager.h"

using namespace std;

class CollectionFactory;

// Collection
//  Maintains a list of Collection Objects.
class Collection
{
public:
   class Query 
   {
   public:
      Query(bool Collapsed = false) { Default(Collapsed); }

      void Default(bool Collapsed = false)
      {
         m_bCollapsed = Collapsed;
         // MetaTags are include in hash calculation if not selected here.
         m_htHash = CopyItem::HashType::Default;
         m_mttMetaType = MetaTagType::Visible;
         m_bIsShort = false;
         m_bIncludeCount = true;
         m_bUIDs = false;
      }
      void UIDs() { Default(true); m_bUIDs = true; }
      Query& Short() { m_bIsShort = true; return *this; }
      Query& IncludeCount() { m_bIncludeCount = true; }
      Query& HashType(CopyItem::HashType hashType) { m_htHash = hashType; return *this; }
      Query& HashAny() { m_htHash = CopyItem::HashType::Default; return *this; }
      Query& HashIDs() { m_htHash = CopyItem::HashType::Ids; return *this; }
      Query& HashMeta() { m_htHash = CopyItem::HashType::Meta; return *this; }
      Query& SearchFor(const string& Search) { m_szSearch = Search; return *this; }
      Query& IncludeMetaType(MetaTagType mtType) { m_mttMetaType = mtType; return *this; }
      Query& AnyMeta(MetaTagType mtType) { m_mttMetaType = MetaTagType::Any; return *this; }
      Query& NoMeta(MetaTagType mtType) { m_mttMetaType = MetaTagType::None; return *this; }
      Query& PublicMeta(MetaTagType mtType) { m_mttMetaType = MetaTagType::Public; return *this; }
      Query& IgnoredMeta(MetaTagType mtType) { m_mttMetaType = MetaTagType::Ignored; return *this; }
      Query& TrackingMeta(MetaTagType mtType) { m_mttMetaType = MetaTagType::Tracking; return *this; }
      Query& HiddenMeta(MetaTagType mtType) { m_mttMetaType = MetaTagType::Hidden; return *this; }

      bool GetIncludeCount() { return m_bIncludeCount; }
      bool GetShort() { return m_bIsShort; }
      string GetSearch() { return m_szSearch; }
      bool GetCollapsed() { return m_bCollapsed; }
      CopyItem::HashType GetHashType() { return m_htHash; }
      MetaTagType GetMetaType() { return m_mttMetaType; }
      bool GetUIDs() { return m_bUIDs; }

   private:
      bool m_bUIDs;
      bool m_bIsShort;
      string m_szSearch;
      bool m_bCollapsed;
      bool m_bIncludeCount;
      CopyItem::HashType m_htHash;
      MetaTagType m_mttMetaType;
   };

   Collection(
      string aszName,
      CollectionSource* aoSource,
      string aszID = "");
   ~Collection();

   string GetName();
   Location  GetIdentifier();
   unsigned int GetChildCount();
   void ChildAdded();

   void AddItem(
      string aszName,
      vector<Tag> alstAttrs = vector<Tag>(),
      vector<Tag> alstMetaTags = vector<Tag>(),
      bool abCloseTransaction = true);

   void AddItemFrom(
      string aszName,
      string aszUID,
      const Location& aAddress,
      bool abCloseTransaction = true);

   void RemoveItem(
      string aszName,
      string aszUID,
      bool abCloseTransaction = true);

   void ChangeItem(
      string aszName,
      string aszUID,
      vector<Tag> alstIdChanges,
      vector<Tag> alstMetaChanges,
      bool abCloseTransaction = true);

   void ReplaceItem(
      string aszName,
      string aszUID,
      string aszNewName,
      vector<Tag> alstIdChanges,
      vector<Tag> alstMetaChanges,
      bool abCloseTransaction = true);

   vector<string> GetMetaData();

   void SaveCollection();

   bool InitializeCollection();
   bool InitializeCollection(string aszFileName,
      vector<string>& rlstInitializeLines);
   void LoadCollection(
      const string& aszFileName,
      CollectionFactory* aoFactory);
   void LoadChanges(vector<string> aszLines);

   vector<string> QueryCollection( Query aiQueryParms );

   vector<string> GetCollectionList(
      MetaTagType atagType = Visible,
      bool abCollapsed = true,
      CopyItem::HashType ahashType = CopyItem::HashType::Default);


   vector<string> GetShortList();

   bool IsLoaded = false;

private:
   friend class CollectionTracker;
   friend class TransactionManager;

   CollectionDetails* m_ptrCollectionDetails;
   CollectionSource* m_ptrCollectionSource;
   CollectionTracker* m_ptrCollectionTracker;
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

   void expandAdditionLine(string& aszLine);
   void collapseCardLine(string& aszLine);

   void saveHistory();
   void saveMeta();
   void saveOverhead();
   void saveCollection();
};


