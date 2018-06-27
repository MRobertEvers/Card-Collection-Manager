#include "CollectionInterface.h"
#include "StoreFrontEnd.h"
#include <set>

CollectionInterface::CollectionInterface( const std::string& aszColdID )
   : m_szColId(aszColdID), m_CopySet(this)
{

}


CollectionInterface::~CollectionInterface()
{

}

std::list<CardInterface>& 
CollectionInterface::GetItemInterfaces()
{
   return m_CopySet.GetItemInterfaces();
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

   m_CopySet.BulkInsert( lstCol );
}

CollectionResolved
CollectionInterface::ReflectDelta( const std::shared_ptr<CollectionDelta> apDelta )
{
   CollectionResolved retval;
   for( auto& rem : apDelta->GetAdded() )
   {
      for( auto& uid : rem.second )
      {
         auto removed = m_CopySet.RemoveItem( rem.first, uid );
         retval.AddRemoved( removed );
      }
   }

   return retval;
}

CollectionDelta::CollectionDelta( const std::vector<std::string>& avecChangedUIDs )
{
   for( auto& delta : avecChangedUIDs )
   {
      std::string szName;
      unsigned int iCount;
      std::vector<std::pair<std::string, std::string>> vecAttrs;
      std::vector<std::pair<std::string, std::string>> vecUids;

      int iCmd = delta.find_first_of( "+-" );
      if( iCmd < 0 )
      {
         continue;
      }

      std::string szBase = delta.substr( iCmd );

      StringInterface::ParseCardLine( szBase, iCount, szName, vecAttrs, vecUids );

      if( delta[iCmd] == '+' )
      {
         for( auto& uid : vecUids )
         {
            m_mapAdded[szName].push_back( uid.second );
         }
      }
      else
      {
         for( auto& uid : vecUids )
         {
            m_mapRemoved[szName].push_back( uid.second );
         }
      }
   }
}

CollectionDelta::~CollectionDelta()
{
}

// Cardname, uids
std::map<std::string, std::vector<std::string>>& 
CollectionDelta::GetAdded()
{
   // TODO: insert return statement here
   return m_mapAdded;
}

std::map<std::string, std::vector<std::string>>& 
CollectionDelta::GetRemoved()
{
   // TODO: insert return statement here
   return m_mapRemoved;
}

InterfaceSet::InterfaceSet( CollectionInterface* apParent )
   : m_pParent( apParent )
{
}

InterfaceSet::~InterfaceSet()
{
}

void 
InterfaceSet::BulkInsert( const std::vector<std::string>& avecBulk )
{
   m_lstIFaces.clear();
   m_mapHashToIter.clear();
   m_mapNameUIDToIter.clear();
   for( auto& szItem : avecBulk )
   {
      m_lstIFaces.push_back( CardInterface( szItem, m_pParent ) );
      auto iter_insert = m_lstIFaces.back();
      //m_mapHashToIter[iter_insert->GetHash()] = iter_insert;
      //for( auto& uid : iter_insert->GetRepresentingUIDs() )
      //{
      //   m_mapNameUIDToIter[iter_insert->GetName()][uid] = iter_insert;
      //}
   }
}

CardInterface*
InterfaceSet::AddItem( const std::string & aszName, const std::string & aszUID )
{
   bool bDoAdd = true;
   auto iter_name = m_mapNameUIDToIter.find( aszName );
   if( iter_name != m_mapNameUIDToIter.end() )
   {
      auto iter_uid = iter_name->second.find( aszUID );
      if( iter_uid != iter_name->second.end() )
      {
         // Do nothing
         bDoAdd = false;
      }
   }

   if( bDoAdd )
   {
      CardInterface tmp( aszName, std::vector<std::string>( 1, aszUID ), m_pParent );
      auto iter_insert = m_lstIFaces.insert( m_lstIFaces.end(), tmp );
      m_mapHashToIter[tmp.GetHash()] = iter_insert;
      m_mapNameUIDToIter[aszName][aszUID] = iter_insert;

      return &*iter_insert;
   }
}

CardInterface*
InterfaceSet::RemoveItem( const std::string & aszName, const std::string & aszUID )
{
   bool bDoRemove = false;
   std::list<CardInterface>::iterator iter_find;
   auto iter_name = m_mapNameUIDToIter.find( aszName );
   if( iter_name != m_mapNameUIDToIter.end() )
   {
      auto iter_uid = iter_name->second.find( aszUID );
      if( iter_uid != iter_name->second.end() )
      {
         iter_find = iter_uid->second;
         bDoRemove = true;

         iter_name->second.erase( iter_uid );
         if( iter_name->second.size() == 0 )
         {
            m_mapNameUIDToIter.erase( iter_name );
         }
      }
   }

   if( bDoRemove )
   {
      CardInterface* ret = &*iter_find;
      m_lstIFaces.erase( iter_find );
      return ret;
   }
}

std::list<CardInterface>& 
InterfaceSet::GetItemInterfaces()
{
   return m_lstIFaces;
}

CollectionResolved::CollectionResolved()
{
}

CollectionResolved::~CollectionResolved()
{
}

std::vector<CardInterface*>& 
CollectionResolved::GetAdded()
{
   // TODO: insert return statement here
   return m_mapAdded;
}

std::vector<CardInterface*>& 
CollectionResolved::GetRemoved()
{
   // TODO: insert return statement here
   return m_mapRemoved;
}

void 
CollectionResolved::AddAdd( CardInterface * apAdd )
{
   m_mapAdded.push_back( apAdd );
}

void 
CollectionResolved::AddRemoved( CardInterface * apAdd )
{
   m_mapRemoved.push_back( apAdd );
}
