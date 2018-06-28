#include "CollectionInterface.h"
#include "StoreFrontEnd.h"


CollectionInterface::CollectionInterface( const std::string& aszColdID )
   : m_szColId(aszColdID), m_Set(this)
{
}


CollectionInterface::~CollectionInterface()
{
}

std::list<CardInterface>& 
CollectionInterface::GetItemInterfaces()
{
   return m_Set.GetItemInterfaces();
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
   for( auto& item : GetItemInterfaces() )
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

// Prepare interface must be called before this function!!
void 
CollectionInterface::Refresh()
{
   auto ptSF = StoreFrontEnd::Server();
   auto lstCol = ptSF->GetAllCardsStartingWith( m_szColId, m_LastQuery );

   m_Set.BulkLoad( lstCol );
}

CollectionInterfaceSet::CollectionInterfaceSet( CollectionInterface* apParent )
   : m_pParent(apParent)
{
}

CollectionInterfaceSet::~CollectionInterfaceSet()
{
}

void 
CollectionInterfaceSet::BulkLoad( const std::vector<std::string>& avecNew )
{
   m_lstCopies.clear();
   for( auto& szItem : avecNew )
   {
      auto tmp = CardInterface( szItem, m_pParent );
      auto iter_insert = m_lstCopies.insert( m_lstCopies.end(), tmp );

      auto& entry = m_mapUIDName[tmp.GetName()];
      for( auto& uid : iter_insert->GetRepresentingUIDs() )
      {
         entry[uid] = iter_insert;
      }

      m_mapHash.emplace( tmp.GetHash(), iter_insert);
   }
}

std::pair<std::string, std::string>
CollectionInterfaceSet::AddItem( const std::string & aszName, const std::string & aszUID )
{
   CardInterface tmp( aszName, aszUID, m_pParent );

   bool bNeedAdd = false;
   std::pair<std::string, std::string> pairRetval;
   auto iter_name = m_mapUIDName.find( aszName );
   if( iter_name != m_mapUIDName.end() )
   {
      auto iter_uid = iter_name->second.find( aszUID );
      if( iter_uid != iter_name->second.end() )
      {
         return pairRetval;
      }
      else
      {
         bNeedAdd = true;
      }
   }
   else
   {
      bNeedAdd = true;
   }

   if( bNeedAdd )
   {
      auto iter_insert = m_lstCopies.insert( m_lstCopies.end(), tmp );
      m_mapHash.emplace( tmp.GetHash(), iter_insert );
      
      if( iter_name != m_mapUIDName.end() )
      {
         iter_name->second.emplace( aszUID, iter_insert );
      }
      else
      {
         m_mapUIDName[aszName][aszUID] = iter_insert;
      }

      pairRetval.first = aszName;
      pairRetval.second = aszUID;
   }

   return pairRetval;
}

std::pair<std::string, std::string>
CollectionInterfaceSet::RemoveItem( const std::string & aszName, const std::string & aszUID )
{
   bool bFound = false;
   std::list<CardInterface>::iterator iter_found = m_lstCopies.end();
   std::pair<std::string, std::string> pairRetval;
   auto iter_name = m_mapUIDName.find( aszName );
   if( iter_name != m_mapUIDName.end() )
   {
      auto iter_uid = iter_name->second.find( aszUID );
      if( iter_uid != iter_name->second.end() )
      {
         iter_found = iter_uid->second;
         bFound = true;
         iter_name->second.erase( iter_uid );

         if( iter_name->second.size() == 0 )
         {
            m_mapUIDName.erase( iter_name );
         }
      }
   }

   if( bFound )
   {
      auto iter_hrange = m_mapHash.equal_range( iter_found->GetHash() );
      for( auto iter_item = iter_hrange.first; iter_item != iter_hrange.second; iter_item++ )
      {
         for( auto& has_uid : iter_item->second->GetRepresentingUIDs() )
         {
            if( has_uid == aszUID )
            {
               m_mapHash.erase( iter_item );
               break;
            }
         }
      }

      m_lstCopies.erase( iter_found );

      pairRetval.first = aszName;
      pairRetval.second = aszUID;
   }

   return pairRetval;
}

std::list<CardInterface>& 
CollectionInterfaceSet::GetItemInterfaces()
{
   return m_lstCopies;
}