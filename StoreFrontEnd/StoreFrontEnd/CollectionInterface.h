#pragma once
#include "CardInterface.h"
#include "StoreFrontEnd.h"
#include <string>
#include <vector>
#include <list>
#include <map>
#include <memory>

// This class is a hacky workaround...
class InterfaceSet
{
public:
   InterfaceSet( CollectionInterface* apParent );
   ~InterfaceSet();

   void BulkInsert( const std::vector<std::string>& avecBulk );
   CardInterface* AddItem( const std::string& aszName, const std::string& aszUID );
   CardInterface* RemoveItem( const std::string& aszName, const std::string& aszUID );

   std::list<CardInterface>& GetItemInterfaces();

private:
   CollectionInterface* m_pParent;

   // Todo: this is hokey
   std::list<CardInterface> m_lstIFaces;
   std::map<std::string, std::list<CardInterface>::iterator> m_mapHashToIter;

   // So bad... Name, uid -> iter
   std::map<std::string, std::map<std::string, std::list<CardInterface>::iterator>> m_mapNameUIDToIter;

};

class CollectionResolved
{
public:
   CollectionResolved(  );
   ~CollectionResolved();

   std::vector<CardInterface*>& GetAdded();
   std::vector<CardInterface*>& GetRemoved();

   void AddAdd( CardInterface* apAdd );
   void AddRemoved( CardInterface* apAdd );

private:
   std::vector<CardInterface*> m_mapAdded;
   std::vector<CardInterface*> m_mapRemoved;
};

// We don't need to track 'changed' uids because we don't maintain their state anyway.
class CollectionDelta
{
public:
   CollectionDelta( const std::vector<std::string>& avecChangedUIDs );
   ~CollectionDelta();

   std::map<std::string,std::vector<std::string>>& GetAdded();
   std::map<std::string,std::vector<std::string>>& GetRemoved();

private:
   std::map<std::string, std::vector<std::string>> m_mapAdded;
   std::map<std::string, std::vector<std::string>> m_mapRemoved;
};

class CollectionInterface
{
public:
   CollectionInterface(const std::string& aszColdID);
   ~CollectionInterface();

   std::list<CardInterface>& GetItemInterfaces();
   std::map<unsigned long, std::vector<std::string>> GetHistoryGroups();
   unsigned int GetItemCount();
   std::string GetColId();
   void PrepareInterface( Query aquery );
   void Refresh();
   CollectionResolved ReflectDelta( const std::shared_ptr<CollectionDelta> apDelta );

private:

   InterfaceSet m_CopySet;
   std::string m_szColId;
   Query m_LastQuery;
};

