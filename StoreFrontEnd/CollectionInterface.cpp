#include "CollectionInterface.h"
#include "StoreFrontEnd.h"


CollectionInterface::CollectionInterface( const std::string& aszColdID )
   : m_szColId(aszColdID)
{
}


CollectionInterface::~CollectionInterface()
{
}

std::vector<CardInterface>& 
CollectionInterface::GetItemInterfaces()
{
   return m_vecCopies;
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