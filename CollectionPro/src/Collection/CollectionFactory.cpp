#include "CollectionFactory.h"

#include "../Config.h"

using namespace std;

CollectionFactory::CollectionFactory(CollectionSource* aoColSource)
{
   m_ColSource = aoColSource;
   m_lstCollections.reserve(10);
}


CollectionFactory::~CollectionFactory()
{
}

void 
CollectionFactory::SaveCollection( string aszCollectionID ) const
{
   auto collection = GetCollection(aszCollectionID);
   if( collection.Good() )
   {
      // Save each member of this family.
      Location oColLocation = collection->GetIdentifier();
      for( auto lCol : m_lstCollections )
      {
         Address olColLocation = lCol->GetIdentifier().ToAddress();
         if( olColLocation.ContainsLocation( oColLocation ) )
         {
            lCol->SaveCollection();
         }
      }
   }
}

string 
CollectionFactory::LoadCollectionFromFile(string aszFileName)
{
   string szRetVal = Config::NotFoundString;
   Collection* oCol;
   bool bInitialized;
   bool bKeepCol = false;

   // If this collection is already in the process of loading.
   // This is to allow for users to say that one collection loads another
   // and vice versa.
   if( m_setInLoading.find( aszFileName ) != m_setInLoading.end() )
   {
      return szRetVal;
   }

   if( !m_ColSource->IsLoaded() )
   {
      // TODO indicate this
      return szRetVal;
   }

   // Create a collection object to work with.
   oCol = new Collection(Config::NotFoundString, m_ColSource);
   bInitialized = oCol->InitializeCollection(aszFileName);
   auto szFoundID = oCol->GetIdentifier();

   // Only load the collection if it doesn't already exist.
   if( !CollectionExists(szFoundID) && bInitialized )
   {
      // Stop circular loading.
      m_setInLoading.insert(aszFileName);

      // Load the collection
      oCol->LoadCollection(aszFileName, this);
      if( oCol->IsLoaded() )
      {
         m_lstCollections.push_back(shared_ptr<Collection>(oCol));
         szRetVal = szFoundID.GetFullAddress();

         bKeepCol = true;
      }
   }

   // If something went wrong with loading, delete the collection.
   if( !bKeepCol )
   {
      delete oCol;
      szRetVal = Config::NotFoundString;
   }

   m_setInLoading.erase(aszFileName);

   return szRetVal;
}

string 
CollectionFactory::CreateNewCollection(string aszColName, string aszParentID)
{
   Collection* oCol;
   if( !m_ColSource->IsLoaded() )
   {
      // TODO indicat this
      return Config::NotFoundString;
   }

   // The parent is required to be loaded to have it as a parent
   if (CollectionExists(aszParentID))
   {
      oCol = new Collection( aszColName, m_ColSource,
                             getNextChildName(aszParentID) );
      auto parent = GetCollection(aszParentID);
      parent->ChildAdded();
      parent->SaveCollection();
   }
   else
   {
      oCol = new Collection(aszColName, m_ColSource);
   }

   m_lstCollections.push_back(shared_ptr<Collection>(oCol));
   return oCol->GetIdentifier().GetFullAddress();
}

vector<string>
CollectionFactory::GetLoadedCollections()
{
   vector<string> lstRetval;
   vector<shared_ptr<Collection>>::iterator iter_Colo;
   for ( iter_Colo = m_lstCollections.begin();
         iter_Colo != m_lstCollections.end(); 
         ++iter_Colo)
   {
      Collection* curr_Col = iter_Colo->get();
      lstRetval.push_back(curr_Col->GetIdentifier().GetFullAddress());
   }
   return lstRetval;
}

bool 
CollectionFactory::CollectionExists(string aszCollectionID) const
{
   return CollectionExists(Location(aszCollectionID));
}

bool 
CollectionFactory::CollectionExists(const Location& aAddrColID) const
{
   return GetCollection(aAddrColID).Good();
}

bool 
CollectionFactory::CollectionFamilyExists( const Address& aAddrColID ) const
{
   bool bExists = false;
   for( auto& loc : aAddrColID.GetLocations() )
   {
      bExists |= CollectionExists( loc );
      if( bExists )
      {
         break;   
      }
   }

   return bExists;
}

TryGet<Collection> 
CollectionFactory::GetCollection(string aszCollectionID) const
{
   return GetCollection(Location(aszCollectionID));
}

TryGet<Collection> 
CollectionFactory::GetCollection(const Location& aAddrColID) const
{
   TryGet<Collection> oRetVal;

   vector<shared_ptr<Collection>>::const_iterator iter_cols;
   for ( iter_cols  = m_lstCollections.cbegin();
         iter_cols != m_lstCollections.cend(); 
         ++iter_cols)
   {
      Collection* curr_Col = iter_cols->get();
      if (aAddrColID == curr_Col->GetIdentifier())
      {
         oRetVal.Set(iter_cols->get());
         break;
      }
   }

   return oRetVal;
}

vector<Collection*> 
CollectionFactory::GetCollectionFamily( const Address& aAddrColID ) const
{
   vector<Collection*> vecRetVal;

   bool bExists = false;
   for( auto& loc : aAddrColID.GetLocations() )
   {
      bExists = CollectionExists( loc );
      if( bExists )
      {
         vecRetVal.push_back(GetCollection(loc).Value());
      }
   }

   return vecRetVal;
}

string 
CollectionFactory::GetCollectionID(const std::string& aszCollectionName)
{
   vector<shared_ptr<Collection>>::const_iterator iter_cols;
   for ( iter_cols  = m_lstCollections.cbegin();
         iter_cols != m_lstCollections.cend(); 
         ++iter_cols)
   {
      Collection* curr_Col = iter_cols->get();
      if ( aszCollectionName == curr_Col->GetName())
      {
         return curr_Col->GetIdentifier().GetFullAddress();
      }
   }

   return Config::NotFoundString;
}

std::vector<Tag> 
CollectionFactory::PeekCollection( const std::string& aszColFile ) const
{
   string szFullpath = Config::Instance()->GetCollectionsDirectory() + "\\" + aszColFile;
   auto ptCol = std::shared_ptr<Collection>(new Collection( Config::NotFoundString, m_ColSource ));
   ptCol->InitializeCollection( szFullpath );

   return ptCol->GetPeekValues();
}

// Returns true if the action was performed.
// All commands must be at least 4 characterrs
bool 
CollectionFactory::processAction( const string& aszAction,
                                  bool abPreload )
{
   size_t iCmdSize = aszAction.find_first_of(' ');
   if( iCmdSize < 4 )
   {
      return false;
   }

   string szCmdStart = aszAction.substr(0, iCmdSize);

   // If pre-load is true, then only operate if Pre- is present.
   // If pre-load is false, then only operate if Pre- is not present.
   bool bProcess = ( abPreload == (szCmdStart == "Pre-") );
   if( bProcess )
   {
      string szCmd;
      string szParms = aszAction.substr(iCmdSize);
      if( abPreload )
      {
         szCmd = aszAction.substr(4, iCmdSize-4);
      }
      else
      {
         szCmd = aszAction.substr(0, iCmdSize);
      }
      performAction( szCmd, szParms );
   }

   return bProcess;
}

// Command must be at least 4 characters long
// Parms must be separated by spaces
bool 
CollectionFactory::performAction( const string& aszActionCmd, 
                                  const string& aszParms )
{
   // Split on " first
   vector<string> lstParms = StringHelper::Str_CmdLineParse(aszParms);

   // Load <CollectionFile>
   if( aszActionCmd == "Load" )
   {
      if( lstParms.size() == 1 )
      {
         string szParmOne = lstParms[0];
         LoadCollectionFromFile(szParmOne);
         return true;
      }
   }
   return false;
}

string 
CollectionFactory::getNextChildName(string aszParentID) const
{
   Addresser addresser;
   unsigned int iHighPrime, iHighPrimeIndex, iCurrentSA;

   Location currentName(aszParentID);
   iCurrentSA = currentName.GetSubAddress();
   iHighPrimeIndex = addresser.GetHighPrimeIndex(iCurrentSA);

   string szSubAddress;
   string szRetval = aszParentID;
   TryGet<Collection> oCol = GetCollection(aszParentID);
   if (oCol.Good())
   {
      int iChildren = oCol->GetChildCount();
      iHighPrimeIndex += iChildren + 1;
      
      iHighPrime = addresser.GetPrime(iHighPrimeIndex);
      szSubAddress = to_string(iCurrentSA*iHighPrime);
      szRetval = currentName.GetMain() + "-" + szSubAddress;
   }

   return szRetval;
}
