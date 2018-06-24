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

string
Collection::AddItem( string aszName,
                     vector<Tag> alstAttrs,
                     vector<Tag> alstMetaTags )
{
   return addItem( aszName, alstAttrs, alstMetaTags );
}

string
Collection::RemoveItem( string aszName,
                        string aszUID )
{
   return removeItem( aszName, aszUID );
}

Tag
Collection::ChangeItem( string aszName,
                        string aszUID,
                        vector<Tag> alstChanges,
                        vector<Tag> alstMetaChanges )
{
   return changeItem( aszName, aszUID, alstChanges, alstMetaChanges );
}

Tag
Collection::ReplaceItem( string aszName,
                         string aszIdentifyingHash,
                         string aszNewName,
                         vector<Tag> alstIdChanges,
                         vector<Tag> alstMetaChanges )
{
   return replaceItem( aszName, aszIdentifyingHash, aszNewName, alstIdChanges, alstMetaChanges );
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
vector<string>
Collection::LoadChanges( vector<string> lstLines )
{
   vector<string> vecRetVal;
   for( auto& szLine : lstLines )
   {
      auto vecRes= loadInterfaceLine( szLine );
      for( auto& item : vecRes )
      {
         vecRetVal.push_back( item );
      }
   }

   return vecRetVal;
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

// Returns UID of newly created item
string 
Collection::addItem( const string& aszName,
                     const vector<Tag>& alstAttrs,
                     const vector<Tag>& alstMetaTags )
{
   auto item = m_ptrCollectionSource->GetCardPrototype( aszName );
   if( !item.Good() )
   {
      return "";
   }
   
   // TODO:
   auto copy = item->AddCopy( GetIdentifier(), alstAttrs, alstMetaTags );
   InvalidateState();

   return copy->GetUID();
}

string
Collection::removeItem( const string& aszName,
                        const string& aszUID )
{
   auto item = m_ptrCollectionSource->GetCardPrototype( aszName );
   if( !item.Good() )
   {
      return "";
   }

   bool bSuccess = item->RemoveCopy( GetIdentifier(), aszUID );
   if( bSuccess )
   {
      InvalidateState();
      return aszUID;
   }

   return "";
}

Tag
Collection::changeItem( const string& aszName,
                        const string& aszUID,
                        const vector<Tag>& alstChanges,
                        const vector<Tag>& alstMetaChanges )
{
   auto item = m_ptrCollectionSource->GetCardPrototype( aszName );
   if( !item.Good() )
   {
      return Tag();
   }

   auto cItem = item->FindCopy( aszUID );
   if( !cItem.Good() ) 
   { 
      return Tag();
   }
   
   auto szBefore = cItem->GetUID();
   cItem.Value()->SetAttributes( alstChanges );
   
   for( auto& attr : alstMetaChanges )
   {
      cItem.Value()->SetMetaTag( attr.first, attr.second );
   }
   auto szAfter = cItem->GetUID();

   InvalidateState();
   
   return Tag(szBefore, szAfter);
}


Tag
Collection::replaceItem( const string& aszName,
                         const string& aszUID,
                         const string& aszNewName,
                         const vector<Tag>& alstIdChanges,
                         const vector<Tag>& alstMetaChanges )
{
   auto item = m_ptrCollectionSource->GetCardPrototype( aszName );
   if( !item.Good() )
   {
      return Tag();
   }

   auto newItem = m_ptrCollectionSource->GetCardPrototype( aszNewName );
   if( !newItem.Good() )
   {
      return Tag();
   }

   string szBefore = "";
   string szAfter = "";
   if( removeItem( item->GetName(), aszUID ) != "" )
   {
      szBefore = aszUID;
      szAfter = addItem( newItem->GetName(), alstIdChanges, alstMetaChanges );
      InvalidateState();

      return Tag( szBefore, szAfter );
   }

   return Tag();
}

// Returns -> { uid } for addition
// Returns { uid } -> for removal
// Returns { uid } -> { uid2 } for delta (they can be the same)
vector<string>
Collection::loadInterfaceLine( const string& aszLine )
{
   string szLine = aszLine;
   auto iLineType = StringInterface::ParseInterfaceLine( szLine );

   vector<string> vecRetval;
   if( iLineType == StringInterface::AddLine )
   {
      auto res = loadAdditionLine( szLine );
      for( auto& resLine : res )
      {
         if( resLine != "" )
         {
            vecRetval.push_back( "-> {" + resLine + "}");
         }
      }
   }
   else if( iLineType == StringInterface::RemoveLine )
   {
      auto res = loadRemoveLine( szLine );
      for( auto& resLine : res )
      {
         if( resLine != "" )
         {
            vecRetval.push_back( "{" + resLine + "} ->");
         }
      }
   }
   else if( iLineType == StringInterface::ChangeLine )
   {
      auto res = loadDeltaLine( szLine );
      vector<string> vecVals;
      for( auto& resLine : res )
      {
         if( resLine != Tag() )
         {
            vecRetval.push_back( string( "{ " + resLine.first + "} -> {" + resLine.second + "}" ) );
         }
      }
   }

   return vecRetval;
}

vector<string>
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

   auto szAddr = StringInterface::FindTagInList( sudoItem.MetaTags, MetaTag::GetAddressKey() );
   if( szAddr != "" )
   {
      aParentAddress = Address( szAddr );
      szID = StringInterface::FindTagInList( sudoItem.MetaTags, MetaTag::GetHashKey() );
      bThisIsParent = (0 == GetIdentifier().Compare( aParentAddress ));
   }

   vector<string> vecRetval;
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
         auto evt = AddItem( sudoItem.Name, sudoItem.Identifiers, sudoItem.MetaTags );
         vecRetval.push_back( evt );
      }
   }

   return vecRetval;
}

// This needs "Card Name : { __hash="hashval" }" All other values are irrelevant.
vector<string>
Collection::loadRemoveLine( const string& aszLine )
{
   CollectionObject::PseudoIdentifier sudoItem;
   CollectionObject::ParseCardLine( aszLine, sudoItem );

   vector<string> vecRetval;
   string szUID = StringInterface::FindTagInList( sudoItem.MetaTags, MetaTag::GetUIDKey() );
   if( szUID != "" )
   {
      for( size_t i = 0; i < sudoItem.Count; i++ )
      {
         auto evt = RemoveItem( sudoItem.Name, szUID );
         vecRetval.push_back( evt );
      }
   }
   return vecRetval;
}

vector<Tag>
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
      return vector<Tag>();
   }
   
   string szUID  = StringInterface::FindTagInList( sudoOldItem.MetaTags, MetaTag::GetUIDKey() );
   auto cItem = oldItem->FindCopy( szUID );
   if( !cItem.Good() )
   {
      // TODO: Error.
      return vector<Tag>();
   }
   vector<Tag> vecRetval;
   for( size_t i = 0; i < sudoOldItem.Count; i++ )
   {
      if( sudoOldItem.Name == sudoNewItem.Name )
      {
         auto evt = ChangeItem( sudoOldItem.Name,
                            szUID,
                            sudoNewItem.Identifiers,
                            sudoNewItem.MetaTags );
         vecRetval.push_back( evt );
      }
      else
      {
         auto newItem = m_ptrCollectionSource->GetCardPrototype( sudoNewItem.Name );
         if( newItem.Good() )
         {
            auto evt = ReplaceItem( sudoOldItem.Name,
                                szUID,
                                sudoNewItem.Name,
                                sudoNewItem.Identifiers,
                                sudoNewItem.MetaTags );
            vecRetval.push_back( evt );
         }
      }
   }

   return vecRetval;
}
