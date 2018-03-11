#include "CollectionLedger.h"
#include "../Addressing/Addresser.h"

std::map<Location, CollectionLedger*>
CollectionLedger::ms_LedgerLookup;

std::map<std::string,  CollectionTree*> 
CollectionLedger::ms_FamilyTrees;

CollectionLedger::CollectionLedger( const Location& alocParent )
   : m_locParent(alocParent)
{
   buildTree();
}

CollectionLedger::~CollectionLedger()
{
   auto iter_FamilyFind = ms_FamilyTrees.find( m_locParent.GetMain() );
   iter_FamilyFind->second->RemoveNode( m_locParent );
   if( !iter_FamilyFind->second->IsLive() )
   {
      ms_FamilyTrees.erase( iter_FamilyFind );
   }
}

std::set<const CopyItem*>
CollectionLedger::ViewOwned()
{
   return m_vecOwned;
}

std::set<const CopyItem*>
CollectionLedger::ViewPresent()
{
   return m_vecPresent;
}

Location 
CollectionLedger::GetLocation()
{
   return m_locParent;
}

void 
CollectionLedger::buildTree()
{
   // Construct each node up to this node.
   // May create virtual nodes.
   auto iter_FamilyFind = ms_FamilyTrees.find( m_locParent.GetMain() );
   if( iter_FamilyFind == ms_FamilyTrees.end() )
   {
      auto ptNewTree = new CollectionTree( m_locParent.GetMain() );
      iter_FamilyFind = ms_FamilyTrees.insert( std::make_pair( m_locParent.GetMain(), ptNewTree ) ).first;
   }

   // Look to see if this node is already virtual.
   auto iter_node = iter_FamilyFind->second->FindCollectionNode( m_locParent );
   if( iter_node.Good() )
   {
      auto ptNode = *iter_node.Value();
      ptNode->OverrideLedger( this );
   }
   else
   {
      // Build it up.
      for( auto& potentialNode : m_locParent.GetLocationsSpecified() )
      {
         if( potentialNode != m_locParent )
         {
            iter_FamilyFind->second->AddCollectionNode( potentialNode, nullptr );
         }
      }

      iter_FamilyFind->second->AddCollectionNode( m_locParent, this );
   }
}

TryGetCopy<CollectionLedger*>
CollectionLedger::Lookup( const Location& aID )
{
   TryGetCopy<CollectionLedger*> ptRetVal;

   auto iter_find = ms_LedgerLookup.find( aID );
   if( iter_find != ms_LedgerLookup.end() )
   {
      ptRetVal.Set( iter_find->second );
   }

   return ptRetVal;
}


TryGetCopy<CollectionTree::CollectionNode*>
CollectionLedger::GetFamilyNode( const Location& aszLocation )
{
   TryGetCopy<CollectionTree::CollectionNode*> ptRetVal;
   auto iter_Family = ms_FamilyTrees.find( aszLocation.GetMain() );
   if( iter_Family != ms_FamilyTrees.end() )
   {
      auto iter_node = iter_Family->second->FindCollectionNode( aszLocation );
      if( iter_node.Good() )
      {
         ptRetVal.Set( *iter_node.Value() );
      }
   }

   return ptRetVal;
}