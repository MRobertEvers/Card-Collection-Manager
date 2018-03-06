#include "Collection.h"

#include <iomanip>
#include <ctime>
#include <algorithm>

#include "../Support/StringHelper.h"
#include "../Addressing/Addresser.h"
#include "../Config.h"
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

bool 
Collection::IsLoaded()
{
   return m_ptrCollectionDetails->IsLoaded();
}

void
Collection::AddItem( string aszName,
                     vector<Tag> alstAttrs,
                     vector<Tag> alstMetaTags )
{
   addItem( aszName, alstAttrs, alstMetaTags );
}

void
Collection::RemoveItem( string aszName,
                        string aszUID )
{
   removeItem( aszName, aszUID );
}

void
Collection::ChangeItem( string aszName,
                        string aszUID,
                        vector<Tag> alstChanges,
                        vector<Tag> alstMetaChanges )
{
   changeItem( aszName, aszUID, alstChanges, alstMetaChanges );
}

void
Collection::ReplaceItem( string aszName,
                         string aszIdentifyingHash,
                         string aszNewName,
                         vector<Tag> alstIdChanges,
                         vector<Tag> alstMetaChanges )
{
   replaceItem( aszName, aszIdentifyingHash, aszNewName, alstIdChanges, alstMetaChanges );
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
Collection::GetCollectionItems()
{
   return m_ptrCollectionSource->GetCollectionCache( GetIdentifier(), All );
}

void 
Collection::addItem( const string& aszName,
                     const vector<Tag>& alstAttrs,
                     const vector<Tag>& alstMetaTags )
{
   auto item = m_ptrCollectionSource->GetCardPrototype( aszName );

   item->AddCopy( GetIdentifier(), alstAttrs, alstMetaTags ).get();
}

void
Collection::removeItem( const string& aszName,
                        const string& aszUID )
{

   auto item = m_ptrCollectionSource->GetCardPrototype( aszName );

   item->RemoveCopy( GetIdentifier(), aszUID );
}

void
Collection::changeItem( const string& aszName,
                        const string& aszUID,
                        const vector<Tag>& alstChanges,
                        const vector<Tag>& alstMetaChanges )
{
   auto item = m_ptrCollectionSource->GetCardPrototype( aszName );
   auto cItem = item->FindCopy( aszUID );
   if( cItem.Good() ) 
   { 
      modifyItem( cItem.Value()->get(), alstChanges, alstMetaChanges );
   }
}


void
Collection::replaceItem( const string& aszName,
                         const string& aszUID,
                         const string& aszNewName,
                         const vector<Tag>& alstIdChanges,
                         const vector<Tag>& alstMetaChanges )
{
   auto item = m_ptrCollectionSource->GetCardPrototype( aszName );
   auto newItem = m_ptrCollectionSource->GetCardPrototype( aszNewName );
   auto cItem = item->FindCopy( aszUID );
   if( cItem.Good() ) 
   { 
      removeItem( item->GetName(), aszUID );
      addItem( newItem->GetName(), alstIdChanges, alstMetaChanges );
   }
}

void
Collection::modifyItem( CopyItem* aptCopy,
                        const vector<Tag>& alstChanges,
                        const vector<Tag>& alstMetaChanges )
{
   for( size_t i = 0; i < alstChanges.size(); i++ )
   {
      aptCopy->SetIdentifyingAttribute( alstChanges[i].first, alstChanges[i].second );
   }

   for( size_t i = 0; i < alstMetaChanges.size(); i++ )
   {
      MetaTagType mTagType = CopyItem::DetermineMetaTagType( alstMetaChanges[i].first );
      aptCopy->SetMetaTag( alstMetaChanges[i].first, alstMetaChanges[i].second, mTagType );
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
   StringInterface stringIFace;
   bool bThisIsParent = true;

   // Convert the line to the official form if needed.
   m_ptrCollectionSource->ExpandAdditionLine( szLine );

   CollectionObject::ParseCardLine( szLine, sudoItem );

   auto szAddr = stringIFace.FindTagInList( sudoItem.MetaTags, CopyItem::GetAddressKey() );
   if( szAddr != "" )
   {
      aParentAddress = Address( szAddr );
      szID = stringIFace.FindTagInList( sudoItem.MetaTags, CopyItem::GetHashKey() );
      bThisIsParent = !(aParentAddress == GetIdentifier());
   }

   // If the ID is specified, then we assume the card already exists.
   if( (!bThisIsParent) && // This is not the parent
       (szID != "") )       // and the id was specified.
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
   StringInterface stringIFace;
   CollectionObject::PseudoIdentifier sudoItem;
   CollectionObject::ParseCardLine( aszLine, sudoItem );

   for( size_t i = 0; i < sudoItem.Count; i++ )
   {
      string szUID = stringIFace.FindTagInList( sudoItem.MetaTags, CopyItem::GetUIDKey() );;
      if( szUID != "" )
      {
         RemoveItem( sudoItem.Name, szUID );
      }
      else { break; }
   }
}
void Collection::loadDeltaLine( const string& aszLine )
{
   StringInterface stringIFace;
   vector<string> lstOldNew = StringHelper::Str_Split( aszLine, "->" );

   CollectionObject::PseudoIdentifier sudoOldItem;
   CollectionObject::ParseCardLine( lstOldNew[0], sudoOldItem );

   CollectionObject::PseudoIdentifier sudoNewItem;
   CollectionObject::ParseCardLine( lstOldNew[1], sudoNewItem );

   int iCache;
   string szUID  = stringIFace.FindTagInList( sudoOldItem.MetaTags, CopyItem::GetUIDKey() );;
   if( (szUID != "") &&
       ((iCache = m_ptrCollectionSource->LoadCard( sudoOldItem.Name )) != -1) )
   {
      TryGet<CollectionObject> itemOld; CopyItem* cItem;

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
