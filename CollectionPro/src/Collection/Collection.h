#pragma once
#include "CollectionSource.h"
#include "CollectionDetails.h"
#include "CollectionTracker.h"
#include "CollectionQueryHelper.h"
#include <memory>

using namespace std;

class CollectionLedger;
class CollectionFactory;
class CollectionObject;

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
   vector<Tag> GetPeekValues();
   map<unsigned long, vector<string>> GetHistoryLines( unsigned int aiStart, 
                                                       unsigned int aiEnd = 0 );

   string AddItem( string aszName,
                   vector<Tag> alstAttrs,
                   vector<Tag> alstMetaTags );

   string RemoveItem( string aszName,
                      string aszUID );

   Tag ChangeItem( string aszName,
                   string aszUID,
                   vector<Tag> alstIdChanges,
                   vector<Tag> alstMetaChanges );

   Tag ReplaceItem( string aszName,
                    string aszUID,
                    string aszNewName,
                    vector<Tag> alstIdChanges,
                    vector<Tag> alstMetaChanges );

   void SaveCollection();

   void ExportCollection( Query aQuery );

   bool InitializeCollection();

   bool InitializeCollection( string aszFileName );

   void LoadCollection( const string& aszFileName,
                        CollectionFactory* aoFactory );

   vector<string> LoadChanges(vector<string> aszLines);

   vector<string> QueryCollection( Query aiQueryParms );

   map<CollectionObject*, set<shared_ptr<CopyItem>>>  GetCollectionItems();

   void InvalidateState();

private:
   friend class CollectionIO;
   friend class CollectionQueryHelper;
   friend class CollectionTracker;

   CollectionLedger* m_ptrCollectionLedger;
   CollectionDetails* m_ptrCollectionDetails;
   CollectionSource* m_ptrCollectionSource;
   CollectionTracker* m_ptrCollectionTracker;
   CollectionQueryHelper* m_ptrCollectionQueryHelper;

   // These all locate by name and hash for a second 
   // time so we dont risk dangling references.
   string addItem( const string& aszName,
                   const vector<Tag>& alstAttrs,
                   const vector<Tag>& alstMetaTags);

   // Returns removed UID
   string removeItem( const string& aszName,
                      const string& aszUID );

   // Returns UID before and after
   Tag changeItem( const string& aszName,
                   const string& aszUID,
                   const vector<Tag>& alstChanges,
                   const vector<Tag>& alstMetaChanges );

   // Returns UID before and after
   Tag replaceItem( const string& aszName,
                    const string& aszUID,
                    const string& aszNewName,
                    const vector<Tag>& alstChanges,
                    const vector<Tag>& alstMetaChanges );

   vector<string> loadInterfaceLine(const string& aszLine);

   vector<string> loadAdditionLine(const string& aszLine);
   vector<string> loadRemoveLine(const string& aszLine);
   vector<Tag> loadDeltaLine(const string& aszLine);
};


