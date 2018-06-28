#pragma once
#include "CardInterface.h"
#include "StoreFrontEnd.h"
#include <string>
#include <vector>
#include <list>
#include <map>
class CollectionInterface;
class CollectionInterfaceSet
{
public:
   CollectionInterfaceSet( CollectionInterface* apParent );
   ~CollectionInterfaceSet();

   void BulkLoad( const std::vector<std::string>& avecNew );
   std::pair<std::string, std::string> AddItem( const std::string& aszName, const std::string& aszUID );
   std::pair<std::string, std::string> RemoveItem( const std::string& aszName, const std::string& aszUID );

   std::list<CardInterface>& GetItemInterfaces();
private:
   CollectionInterface* m_pParent;
   std::list<CardInterface> m_lstCopies;
   std::map<std::string, std::map<std::string, std::list<CardInterface>::iterator>> m_mapUIDName;
   std::multimap<std::string, std::list<CardInterface>::iterator> m_mapHash;
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

private:
   std::string m_szColId;
   CollectionInterfaceSet m_Set;
   Query m_LastQuery;
};

