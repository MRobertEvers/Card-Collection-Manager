#pragma once
#include "CardInterface.h"
#include "StoreFrontEnd.h"
#include <string>
#include <vector>
#include <list>
#include <map>

class CollectionDelta
{
public:
   CollectionDelta( const std::vector<std::string>& avecChangedUIDs );
   ~CollectionDelta();

private:
   std::vector<std::string> m_vecAdded;
   std::vector<std::string> m_vecRemoved;
   std::vector<std::pair<std::string, std::string>> m_vecChanged;
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
   std::list<CardInterface> m_vecCopies;
   Query m_LastQuery;
};

