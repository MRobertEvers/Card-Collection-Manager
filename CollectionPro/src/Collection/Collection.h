#pragma once
#include "CollectionSource.h"
#include "CollectionDetails.h"
#include "CollectionTracker.h"
#include "CollectionQueryHelper.h"

using namespace std;

class CollectionFactory;
class CollectionItem;

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
   bool IsLoaded();

   void AddItem( string aszName,
                 vector<Tag> alstAttrs,
                 vector<Tag> alstMetaTags );

   void RemoveItem( string aszName,
                    string aszUID );

   void ChangeItem( string aszName,
                    string aszUID,
                    vector<Tag> alstIdChanges,
                    vector<Tag> alstMetaChanges );

   void ReplaceItem( string aszName,
                     string aszUID,
                     string aszNewName,
                     vector<Tag> alstIdChanges,
                     vector<Tag> alstMetaChanges );

   void SaveCollection();

   bool InitializeCollection();

   bool InitializeCollection( string aszFileName );

   void LoadCollection( const string& aszFileName,
                        CollectionFactory* aoFactory );

   void LoadChanges(vector<string> aszLines);

   vector<string> QueryCollection( Query aiQueryParms );

   vector<int> GetCollectionItems();

private:
   friend class CollectionIO;
   friend class CollectionQueryHelper;
   friend class CollectionTracker;

   CollectionDetails* m_ptrCollectionDetails;
   CollectionSource* m_ptrCollectionSource;
   CollectionTracker* m_ptrCollectionTracker;
   CollectionQueryHelper* m_ptrCollectionQueryHelper;

   // These all locate by name and hash for a second 
   // time so we dont risk dangling references.
   void addItem( const string& aszName,
                 const vector<Tag>& alstAttrs,
                 const vector<Tag>& alstMetaTags);

   void removeItem( const string& aszName,
                    const string& aszUID );

   void changeItem( const string& aszName,
                    const string& aszUID,
                    const vector<Tag>& alstChanges,
                    const vector<Tag>& alstMetaChanges );

   void replaceItem( const string& aszName,
                     const string& aszUID,
                     const string& aszNewName,
                     const vector<Tag>& alstChanges,
                     const vector<Tag>& alstMetaChanges );

   void modifyItem( CopyItem* aptCopy,
                    const vector<Tag>& alstChanges,
                    const vector<Tag>& alstMetaChanges );

   void loadInterfaceLine(const string& aszLine);

   void loadAdditionLine(const string& aszLine);
   void loadRemoveLine(const string& aszLine);
   void loadDeltaLine(const string& aszLine);

private:
   void saveHistory();
   void saveMeta();
   void saveOverhead();
   void saveCollection();
};


