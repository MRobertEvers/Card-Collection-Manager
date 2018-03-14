#include "CollectionTree.h"
#include "CollectionLedger.h"
#include "../Addressing/Addresser.h"

#include <algorithm>

CollectionTree::
CollectionNode::CollectionNode( const Location& aNode )
   : m_Location( aNode )
{
   m_ptLedger = nullptr;
}

CollectionTree::
CollectionNode::CollectionNode( const Location& aNode, CollectionLedger* aptLedger )
   : CollectionNode(aNode)
{
   m_ptLedger = aptLedger;
}

CollectionTree::
CollectionNode::~CollectionNode()
{

}

void 
CollectionTree::
CollectionNode::SetParent( CollectionNode* aptParent )
{
   m_ptParent = aptParent;
}

TryGetCopy<CollectionTree::CollectionNode*>
CollectionTree::
CollectionNode::FindNode( const Location& aNode )
{
   TryGetCopy<CollectionTree::CollectionNode*> ptRetVal;

   // Check the children first.
   for( auto& child : m_vecChildren )
   {
      if( aNode == child->GetLocation() )
      {
         ptRetVal.Set(child);
         break;
      }
   }

   // Check the childrens children
   for( auto& child : m_vecChildren )
   {
       auto addrChild = child->GetLocation().ToAddress();
       if( addrChild.ContainsLocation( aNode ) )
       {
          auto foundNode = child->FindNode( aNode );
          if( foundNode.Good() )
          {
             ptRetVal.Set( *foundNode.Value() );
             break;
          }
       }
   }

   return ptRetVal;
}

bool
CollectionTree::
CollectionNode::InsertNode( CollectionNode* aptNode )
{
   // 2. Find where this node would go.

   // 3. Dont add a duplicate.
   for( auto& child : m_vecChildren )
   {
      if( aptNode->GetLocation() == child->GetLocation() )
      {
         return false;
      }
   }

   bool bIsParentOfChild = false;
   auto iter_Child = m_vecChildren.begin();
   while( iter_Child != m_vecChildren.end() )
   {
      auto child = *iter_Child;

      // 4. Check if new node is a child of child.
      auto addrChildAddr = child->GetLocation().ToAddress();
      bool bChildIsParent = addrChildAddr.ContainsLocation( aptNode->GetLocation() );
      if( bChildIsParent )
      {
         return child->InsertNode( aptNode );
      }

      // 5. If the new node is a parent of a child node
      auto addrNewAddr = aptNode->GetLocation().ToAddress();
      bool bNodeIsParentOfChild = addrNewAddr.ContainsLocation( child->GetLocation() );
      if( bNodeIsParentOfChild )
      {
         bIsParentOfChild = true;
         aptNode->SetParent( child->GetParent() );
         aptNode->AddNode( child );
         child->SetParent( aptNode );
         iter_Child = m_vecChildren.erase( iter_Child );
      }
   }

   AddNode( aptNode );
   return true;
}

void
CollectionTree::
CollectionNode::AddNode( CollectionNode* aptChild )
{
   aptChild->SetParent( this );
   m_vecChildren.push_back( aptChild );
}

bool
CollectionTree::
CollectionNode::RemoveNode( CollectionNode* aptChild )
{
   auto iter_findNode = std::find( m_vecChildren.begin(), 
                                   m_vecChildren.end(), 
                                   aptChild );
   if( iter_findNode != m_vecChildren.end() )
   {
      if( (*iter_findNode)->GetChildren().size() > 0 )
      {
         (*iter_findNode)->OverrideLedger( nullptr );
      }
      else
      {
         m_vecChildren.erase( iter_findNode );
         if( m_vecChildren.size() == 0 && IsVirtual())
         {
            if( m_ptParent != nullptr )
            {
               m_ptParent->RemoveNode( this );
            }
            else
            {
               OverrideLedger( nullptr );
            }
         }
      }
      return true;
   }
   else
   {
      for( auto& child : m_vecChildren )
      {
         if( child->RemoveNode( aptChild ) )
         {
            return true;
         }
      }
   }
}

Location 
CollectionTree::
CollectionNode::GetLocation()
{
   return m_Location;
}

CollectionTree::CollectionNode*
CollectionTree::
CollectionNode::GetParent()
{
   return m_ptParent;
}

std::vector<CollectionTree::CollectionNode*>
CollectionTree::
CollectionNode::GetChildren()
{
   return m_vecChildren;
}

CollectionLedger*
CollectionTree::
CollectionNode::GetLedger()
{
   return m_ptLedger;
}

void 
CollectionTree::
CollectionNode::OverrideLedger( CollectionLedger* aptLedger )
{
   m_ptLedger = aptLedger;
}

bool 
CollectionTree::
CollectionNode::IsVirtual()
{
   return m_ptLedger == nullptr;
}

CollectionTree::CollectionTree(const std::string& aszFamilyLine )
   : m_szFamilyLine( aszFamilyLine )
{
  
}


CollectionTree::~CollectionTree()
{

}


void 
CollectionTree::AddCollectionNode( const Location& aNode, CollectionLedger* aptLedger )
{
   auto ptNode = new CollectionTree::CollectionNode( aNode, aptLedger );

   if( m_ptRootNode == nullptr )
   {
      m_ptRootNode = ptNode;
   }
   else
   {
      // 0. Verify its not a duplicate.
      bool bIsDuplicate = aNode == m_ptRootNode->GetLocation();
      if( bIsDuplicate )
      {
         return;
      }

      // 1. Check if we need to replace the root node.
      bool bIsParent = aNode.ToAddress().ContainsLocation( m_ptRootNode->GetLocation() );
      if( bIsParent )
      {
         ptNode->AddNode( m_ptRootNode );
         m_ptRootNode->SetParent( ptNode );
         m_ptRootNode = ptNode;
      }
      else
      {
         // Otherwise, try to insert the node;
         bool bDidInsert = m_ptRootNode->InsertNode( ptNode );
      }
   }   
}

TryGetCopy<CollectionTree::CollectionNode*>
CollectionTree::FindCollectionNode( const Location& aNode )
{
   TryGetCopy<CollectionTree::CollectionNode*> ptRetVal;
   if( m_ptRootNode == nullptr )
   {

   }
   else if( m_ptRootNode->GetLocation() == aNode )
   {
      ptRetVal.Set( m_ptRootNode );
   }
   else
   {
      auto iter_find = m_ptRootNode->FindNode( aNode );
      if( iter_find.Good() )
      {
         ptRetVal.Set( *iter_find.Value() );
      }
   }

   return ptRetVal;
}

void 
CollectionTree::RemoveNode( const Location& aNode )
{
   auto find_node = FindCollectionNode( aNode );
   if( find_node.Good() )
   {
      CollectionTree::CollectionNode* ptNode = *find_node.Value();
      bool bIsRoot = m_ptRootNode == ptNode;

      if( bIsRoot )
      {
         if( m_ptRootNode->GetChildren().size() > 0 )
         {
            m_ptRootNode->OverrideLedger( nullptr );
         }
         else
         {
            delete m_ptRootNode;
            m_ptRootNode = nullptr;
         }
         // TODO: Need to handle the orphaned nodes...
      }
      else
      {
         auto ptParent = ptNode->GetParent();
         ptNode->RemoveNode( ptNode );
         if( ptParent != nullptr && ptParent->IsVirtual() )
         {
            RemoveNode( ptParent->GetLocation() );
         }
      }
   }
}

bool 
CollectionTree::IsLive()
{
   return m_ptRootNode != nullptr;
}