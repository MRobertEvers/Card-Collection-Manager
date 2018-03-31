#include "Collection.h"
#include "../Support/StringHelper.h"
#include "../Addressing/Addresser.h"
#include "../Config.h"
#include "../StringInterface.h"
#include "CollectionIO.h"
#include "CollectionLedger.h"


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

   m_ptrCollectionLedger = new CollectionLedger(this->GetIdentifier());

   m_ptrCollectionSource = aoSource;

   m_ptrCollectionTracker->Track();
}


Collection::~Collection()
{
   delete m_ptrCollectionLedger;
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

vector<Tag> 
Collection::GetPeekValues()
{
   return m_ptrCollectionDetails->GetPeekValues();
}

map<unsigned long, vector<string>>
Collection::GetHistoryLines( unsigned int aiStart, unsigned int aiEnd )
{
   CollectionIO io( this );
   return io.GetHistoryTransactions(aiStart, aiEnd);
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
   CollectionIO IO( this );
   IO.SaveCollection();
}

void 
Collection::ExportCollection( Query aQuery )
{
   CollectionIO IO( this );
   IO.ExportCollection(aQuery);
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

map<CollectionObject*, set<shared_ptr<CopyItem>>>
Collection::GetCollectionItems()
{
   return m_ptrCollectionLedger->ViewSortedPresent();
   //return m_ptrCollectionSource->GetCollectionCache( GetIdentifier(), All );
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
   if( !item.Good() )
   {
      return;
   }
   
   item->AddCopy( GetIdentifier(), alstAttrs, alstMetaTags );
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
   if( !item.Good() )
   {
      return false;
   }

   auto cItem = item->FindCopy( aszUID );
   if( !cItem.Good() ) 
   { 
      return false;
   }

   item->SetIdentifyingTraits( *cItem.Value(), alstChanges, alstMetaChanges );
   InvalidateState();
   return true;
}


bool
Collection::replaceItem( const string& aszName,
                         const string& aszUID,
                         const string& aszNewName,
                         const vector<Tag>& alstIdChanges,
                         const vector<Tag>& alstMetaChanges )
{
   auto item = m_ptrCollectionSource->GetCardPrototype( aszName );
   if( !item.Good() )
   {
      return false;
   }

   auto newItem = m_ptrCollectionSource->GetCardPrototype( aszNewName );
   if( !newItem.Good() )
   {
      return false;
   }

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

void 
Collection::loadAdditionLine( const string& aszLine )
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
void 
Collection::loadRemoveLine( const string& aszLine )
{
   CollectionObject::PseudoIdentifier sudoItem;
   CollectionObject::ParseCardLine( aszLine, sudoItem );

   string szUID = StringInterface::FindTagInList( sudoItem.MetaTags, CopyItem::GetUIDKey() );;
   for( size_t i = 0; i < sudoItem.Count; i++ )
   {
      if( szUID != "" )
      {
         RemoveItem( sudoItem.Name, szUID );
      }
      else { break; }
   }
}
void 
Collection::loadDeltaLine( const string& aszLine )
{
   vector<string> lstOldNew = StringHelper::Str_Split( aszLine, "->" );

   CollectionObject::PseudoIdentifier sudoOldItem;
   CollectionObject::ParseCardLine( lstOldNew[0], sudoOldItem );

   CollectionObject::PseudoIdentifier sudoNewItem;
   CollectionObject::ParseCardLine( lstOldNew[1], sudoNewItem );

   auto oldItem = m_ptrCollectionSource->GetCardPrototype( sudoOldItem.Name );
   if( !oldItem.Good() )
   {
      // TODO: Log this.
      return;
   }
   
   string szUID  = StringInterface::FindTagInList( sudoOldItem.MetaTags, CopyItem::GetUIDKey() );
   auto cItem = oldItem->FindCopy( szUID );
   if( !cItem.Good() )
   {
      // TODO: Error.
      return;
   }

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
