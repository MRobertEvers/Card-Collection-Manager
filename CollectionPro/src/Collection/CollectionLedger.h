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

   std::set<const CopyItem*> ViewOwned();
   std::set<const CopyItem*> ViewPresent();
   Location GetLocation();

private:
   friend class AddressBook;

   Location m_locParent;

   void buildTree();

public:
   static TryGetCopy<CollectionLedger*> Lookup( const Location& aszBaseAddress );
   static TryGetCopy<CollectionTree::CollectionNode*> GetFamilyNode( const Location& aszLocation );

private:
   static std::map<Location, CollectionLedger*> ms_LedgerLookup;
   static std::map<std::string, CollectionTree*> ms_FamilyTrees;
};