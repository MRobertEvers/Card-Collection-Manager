#pragma once
#include "../Addressing/Addresser.h"
#include "../Support/TryGet.h"

#include <string>
#include <vector>
#include <memory>

class CollectionLedger;

class CollectionTree
{
public:
   class CollectionNode
   {
   public:
      CollectionNode( const Location& aNode );
      CollectionNode( const Location& aNode, CollectionLedger* aptLedger );
      ~CollectionNode();

      void SetParent( CollectionNode* aptParent );

      TryGetCopy<CollectionNode*> FindNode( const Location& aNode );
      bool InsertNode( CollectionNode* aptNode );
      void AddNode( CollectionNode* aptNode );
      bool RemoveNode( CollectionNode* aptNode );

      Location GetLocation();

      CollectionNode* GetParent();
      std::vector<CollectionNode*> GetChildren();
      CollectionLedger* GetLedger();
      void OverrideLedger(CollectionLedger* aptLedger);

      bool IsVirtual();

   private:
      Location m_Location;
      CollectionLedger* m_ptLedger;
      CollectionNode* m_ptParent;
      std::vector<CollectionNode*> m_vecChildren;
   };

public:
   CollectionTree(const std::string& aszTree);
   ~CollectionTree();

   void AddCollectionNode( const Location& aNode, CollectionLedger* aptLedger );
   TryGetCopy<CollectionNode*> FindCollectionNode( const Location& aNode );
   void RemoveNode( const Location& aNode );
   bool IsLive();

private:
   CollectionNode* m_ptRootNode;
   std::string m_szFamilyLine;
};

