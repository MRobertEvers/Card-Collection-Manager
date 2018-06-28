#include "CollectionInterface.h"
#include "StoreFrontEnd.h"
#include <wx/wxprec.h>


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

std::shared_ptr<CollectionDeltaResolution>
CollectionInterface::Refresh( std::shared_ptr<CollectionDelta> apDelta )
{
   return m_Set.Refresh( apDelta );
}

CollectionInterfaceSet::CollectionInterfaceSet( CollectionInterface* apParent )
   : m_pParent(apParent)
{
}

CollectionInterfaceSet::~CollectionInterfaceSet()
{
}

std::shared_ptr<CollectionDeltaResolution>
CollectionInterfaceSet::Refresh( std::shared_ptr<CollectionDelta> apDelta )
{
   auto ret = std::shared_ptr<CollectionDeltaResolution>( new CollectionDeltaResolution() );

   // TODO: Handle if collapsed or not...
   for( auto& added : apDelta->GetAdded() )
   {
      auto iter_name = m_mapUIDName.find( added.first );
      if( iter_name != m_mapUIDName.end() )
      {
         // Check if we have it first.
         for( auto& uid : added.second )
         {
            auto iter_uid = iter_name->second.find( uid );
            if( iter_uid == iter_name->second.end() )
            {
               auto iter_added = AddItem( added.first, uid );
               ret->AddAdd( iter_added );
            }
         }
      }
      else
      {
         // Add all of them
         for( auto& uid : added.second )
         {
            auto iter_added = AddItem( added.first, uid );
            ret->AddAdd( iter_added );
         }
      }
   }

   for( auto& removed : apDelta->GetRemoved() )
   {
      auto iter_name = m_mapUIDName.find( removed.first );
      if( iter_name != m_mapUIDName.end() )
      {
         // Check if we have it first.
         for( auto& uid : removed.second )
         {
            auto iter_uid = iter_name->second.find( uid );
            if( iter_uid != iter_name->second.end() )
            {
               auto iter_rem = RemoveItem( removed.first, uid );
               ret->AddRemove( iter_rem.first, iter_rem.second );
            }
         }
      }
   }

   return ret;
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

std::list<CardInterface>::iterator
CollectionInterfaceSet::AddItem( const std::string & aszName, const std::string & aszUID )
{
   CardInterface tmp( aszName, aszUID, m_pParent );

   bool bNeedAdd = false;
   std::list<CardInterface>::iterator iter_add;
   auto iter_name = m_mapUIDName.find( aszName );
   if( iter_name != m_mapUIDName.end() )
   {
      auto iter_uid = iter_name->second.find( aszUID );
      if( iter_uid != iter_name->second.end() )
      {
         return iter_add;
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

      iter_add = iter_insert;
   }

   return iter_add;
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
         bool bBreak = false;
         for( auto& has_uid : iter_item->second->GetRepresentingUIDs() )
         {
            if( has_uid == aszUID )
            {
               m_mapHash.erase( iter_item );
               bBreak = true;
               break;
            }
         }

         if( bBreak = true )
         {
            break;
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

      std::string szBase = delta.substr( iCmd+1 );
      wxString base( szBase );
      base.Trim();
      szBase = base.ToStdString();

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

void 
CollectionDeltaResolution::AddRemove( const std::string & aszName, const std::string & aszUID )
{
   m_vecRemoved.push_back( std::make_pair( aszName, aszUID ) );
}

void 
CollectionDeltaResolution::AddAdd( std::list<CardInterface>::iterator aIter )
{
   m_vecAdded.push_back( aIter );
}

std::vector<std::list<CardInterface>::iterator>& 
CollectionDeltaResolution::GetAdded()
{
   return m_vecAdded;
}

std::vector<std::pair<std::string, std::string>>& 
CollectionDeltaResolution::GetRemoved()
{
   return m_vecRemoved;
}
