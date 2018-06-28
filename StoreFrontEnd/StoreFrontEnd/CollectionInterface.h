#pragma once
#include "CardInterface.h"
#include "StoreFrontEnd.h"
#include <string>
#include <vector>
#include <memory>
#include <list>
#include <map>
class CollectionInterface;
class CollectionDelta;
class CollectionDeltaResolution;
class CollectionInterfaceSet
{
public:
   CollectionInterfaceSet( CollectionInterface* apParent );
   ~CollectionInterfaceSet();

   std::shared_ptr<CollectionDeltaResolution> Refresh( std::shared_ptr<CollectionDelta> apDelta );
   void BulkLoad( const std::vector<std::string>& avecNew );
   std::list<CardInterface>::iterator AddItem( const std::string& aszName, const std::string& aszUID );
   std::pair<std::string, std::string> RemoveItem( const std::string& aszName, const std::string& aszUID );

   std::list<CardInterface>& GetItemInterfaces();
private:
   CollectionInterface* m_pParent;
   std::list<CardInterface> m_lstCopies;
   std::map<std::string, std::map<std::string, std::list<CardInterface>::iterator>> m_mapUIDName;
   std::multimap<std::string, std::list<CardInterface>::iterator> m_mapHash;
};

class CollectionDeltaResolution
{
public:
   void AddRemove( const std::string& aszName, const std::string& aszUID );
   void AddAdd( std::list<CardInterface>::iterator aIter );

   std::vector<std::list<CardInterface>::iterator>& GetAdded();
   std::vector<std::pair<std::string, std::string>>& GetRemoved();
private:
   std::vector<std::list<CardInterface>::iterator> m_vecAdded;
   std::vector<std::pair<std::string, std::string>> m_vecRemoved;
};

class CollectionDelta
{
public:
   CollectionDelta( const std::vector<std::string>& avecChangedUIDs );
   ~CollectionDelta();

   std::map<std::string, std::vector<std::string>>& GetAdded();
   std::map<std::string, std::vector<std::string>>& GetRemoved();

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
   std::shared_ptr<CollectionDeltaResolution> Refresh( std::shared_ptr<CollectionDelta> apDelta );

private:
   std::string m_szColId;
   CollectionInterfaceSet m_Set;
   Query m_LastQuery;
};

