#include "CollectionInterface.h"
#include "StoreFrontEnd.h"


CollectionInterface::CollectionInterface( const std::string& aszColdID )
   : m_szColId(aszColdID)
{
}


CollectionInterface::~CollectionInterface()
{
}

std::list<CardInterface>& 
CollectionInterface::GetItemInterfaces()
{
   return m_vecCopies;
}

std::map<unsigned long, std::vector<std::string>>
CollectionInterface::GetHistoryGroups()
{
   auto ptSF = StoreFrontEnd::Server();
   return ptSF->GetHistoryLines( GetColId(), 0 );
}

unsigned int 
CollectionInterface::GetItemCount()
{
   unsigned int uiCount = 0;
   for( auto& item : m_vecCopies )
   {
      uiCount += item.GetNumber();
   }
   return uiCount;
}

std::string 
CollectionInterface::GetColId()
{
   return m_szColId;
}

void 
CollectionInterface::PrepareInterface( Query aquery )
{
   m_LastQuery = aquery;
}

void 
CollectionInterface::Refresh()
{
   auto ptSF = StoreFrontEnd::Server();
   auto lstCol = ptSF->GetAllCardsStartingWith( m_szColId, m_LastQuery );

   m_vecCopies.clear();
   for( auto& szItem : lstCol )
   {
      m_vecCopies.push_back( CardInterface( szItem ) );
   }
}