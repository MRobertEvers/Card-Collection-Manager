#include "Collection.h"

#include <ctime>
#include <iomanip>
#include <algorithm>

#include "../Support/ListHelper.h"
#include "../Support/StringHelper.h"
#include "../Addressing/Addresser.h"
#include "../Config.h"
#include "AddAction.h"
#include "RemoveAction.h"
#include "ChangeAction.h"
#include "ReplaceAction.h"
#include "AddFromAction.h"
#include "CollectionIO.h"
#include "../StringInterface.h"

using namespace std;

// An ID will be given to the collection if there is a parent.
Collection::Collection( string aszName,
   CollectionSource* aoSource,
   string aszID )
{
   m_ptrCollectionTracker = new CollectionTracker( this );
   m_ptrCollectionDetails = new CollectionDetails();
   m_ptrTransactionManager = new TransactionManager( this );
   m_ptrCollectionQueryHelper = new CollectionQueryHelper( this );

   m_ptrCollectionDetails->SetName( aszName );
   m_ptrCollectionDetails->SetFileName( aszName, true );
   m_ptrCollectionDetails->SetChildrenCount( 0 );
   m_ptrCollectionDetails->AssignAddress( aszID );

   m_ptrCollectionSource = aoSource;

   m_ptrCollectionTracker->Track();
}


Collection::~Collection()
{
   delete m_ptrCollectionTracker;
   delete m_ptrCollectionDetails;
   delete m_ptrTransactionManager;
   delete m_ptrCollectionQueryHelper;
}

string Collection::GetName()
{
   return m_ptrCollectionDetails->GetName();
}

Location Collection::GetIdentifier()
{

   return Location( *m_ptrCollectionDetails->GetAddress() );
}

unsigned int
Collection::GetChildCount()
{
   return m_ptrCollectionDetails->GetChildrenCount();
}

void
Collection::ChildAdded()
{
   m_ptrCollectionDetails->IncrementChildCount();
}

void
Collection::AddItem( string aszName,
   vector<Tag> alstAttrs,
   vector<Tag> alstMetaTags,
   bool abCloseTransaction )
{
   AddAction addAction;
   addAction.SetIDs( alstAttrs );
   addAction.SetMeta( alstMetaTags );
   addAction.SetName( aszName );

   m_ptrTransactionManager->IncludeAction( addAction );

   m_ptrTransactionManager->FinalizeTransaction( abCloseTransaction );
}

void
Collection::AddItemFrom( string aszName,
   string aszIdentifyingHash,
   const Location& aAddress,
   bool abCloseTransaction )
{
   AddFromAction afAction;
   afAction.SetHash( aszIdentifyingHash );
   afAction.SetName( aszName );
   //   afAction.SetResi(aAddress);

   m_ptrTransactionManager->IncludeAction( afAction );

   m_ptrTransactionManager->FinalizeTransaction( abCloseTransaction );
}

void
Collection::RemoveItem( string aszName,
   string aszUID,
   bool abCloseTransaction )
{
   RemoveAction rmAction;
   rmAction.SetUID( aszUID );
   rmAction.SetName( aszName );

   m_ptrTransactionManager->IncludeAction( rmAction );

   m_ptrTransactionManager->FinalizeTransaction( abCloseTransaction );
}

void
Collection::ChangeItem( string aszName,
   string aszIdentifyingHash,
   vector<Tag> alstChanges,
   vector<Tag> alstMetaChanges,
   bool abCloseTransaction )
{
   ChangeAction chAction;
   chAction.SetIDs( alstChanges );
   chAction.SetMeta( alstMetaChanges );
   chAction.SetUID( aszIdentifyingHash );
   chAction.SetName( aszName );

   m_ptrTransactionManager->IncludeAction( chAction );

   m_ptrTransactionManager->FinalizeTransaction( abCloseTransaction );
}

void
Collection::ReplaceItem( string aszName,
   string aszIdentifyingHash,
   string aszNewName,
   vector<Tag> alstIdChanges,
   vector<Tag> alstMetaChanges,
   bool abCloseTransaction )
{
   ReplaceAction rpAction;
   rpAction.SetIDs( alstIdChanges );
   rpAction.SetMeta( alstMetaChanges );
   rpAction.SetNewCard( aszNewName );
   rpAction.SetUID( aszIdentifyingHash );
   rpAction.SetName( aszName );

   m_ptrTransactionManager->IncludeAction( rpAction );

   m_ptrTransactionManager->FinalizeTransaction( abCloseTransaction );
}

vector<string>
Collection::GetMetaData()
{
   vector<string> lstRetval;
   lstRetval.push_back( "Name=\"" + GetName() + "\"" );

   lstRetval.push_back( "ID=\"" + GetIdentifier().GetFullAddress() + "\"" );

   // For Each tagged item, produces "LongName : MetaData"
   for( auto metaData : m_lstTaggedItems )
   {
      string szMetaData = metaData.first + ": {" +
         metaData.second.first + "=\"" +
         metaData.second.second + "\" }";
      lstRetval.push_back( szMetaData );
   }

   return lstRetval;
}

void
Collection::SaveCollection()
{
   m_ptrCollectionDetails->SetTimeStamp();

   saveOverhead();

   saveHistory();

   saveMeta();

   saveCollection();
}

bool
Collection::InitializeCollection()
{
   m_ptrCollectionDetails->SetInitialized( true );
   return true;
}


bool 
Collection::InitializeCollection( string aszFileName )
{
   CollectionIO loader( this );
   return loader.InitializeCollection( aszFileName );
}

void
Collection::LoadCollection( const string& aszFileName,
                            CollectionFactory* aoFactory )
{

   CollectionIO loader(this);
   loader.LoadCollection( aszFileName, aoFactory );
   
   // TODO: Put the below in the loader.
   IsLoaded = (GetIdentifier().GetMain() != "");

   if( IsLoaded )
   {
      m_ptrCollectionTracker->Track();

      if( GetName() == "" )
      {
         m_ptrCollectionDetails->SetName( aszFileName );
      }
   }
}

// Returns all the copies impacted by this function.
void
Collection::LoadChanges( vector<string> lstLines )
{
   vector<string>::iterator iter_Lines = lstLines.begin();
   for( ; iter_Lines != lstLines.end(); ++iter_Lines )
   {
      loadInterfaceLine( *iter_Lines );
   }
}

vector<string>
Collection::QueryCollection( Query aiQueryParms )
{
   return m_ptrCollectionQueryHelper->QueryCollection( aiQueryParms );
}

vector<int>
Collection::getCollection()
{
   if( m_ptrCollectionSource->IsSyncNeeded( GetIdentifier() ) )
   {
      m_lstItemCacheIndexes = m_ptrCollectionSource->
         GetCollectionCache( GetIdentifier() );

      m_ptrTransactionManager->TransactionsAsynced();
   }

   return m_lstItemCacheIndexes;
}

CopyItem*
Collection::addItem(
   const string& aszName,
   const vector<Tag>& alstAttrs,
   const vector<Tag>& alstMetaTags )
{
   CopyItem* cItem;
   int iCache = m_ptrCollectionSource->LoadCard( aszName );
   if( iCache == -1 )
   {
      // TODO Could Not Load.
      return nullptr;
   }

   auto item = m_ptrCollectionSource->GetCardPrototype( aszName );

   cItem = item->AddCopy( GetIdentifier(), alstAttrs, alstMetaTags ).get();

   registerItem( iCache );

   // Notify other collections they may need to sync since this may have
   //  been borrowed by other collections.
   // There should never be other copies with that hash not in resident. 
   // They are removed at load time.
   m_ptrCollectionSource->NotifyNeedToSync( GetIdentifier() );

   return cItem;
}

void
Collection::addItemFrom(
   const string& aszName,
   const string& aszIdentifyingHash,
   const Identifier& aResiAddress )
{
   TryGet<CollectionObject> item;

   int iCache = m_ptrCollectionSource->LoadCard( aszName );
   item = m_ptrCollectionSource->GetCardPrototype( iCache );
   if( !item.Good() ) { return; }

   auto cItem = item->FindCopy( aszIdentifyingHash );
   if( !cItem.Good() ) { return; }
   auto copy = cItem.Value()->get();

   copy->AddResident( GetIdentifier() );

   registerItem( iCache );
}

void
Collection::removeItem( const string& aszName,
   const string& aszUID )
{
   // The copy is already verified to exist at this point
   int iCache = m_ptrCollectionSource->LoadCard( aszName );

   TryGet<CollectionObject> item = m_ptrCollectionSource->GetCardPrototype( iCache );

   item->RemoveCopy( GetIdentifier(), aszUID );

   // Remove any items from the cache that are no longer in this collection.
   int iRemainingCopies = item->FindCopies( GetIdentifier(), All ).size();
   if( iRemainingCopies == 0 )
   {
      unregisterItem( iCache );
   }

   // Notify other collections they may need to sync since this may have been 
   //  borrowed by other collections.
   m_ptrCollectionSource->NotifyNeedToSync( GetIdentifier() );
}

void
Collection::changeItem( const string& aszName,
   const string& aszUID,
   const vector<Tag>& alstChanges,
   const vector<Tag>& alstMetaChanges )
{
   int iCache = m_ptrCollectionSource->LoadCard( aszName );

   TryGet<CollectionObject> item = m_ptrCollectionSource->GetCardPrototype( iCache );
   CopyItem* cItem = item->FindCopy( aszUID ).Value()->get();
   if( cItem == nullptr ) { return; }

   modifyItem( cItem, alstChanges, alstMetaChanges );

   // Notify other collections they may need to sync since this may have been
   //  borrowed by other collections.
   m_ptrCollectionSource->NotifyNeedToSync( GetIdentifier() );
}


void
Collection::replaceItem( const string& aszName,
   const string& aszUID,
   const string& aszNewName,
   const vector<Tag>& alstIdChanges,
   const vector<Tag>& alstMetaChanges )
{
   int iCache = m_ptrCollectionSource->LoadCard( aszName );
   int iNewCache = m_ptrCollectionSource->LoadCard( aszNewName );

   TryGet<CollectionObject> item = m_ptrCollectionSource->GetCardPrototype( iCache );
   TryGet<CollectionObject> newItem = m_ptrCollectionSource->GetCardPrototype( iNewCache );
   auto cItem = item->FindCopy( aszUID );
   if( !cItem.Good() ) { return; }

   removeItem( item->GetName(), aszUID );
   addItem( newItem->GetName(), alstIdChanges, alstMetaChanges );
}

void
Collection::registerItem( int aiCacheIndex )
{
   int iFound = ListHelper::List_Find( aiCacheIndex, m_lstItemCacheIndexes );

   if( iFound == -1 )
   {
      m_lstItemCacheIndexes.push_back( aiCacheIndex );
   }
}

void
Collection::unregisterItem( int aiCacheIndex )
{
   vector<int> lstNewCacheIndexes;
   for( auto iCache : m_lstItemCacheIndexes )
   {
      if( iCache != aiCacheIndex )
      {
         lstNewCacheIndexes.push_back( iCache );
      }
   }

   m_lstItemCacheIndexes = lstNewCacheIndexes;
}

void
Collection::modifyItem( CopyItem* aptCopy,
                        const vector<Tag>& alstChanges,
                        const vector<Tag>& alstMetaChanges )
{
   for( size_t i = 0; i < alstChanges.size(); i++ )
   {
      aptCopy->
         SetIdentifyingAttribute( alstChanges[i].first, alstChanges[i].second );
   }

   for( size_t i = 0; i < alstMetaChanges.size(); i++ )
   {
      MetaTagType mTagType = CopyItem::DetermineMetaTagType( alstMetaChanges[i].first );
      aptCopy->
         SetMetaTag( alstMetaChanges[i].first, alstMetaChanges[i].second, mTagType );
   }
}

// This should only be called during initial loading.
void Collection::loadMetaTagFile()
{
   string szFileName;
   string szPlainHash;
   vector<string> lstMetaLines;
   TryGet<CollectionObject> item;

   szFileName = CollectionIO::GetMetaFile( m_ptrCollectionDetails->GetFileName() );
   CollectionIO::GetFileLines( szFileName, lstMetaLines );

   for( size_t i = 0; i < lstMetaLines.size(); i++ )
   {
      CollectionObject::PseudoIdentifier sudoItem;
      CollectionObject::ParseCardLine( lstMetaLines[i], sudoItem );

      vector<Tag> lstMetaTags = sudoItem.MetaTags;

      // Clear the meta so the hash may be obtained.
      sudoItem.MetaString = "";
      sudoItem.MetaTags.clear();

      int iRealCard = m_ptrCollectionSource->LoadCard( sudoItem.Name );
      if( iRealCard == -1 ) { continue; }

      item = m_ptrCollectionSource->GetCardPrototype( iRealCard );

      szPlainHash = item->GenerateHash( GetIdentifier(),
         sudoItem.Identifiers,
         sudoItem.MetaTags );

      // Gets the first matching item resident in this collection.
      auto matchingCopy = item->FindCopy( szPlainHash, Hash );
      if( matchingCopy.Good() )
      {
         MetaTagType mTagType;
         auto copy = matchingCopy.Value()->get();
         for( size_t t = 0; t < lstMetaTags.size(); t++ )
         {
            mTagType = CopyItem::DetermineMetaTagType( lstMetaTags[t].first );
            copy->SetMetaTag( lstMetaTags[t].first,
               lstMetaTags[t].second,
               mTagType, false );
         }
      }
   }
}

// Returns true if the line could be processed by the collections
// Only returns true for data lines.
bool Collection::loadOverheadLine( const string& aszLine )
{
   string szDefKey( Config::CollectionDefinitionKey );
   if( aszLine.size() < 2 ) { return true; }
   if( aszLine.substr( 0, szDefKey.size() ) == szDefKey )
   {
      // The line is a data line.
      loadCollectionDataLine( aszLine );
      return true;
   }
   else
   {
      return false;
   }
}

// Expects the input to be of the form : Key = "Value"
void
Collection::loadCollectionDataLine( const string& aszData )
{
   string szBaseLine = aszData.substr( 2 );
   vector<string> lstSplitLine = StringHelper::Str_Split( szBaseLine, "=" );

   if( lstSplitLine.size() != 2 ) { return; }

   vector<string>::iterator iter_Lines = lstSplitLine.begin();
   for( ; iter_Lines != lstSplitLine.end(); ++iter_Lines )
   {
      *iter_Lines = StringHelper::Str_Trim( *iter_Lines, ' ' );
   }

   string szKey = lstSplitLine.at( 0 );
   string szValue = lstSplitLine.at( 1 );
   szValue = StringHelper::Str_Trim( szValue, '\"' );

   if( szKey == "Name" )
   {
      m_ptrCollectionDetails->SetName( szValue );
   }
   else if( szKey == "ID" )
   {
      m_ptrCollectionDetails->AssignAddress( szValue );
   }
   else if( szKey == "CC" )
   {
      m_ptrCollectionDetails->SetChildrenCount( stoi( szValue ) );
   }
   else if( szKey == "Session" )
   {
      tm tm{};
      istringstream str_stream( szValue );
      str_stream >> get_time( &tm, "%Y-%m-%d_%T" );
      time_t time = mktime( &tm );
      m_ptrCollectionDetails->SetTimeStamp( time );
   }
}

// May return null depending on input
void
Collection::loadInterfaceLine( const string& aszLine )
{
   if( aszLine.size() <= 2 ) { return; }

   string szTrimmedLine = StringHelper::Str_Trim( aszLine, ' ' );

   string szLoadDirective = szTrimmedLine.substr( 0, 1 );

   if( szLoadDirective == "-" ) // REMOVE
   {
      szTrimmedLine = szTrimmedLine.substr( 1 );
      // Of the form ([] meaning optional)
      // Sylvan Card Name [{ set="val" color="val2" } ][: { metatag1="val" metatag2="val2" }]
      loadRemoveLine( szTrimmedLine );
   }
   else if( szLoadDirective == "%" ) // CHANGE
   {
      szTrimmedLine = szTrimmedLine.substr( 1 );
      // Of the form
      // Sylvan Card Name [{ set="val" color="val2" } ][: { metatag1="val" metatag2="val2" }] ->
      //   Another Card Name [{ set="val" color="val2" } ][: { metatag1="val" metatag2="val2" }]
      loadDeltaLine( szTrimmedLine );
   }
   else // ADD
   {
      if( szLoadDirective == "+" )
      {
         szTrimmedLine = szTrimmedLine.substr( 1 );
      }
      // Of the form
      // Sylvan Card Name [{ set="val" color="val2" } ][: { metatag1="val" metatag2="val2" }]
      loadAdditionLine( szTrimmedLine );
   }
}

void Collection::loadAdditionLine( const string& aszLine )
{
   string szID = "";
   string szLine = aszLine;
   Address aParentAddress;
   CollectionObject::PseudoIdentifier sudoItem;
   bool bThisIsParent = true;

   // Convert the line to the official form if needed.
   m_ptrCollectionSource->ExpandAdditionLine( szLine );

   CollectionObject::ParseCardLine( szLine, sudoItem );

   int iFoundAddress = ListHelper::List_Find( string( "Parent" ),
      sudoItem.MetaTags,
      Config::Instance()->GetTagHelper() );
   if( iFoundAddress != -1 )
   {
      aParentAddress = Address( sudoItem.MetaTags.at( iFoundAddress ).second );
      int iFoundHash = ListHelper::List_Find( string( Config::HashKey ),
         sudoItem.MetaTags,
         Config::Instance()->GetTagHelper() );
      szID = sudoItem.MetaTags.at( iFoundHash ).second;
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
         AddItem( sudoItem.Name, sudoItem.Identifiers, sudoItem.MetaTags );
      }
   }
}

// This needs "Card Name : { __hash="hashval" }" All other values are irrelevant.
void Collection::loadRemoveLine( const string& aszLine )
{
   CollectionObject::PseudoIdentifier sudoItem;
   CollectionObject::ParseCardLine( aszLine, sudoItem );

   for( size_t i = 0; i < sudoItem.Count; i++ )
   {
      string szUID;
      int iUID = ListHelper::List_Find( CopyItem::GetUIDKey(),
         sudoItem.MetaTags,
         Config::Instance()->GetTagHelper() );
      if( iUID != -1 )
      {
         szUID = sudoItem.MetaTags[iUID].second;
         RemoveItem( sudoItem.Name, szUID );
      }
      else { break; }
   }
}
void Collection::loadDeltaLine( const string& aszLine )
{
   vector<string> lstOldNew = StringHelper::Str_Split( aszLine, "->" );

   CollectionObject::PseudoIdentifier sudoOldItem;
   CollectionObject::ParseCardLine( lstOldNew[0], sudoOldItem );

   CollectionObject::PseudoIdentifier sudoNewItem;
   CollectionObject::ParseCardLine( lstOldNew[1], sudoNewItem );

   int iCache;
   int iUID = ListHelper::List_Find( CopyItem::GetUIDKey(),
      sudoOldItem.MetaTags,
      Config::Instance()->GetTagHelper() );
   if( (iUID != -1) &&
      (iCache = m_ptrCollectionSource->LoadCard( sudoOldItem.Name )) != -1 )
   {
      string szUID; TryGet<CollectionObject> itemOld; CopyItem* cItem;

      szUID = sudoOldItem.MetaTags[iUID].second;
      itemOld = m_ptrCollectionSource->GetCardPrototype( iCache );
      cItem = itemOld->FindCopy( szUID )->get();

      for( size_t i = 0; i < sudoOldItem.Count; i++ )
      {
         int iNewCache;
         if( sudoOldItem.Name == sudoNewItem.Name )
         {
            ChangeItem( sudoOldItem.Name,
               szUID,
               sudoNewItem.Identifiers,
               sudoNewItem.MetaTags );
         }
         else if( (iNewCache = m_ptrCollectionSource->LoadCard( sudoNewItem.Name )) != -1 )
         {
            TryGet<CollectionObject> itemNew = m_ptrCollectionSource->
               GetCardPrototype( iNewCache );
            ReplaceItem( sudoOldItem.Name,
               szUID,
               sudoNewItem.Name,
               sudoNewItem.Identifiers,
               sudoNewItem.MetaTags );
         }
      }

   }
}

void
Collection::saveHistory()
{
   m_ptrCollectionTracker->Track();
   CollectionDeltaClass cdc = m_ptrCollectionTracker->CalculateChanges();

   vector<string> lstHistoryLines;
   for( size_t i = 0; i < cdc.Additions.size(); i++ )
   {
      lstHistoryLines.push_back( cdc.Additions[i] );
   }

   for( size_t i = 0; i < cdc.Removals.size(); i++ )
   {
      lstHistoryLines.push_back( cdc.Removals[i] );
   }

   for( size_t i = 0; i < cdc.Changes.size(); i++ )
   {
      lstHistoryLines.push_back( cdc.Changes[i] );
   }

   if( lstHistoryLines.size() > 0 )
   {
      string szTimeString = "";
      time_t now = time( 0 );
      struct tm timeinfo;
      localtime_s( &timeinfo, &now );
      char str[26];
      asctime_s( str, sizeof str, &timeinfo );
      str[strlen( str ) - 1] = 0;

      ofstream oHistFile;
      string szHistFile = CollectionIO::GetHistoryFile( m_ptrCollectionDetails->GetFileName() );
      oHistFile.open( szHistFile, ios_base::app );

      oHistFile << "[" << str << "] " << endl;

      vector<string>::iterator iter_histLines = lstHistoryLines.begin();
      for( ; iter_histLines != lstHistoryLines.end(); ++iter_histLines )
      {
         oHistFile << *iter_histLines << endl;
      }

      oHistFile.close();
   }
}

void Collection::saveMeta()
{
   ofstream oMetaFile;
   Query listQuery(true);
   listQuery.IncludeMetaType( Persistent );
   vector<string> lstMetaLines = QueryCollection( listQuery );

   oMetaFile.open( CollectionIO::GetMetaFile( m_ptrCollectionDetails->GetFileName() ) );

   vector<string>::iterator iter_MetaLine = lstMetaLines.begin();
   for( ; iter_MetaLine != lstMetaLines.end(); ++iter_MetaLine )
   {
      if( iter_MetaLine->find_first_of( ':' ) != string::npos )
      {
         oMetaFile << *iter_MetaLine << endl;
      }
   }

   oMetaFile.close();
}

void
Collection::saveOverhead()
{
   ofstream oColFile;
   oColFile.open( CollectionIO::GetOverheadFile( m_ptrCollectionDetails->GetFileName() ) );

   tm otm;
   time_t time = m_ptrCollectionDetails->GetTimeStamp();
   localtime_s( &otm, &time );

   oColFile << Config::CollectionDefinitionKey
      << " ID=\"" << GetIdentifier().GetFullAddress() << "\"" << endl;

   oColFile << Config::CollectionDefinitionKey
      << " CC=\"" << m_ptrCollectionDetails->GetChildrenCount() << "\"" << endl;

   oColFile << Config::CollectionDefinitionKey
      << " Session=\"" << put_time( &otm, "%F_%T" ) << "\"" << endl;

   for( auto szLine : m_ptrCollectionDetails->GetProcessLines() )
   {
      oColFile << szLine << endl;
   }

   oColFile.close();
}

void Collection::saveCollection()
{
   // Group lists only by id. When loading, these lists are only
   // used to create a template card. We only need the base details.
   Query listQuery(true);
   listQuery.IncludeMetaType( None );
   listQuery.HashType( CopyItem::HashType::Ids );
   vector<string> lstLines = QueryCollection( listQuery );

   // Convert the lines to shorthand
   for( auto& szLine : lstLines )
   {
      m_ptrCollectionSource->CollapseCardLine( szLine );
   }

   ofstream oColFile;
   oColFile.open( m_ptrCollectionDetails->GetFile() );

   oColFile << "\"" << m_ptrCollectionDetails->GetName() << "\"" << endl;

   vector<string>::iterator iter_Line = lstLines.begin();
   for( ; iter_Line != lstLines.end(); ++iter_Line )
   {
      oColFile << *iter_Line << endl;
   }

   oColFile.close();
}
