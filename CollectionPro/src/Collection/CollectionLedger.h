#pragma once
#include "Ledger.h"
#include "CollectionTree.h"
#include "..\Support\TryGet.h"

#include <map>
#include <string>
#include <memory>
#include <vector>

class Location;
class CopyItem;

class CollectionLedger : public Ledger
{
public:
   CollectionLedger( const Location& alocParent );
   ~CollectionLedger();

   std::set<std::shared_ptr<CopyItem>> ViewOwned();
   std::set<std::shared_ptr<CopyItem>> ViewPresent();
   std::map<CollectionObject*, std::set<std::shared_ptr<CopyItem>>>  ViewSortedPresent();
   Location GetLocation();

private:
   friend class AddressBook;

   Location m_locParent;

   void buildTree();

public:
   static TryGetCopy<CollectionTree::CollectionNode*> GetFamilyNode( const Location& aszLocation );

private:
   static std::map<std::string, CollectionTree*> ms_FamilyTrees;
};