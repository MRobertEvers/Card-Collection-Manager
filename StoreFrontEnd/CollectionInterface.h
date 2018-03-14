#pragma once
#include "CardInterface.h"
#include "StoreFrontEnd.h"
#include <string>
#include <vector>

class CollectionInterface
{
public:
   CollectionInterface(const std::string& aszColdID);
   ~CollectionInterface();

   std::vector<CardInterface>& GetItemInterfaces();
   void PrepareInterface( Query aquery );
   void Refresh();

private:
   std::string m_szColId;
   std::vector<CardInterface> m_vecCopies;
   Query m_LastQuery;
};

