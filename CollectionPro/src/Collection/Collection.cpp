#include "Collection.h"
#include "../Support/StringHelper.h"
#include "../Addressing/Addresser.h"
#include "../Config.h"
#include "CollectionIO.h"
#include "../StringInterface.h"
#include "CollectionLedger.h"

#include <iomanip>
#include <ctime>
#include <algorithm>

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

   m_ptrLedger = new CollectionLedger(this->GetIdentifier());

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
   for( auto& szLine : lstLines )
   {
      loadInterfaceLine( szLine );
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
Collection::InvalidateState()
{
   // For any component that may cache data for speed,
   // this tells it that it needs to refresh.
   m_ptrCollectionQueryHelper->InvalidateState();
}

void 
Collection::addItem( const string& aszName,
                     const vector<Tag>& alstAttrs,
                     const vector<Tag>& alstMetaTags )
{
   auto item = m_ptrCollectionSource->GetCardPrototype( aszName );

   item->AddCopy( GetIdentifier(), alstAttrs, alstMetaTags ).get();
   InvalidateState();
}

bool
Collection::removeItem( const string& aszName,
                        const string& aszUID )
{
   auto item = m_ptrCollectionSource->GetCardPrototype( aszName );
   if( !item.Good() )
   {
      return false;
   }

   bool bSuccess = item->RemoveCopy( GetIdentifier(), aszUID );
   if( bSuccess )
   {
      InvalidateState();
   }
   return bSuccess;
}

bool
Collection::changeItem( const string& aszName,
                        const string& aszUID,
                        const vector<Tag>& alstChanges,
                        const vector<Tag>& alstMetaChanges )
{
   auto item = m_ptrCollectionSource->GetCardPrototype( aszName );
   auto cItem = item->FindCopy( aszUID );
   if( cItem.Good() ) 
   { 
      item->SetIdentifyingTraits( *cItem.Value(), alstChanges, alstMetaChanges );
      InvalidateState();
      return true;
   }

   return false;
}


bool
Collection::replaceItem( const string& aszName,
                         const string& aszUID,
                         const string& aszNewName,
                         const vector<Tag>& alstIdChanges,
                         const vector<Tag>& alstMetaChanges )
{
   auto item = m_ptrCollectionSource->GetCardPrototype( aszName );
   auto newItem = m_ptrCollectionSource->GetCardPrototype( aszNewName );

   if( removeItem( item->GetName(), aszUID ) )
   {
      addItem( newItem->GetName(), alstIdChanges, alstMetaChanges );
      InvalidateState();
      return true;
   }

   return false;
}

void
Collection::loadInterfaceLine( const string& aszLine )
{
   string szLine = aszLine;
   auto iLineType = StringInterface::ParseInterfaceLine( szLine );

   if( iLineType == StringInterface::AddLine )
   {
      loadAdditionLine( szLine );
   }
   else if( iLineType == StringInterface::RemoveLine )
   {
      loadRemoveLine( szLine );
   }
   else if( iLineType == StringInterface::ChangeLine )
   {
      loadDeltaLine( szLine );
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

   auto szAddr = StringInterface::FindTagInList( sudoItem.MetaTags, CopyItem::GetAddressKey() );
   if( szAddr != "" )
   {
      aParentAddress = Address( szAddr );
      szID = StringInterface::FindTagInList( sudoItem.MetaTags, CopyItem::GetHashKey() );
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
   CollectionObject::PseudoIdentifier sudoItem;
   CollectionObject::ParseCardLine( aszLine, sudoItem );

   for( size_t i = 0; i < sudoItem.Count; i++ )
   {
      string szUID = StringInterface::FindTagInList( sudoItem.MetaTags, CopyItem::GetUIDKey() );;
      if( szUID != "" )
      {
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

   auto oldItem = m_ptrCollectionSource->GetCardPrototype( sudoOldItem.Name );
   if( oldItem.Good() )
   {
      string szUID  = StringInterface::FindTagInList( sudoOldItem.MetaTags, CopyItem::GetUIDKey() );
      auto cItem = oldItem->FindCopy( szUID );
      if( cItem.Good() )
      {
         for( size_t i = 0; i < sudoOldItem.Count; i++ )
         {
            if( sudoOldItem.Name == sudoNewItem.Name )
            {
               ChangeItem( sudoOldItem.Name,
                           szUID,
                           sudoNewItem.Identifiers,
                           sudoNewItem.MetaTags );
            }
            else
            {
               auto newItem = m_ptrCollectionSource->GetCardPrototype( sudoNewItem.Name );
               if( newItem.Good() )
               {
                  ReplaceItem( sudoOldItem.Name,
                               szUID,
                               sudoNewItem.Name,
                               sudoNewItem.Identifiers,
                               sudoNewItem.MetaTags );
               }
            }
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
