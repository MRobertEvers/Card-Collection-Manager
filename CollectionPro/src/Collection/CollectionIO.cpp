#include "CollectionIO.h"
#include "Collection.h"
#include "CollectionFactory.h"
#include "CollectionLedger.h"
#include "../StringInterface.h"
#include "../Addressing/Addresser.h"

#include <algorithm>
#include <iomanip>
#include <ctime>

using namespace std;

CollectionIO::CollectionIO(Collection* aptCollection)
   : m_ptCollection(aptCollection)
{
   m_ptLoadToken = std::unique_ptr<LoadToken>( new LoadToken() );
}


CollectionIO::~CollectionIO()
{
}

bool 
CollectionIO::InitializeCollection( const std::string& aszFileName )
{
   PrepareCollectionInitialization( aszFileName );
   PopulateCollectionFields();

   return true;
}

bool 
CollectionIO::LoadCollection( const string& aszFileName, CollectionFactory* aoFactory )
{
   m_ptFactory = aoFactory;

   PrepareCollectionInitialization( aszFileName );
   PrepareCollectionItems();
   PopulateCollectionFields();
   ConsolodateItems();
   FinalizeLoad();

   return true;
}

bool 
CollectionIO::PrepareCollectionInitialization( const string& aszFileName )
{
   bool bRetVal = true;

   // a. Save the file name.
   m_ptLoadToken->CollectionFileName = StripFileName(aszFileName);

   // b. Get lines in quotes and card lines from file.
   vector<string> vecFileLines;
   bRetVal &= GetFileLines( aszFileName, vecFileLines );

   // c. Save the collection name
   string szName;
   vector<string> vecCardLines;
   bRetVal &= GetNameAndCollectionLines( vecFileLines, szName, vecCardLines );
   m_ptLoadToken->CollectionName = szName;
   m_ptLoadToken->CardLines = vecCardLines;

   // d. Load the overhead file. Requires Collection name. Save
   // the processlines for saving later.
   loadOverheadFile();

   return bRetVal;
}

bool 
CollectionIO::PrepareCollectionItems()
{
   // a. Expand any shorthand card lines.
   // Treats ALL lines as addition lines.
   for( auto& szCardLine : m_ptLoadToken->CardLines )
   {
      m_ptCollection->m_ptrCollectionSource->ExpandAdditionLine( szCardLine );
   }

   // b. Create a list representing all the base cards.
   // THERE ARE NO METATAGS HERE.
   for( auto& szCardLine : m_ptLoadToken->CardLines )
   {
      CollectionObject::PseudoIdentifier sudoItem;
      if( CollectionObject::ParseCardLine( szCardLine, sudoItem ) )
      {
         for( size_t i = 0; i < sudoItem.Count; i++ )
         {
            loadPotentialItem( sudoItem.Name, sudoItem.Identifiers, sudoItem.MetaTags );
         }
      }
      else
      {
         // TODO: Log ERROR
      }
   }

   // c. Load Meta Tag lines
   loadMetaTagFile();

   // d. Bind meta tags to base cards
   for( auto& szLine : m_ptLoadToken->MetaTagLines )
   {
      bindMetaTagLine( szLine );
   }

   return true;
}

bool 
CollectionIO::PopulateCollectionFields()
{
   m_ptCollection->m_ptrCollectionDetails->SetName( m_ptLoadToken->CollectionName );
   m_ptCollection->m_ptrCollectionDetails->SetProcessLines( m_ptLoadToken->OverheadProcessLines );
   m_ptCollection->m_ptrCollectionDetails->SetFileName( m_ptLoadToken->CollectionFileName );

   m_ptCollection->m_ptrCollectionDetails->SetInitialized( true );

   for( auto& szPropertLine : m_ptLoadToken->OverheadPropertyLines )
   {
      loadOverheadLine( szPropertLine );
   }

   return true;
}

bool 
CollectionIO::ConsolodateItems()
{
   auto ptSource = m_ptCollection->m_ptrCollectionSource;
   Location newColAddr( m_ptLoadToken->CollectionAddress );

   size_t uiTokenIndex = 0;
   m_ptLoadToken->ItemLoadTokens.resize( m_ptLoadToken->CardItems.size() );

   for( auto& item_copy_pair : m_ptLoadToken->CardItems )
   {
      // Get all the local items already loaded
      // Items should be good at this point.
      auto item = ptSource->GetCardPrototype( item_copy_pair.first ).Value();

      // This token will be used to store all the items that fall into the cases below.
      auto& itemLoadToken = m_ptLoadToken->ItemLoadTokens.at( uiTokenIndex++ );
      itemLoadToken.Item = item;

      auto& vecNewCopies = item_copy_pair.second;

      // Handle All Copies that were specified in the collection file.
      // aka pt_newly_loaded_copy
      for( auto& ptNewCopy : vecNewCopies )
      {
         // See if another collection loaded this item already.
         auto tgExistingCopy = item->FindCopy( ptNewCopy->GetUID() );
         if( tgExistingCopy.Good() )
         {
            auto ptExistingCopy = *tgExistingCopy.Value();

            // Some other collection loaded this item already.
            // See if the already loaded item was changed more
            // recently than the newly loaded item.
            if( isAOlderThanB( ptNewCopy, ptExistingCopy ) )
            {
               // The newcopy is from an older session, use the already item.

               // Prepare the existing copy to take the details of the newly loaded copy.
               itemLoadToken.NewerExistingItems.insert( make_pair (ptNewCopy, ptExistingCopy ));
            }
            else
            {
               // Prepare the new copy to replace the already loaded one.
               transferCopyFirstToSecond( ptExistingCopy, ptNewCopy );

               // Add to outdated items for removal.
               itemLoadToken.ReplacedByNewlyLoadedItems.push_back( ptExistingCopy );
               itemLoadToken.ConfirmedCopyItems.push_back( ptNewCopy );
            }
         }
         else if( !ptNewCopy->IsParent(newColAddr) )
         {
            // Special case if its a borrowed copy not already loaded.
            // Check if its parent is loaded.
            // Check if any of those collections are more recently edited than 
            // this collection.
            // If not, this item was added while those collections were not loaded.
            // If any of the other collections that this item claims to be in are newer
            // than this copy, but dont have this copy, then this copy was deleted.
            bool bDoes_Not_Exist_In_More_Recently_Editted_Collection = false;
            auto vecPossibleParents = m_ptFactory->GetCollectionFamily( ptNewCopy->GetAddress() );
            for( auto& colParent : vecPossibleParents )
            {
               string szOtherColSession = to_string( colParent->m_ptrCollectionDetails->GetTimeStamp() );
               if( isAOlderThanB( ptNewCopy->GetSession(), szOtherColSession ) )
               {
                  bDoes_Not_Exist_In_More_Recently_Editted_Collection = true;
                  break;
               }
            }

            if( !bDoes_Not_Exist_In_More_Recently_Editted_Collection )
            {
               // Use this item.
               itemLoadToken.ConfirmedCopyItems.push_back( ptNewCopy );
            }
         }
         else
         {
            // Use this item.
            itemLoadToken.ConfirmedCopyItems.push_back( ptNewCopy );
         }
      }

      // Handle all copies that were not specified in the collection file
      // but say there are in this collection.
      auto vecCopiesReferencingThisCol = item->FindCopies( newColAddr, CollectionObjectType::Local );
      for( auto& ptAlreadyLoadedCopy : vecCopiesReferencingThisCol )
      {
         // Only keep these copies if they are more recently editted than this collection
         if( isAOlderThanB( m_ptLoadToken->CollectionSessionTime,
                            ptAlreadyLoadedCopy->GetSession() )    )
         {
            itemLoadToken.ItemsLikelyAddedWhileUnloaded.push_back( ptAlreadyLoadedCopy );
         }
         else
         {
            itemLoadToken.OutDatedRemovedItems.push_back( ptAlreadyLoadedCopy );
         }
      }
   }

   // Handle each of the cases.
   // This officially loads all of the collection items.
   for( auto& loadedItem : m_ptLoadToken->ItemLoadTokens )
   {
      auto& item = loadedItem.Item;

      // Handle newly loaded copies.
      for( auto& confirmedCopy : loadedItem.ConfirmedCopyItems )
      {
         item->AddCopy( confirmedCopy );
      }

      for( auto& pairTransferTo : loadedItem.NewerExistingItems )
      {
         auto copyNeedingTransfer = pairTransferTo.first;
         auto targetCopy = pairTransferTo.second;
         transferCopyFirstToSecond( copyNeedingTransfer, targetCopy );
      }

      // Handle pre-loaded copies.
      for( auto& outDatedCopy : loadedItem.ReplacedByNewlyLoadedItems )
      {
         // Outdated items are
         item->DeleteCopy( outDatedCopy );
      }
      
      for( auto& outDatedRemovedCopy : loadedItem.OutDatedRemovedItems )
      {
         // TODO: JUST MAKE IT VIRTUAL.
         item->DeleteCopy( outDatedRemovedCopy );
      }

      for( auto& likelyNewCopy : loadedItem.ItemsLikelyAddedWhileUnloaded )
      {
         likelyNewCopy->AddResident( newColAddr );
      }
   }

   return true;
}

bool 
CollectionIO::FinalizeLoad()
{
   bool bIsLoaded = (m_ptCollection->GetIdentifier().GetMain() != "");
   m_ptCollection->m_ptrCollectionDetails->SetIsLoaded( bIsLoaded );

   if( bIsLoaded )
   {
      m_ptCollection->m_ptrCollectionTracker->Track();

      if( m_ptCollection->GetName() == "" )
      {
         m_ptCollection->m_ptrCollectionDetails->SetName( m_ptLoadToken->CollectionFileName );
      }
   }
   return true;
}

bool 
CollectionIO::loadOverheadFile()
{
   // Requres the collection file name.
   vector<string> vecCollectionPropertyLines;

   // Get the overhead file name.
   string szFileName = m_ptLoadToken->CollectionFileName;
   szFileName = GetOverheadFile( szFileName );

   // Get the lines from the file for individual processing.
   GetFileLines( szFileName, vecCollectionPropertyLines );

   // a. Store the properties indicated in the overhead file.
   // b. Store off the processing (like post-load actions) lines in the ovh file.
   for( auto& szLine : vecCollectionPropertyLines )
   {
      if( StringInterface::IsCollectionOverheadPropertyLine( szLine ) )
      {
         m_ptLoadToken->OverheadPropertyLines.push_back( szLine );
         storeOverheadProperyLineValue( szLine );
      }
      else
      {
         m_ptLoadToken->OverheadProcessLines.push_back( szLine );
      }
   }

   return true;
}

bool 
CollectionIO::loadOverheadLine(const std::string& aszLine)
{
   auto ptDetails = m_ptCollection->m_ptrCollectionDetails;

   string szBaseLine = aszLine.substr( 2 );
   vector<string> lstSplitLine = StringHelper::Str_Split( szBaseLine, "=" );

   if( lstSplitLine.size() != 2 ) { return false; }

   vector<string>::iterator iter_Lines = lstSplitLine.begin();
   for( ; iter_Lines != lstSplitLine.end(); ++iter_Lines )
   {
      *iter_Lines = StringHelper::Str_Trim( *iter_Lines, ' ' );
   }

   string szKey = lstSplitLine.at( 0 );
   string szValue = lstSplitLine.at( 1 );
   szValue = StringHelper::Str_Trim( szValue, '\"' );

   if( szKey == "Name" )
   {
      ptDetails->SetName( szValue );
   }
   else if( szKey == "ID" )
   {
      ptDetails->AssignAddress( szValue );

      if( m_ptCollection->m_ptrCollectionLedger->GetLocation() != *ptDetails->GetAddress() )
      {
         delete m_ptCollection->m_ptrCollectionLedger;
         m_ptCollection->m_ptrCollectionLedger = new CollectionLedger( *ptDetails->GetAddress() );
      }
   }
   else if( szKey == "CC" )
   {
      ptDetails->SetChildrenCount( stoi( szValue ) );
   }
   else if( szKey == "Session" )
   {
      tm tm{};
      istringstream str_stream( szValue );
      str_stream >> get_time( &tm, "%Y-%m-%d_%T" );
      time_t time = mktime( &tm );
      ptDetails->SetTimeStamp( time );
   }
   
   return true;
}

bool 
CollectionIO::storeOverheadProperyLineValue(const std::string& aszLine)
{
   string szBaseLine = aszLine.substr( string(Config::CollectionDefinitionKey).size() );

   vector<string> lstSplitLine = StringHelper::Str_Split( szBaseLine, "=" );
   if( lstSplitLine.size() != 2 ) { return false; }

   for( auto& szLine : lstSplitLine )
   {
      szLine = StringHelper::Str_Trim( szLine, ' ' );
   }

   string szKey = lstSplitLine.at( 0 );
   string szValue = lstSplitLine.at( 1 );
   szValue = StringHelper::Str_Trim( szValue, '\"' );

   if( szKey == "Name" )
   {
      m_ptLoadToken->CollectionName = szValue;
   }
   else if( szKey == "ID" )
   {
      m_ptLoadToken->CollectionAddress = szValue;
   }
   else if( szKey == "CC" )
   {
      m_ptLoadToken->CollectionChildCount = szValue;
   }
   else if( szKey == "Session" )
   {
      m_ptLoadToken->CollectionSessionTime = szValue;
   }

   return true;
}

bool 
CollectionIO::loadPotentialItem( const string& aszName,
                                 const vector<Tag>& alstAttrs,
                                 const vector<Tag>& alstMetaTags )
{
   auto ptSource = m_ptCollection->m_ptrCollectionSource;
   auto item = ptSource->GetCardPrototype( aszName );

   if( item.Good() )
   {
      auto cItem = item->CreateCopy( Address(m_ptLoadToken->CollectionAddress),
                                     alstAttrs, alstMetaTags );

      m_ptLoadToken->CardItems[aszName].push_back( cItem );
      return true;
   }
   else
   {
      // TODO: ERROR
      return false;
   }
}

bool 
CollectionIO::loadMetaTagFile()
{
   auto szFileName = GetMetaFile( m_ptLoadToken->CollectionFileName );
   GetFileLines( szFileName, m_ptLoadToken->MetaTagLines );
   return true;
}

bool 
CollectionIO::bindMetaTagLine( const std::string& aszLine )
{
   auto ptSource = m_ptCollection->m_ptrCollectionSource;

   CollectionObject::PseudoIdentifier sudoItem;
   CollectionObject::ParseCardLine( aszLine, sudoItem );

   vector<Tag> vecMetaTags = sudoItem.MetaTags;

   // Clear the meta so the hash may be obtained.
   // ( We are looking for a blank card with a matching hash )
   sudoItem.MetaString = "";
   sudoItem.MetaTags.clear();

   auto item = ptSource->GetCardPrototype( sudoItem.Name );
   if( item.Good() )
   {
      auto szPlainHash = item->GenerateHash( Address( m_ptLoadToken->CollectionAddress ),
                                             sudoItem.Identifiers,
                                             vecMetaTags );
      for( size_t i = 0; i < sudoItem.Count; i++ )
      {
         auto matchingCopy = item->FindCopy( szPlainHash, Hash,
                                             m_ptLoadToken->CardItems[sudoItem.Name] );

         if( matchingCopy.Good() )
         {
            // Bind the tags.
            for( auto& Tag : vecMetaTags )
            {
               (*matchingCopy.Value())->SetMetaTag( Tag.first, Tag.second,
                                                    CopyItem::DetermineMetaTagType( Tag.first ),
                                                    false );
            }
         }
         else
         {
            // There should really be no dangling tags.. TODO: ERROR
         }
      }
   }
   else
   {
      // TODO: ERROR
   }

   return true;
}

bool 
CollectionIO::isAOlderThanB( const std::shared_ptr<CopyItem>& aA, const std::shared_ptr<CopyItem>& aB )
{
   string szSessionNew = aB->GetSession();
   string szSessionOld = aA->GetSession();

   return isAOlderThanB(szSessionOld, szSessionNew);
}

bool 
CollectionIO::isAOlderThanB( const std::string& aA, const std::string& aB )
{
   // If there is a match, check the Session to see which one is newer.
   unsigned long lSessNew = stoul( aB, nullptr, 16 );
   unsigned long lSessOld = stoul( aA, nullptr, 16 );

   return lSessOld <= lSessNew;
}


bool 
CollectionIO::transferCopyFirstToSecond( const std::shared_ptr<CopyItem>& aA,
                                         const std::shared_ptr<CopyItem>& aB )
{
   for( auto& aAResis : aA->GetResidentIn() )
   {
      aB->AddResident( aAResis );
   }

   return true;
}


bool CollectionIO::CollectionFileExists( string aszFileName )
{
   string szFullFileName = GetCollectionFile( aszFileName );
   ifstream f( szFullFileName.c_str() );
   bool bRetVal = f.good();
   f.close();
   return bRetVal;
}

std::string
CollectionIO::StripFileName( std::string aszFilePath )
{
   auto lstSplitFile = StringHelper::Str_Split( aszFilePath, "\\" );
   auto szFile = lstSplitFile[lstSplitFile.size() - 1];
   auto lstSplitExt = StringHelper::Str_Split( szFile, "." );
   szFile = lstSplitExt[0];

   return szFile;
}

string CollectionIO::GetCollectionFile( string aszCollectionName )
{
   return Config::Instance()->GetCollectionsDirectory() + "\\" +
      StringHelper::Str_Replace( aszCollectionName, ' ', '_' ) + ".txt";
}

string CollectionIO::GetMetaFile( string aszCollectionName )
{
   Config* config = Config::Instance();
   return config->GetCollectionsMetaDirectory() + "\\" +
      StringHelper::Str_Replace( aszCollectionName, ' ', '_' ) + "." +
      string( Config::MetaFileExtension ) + ".txt";
}

string CollectionIO::GetHistoryFile( string aszCollectionName )
{
   Config* config = Config::Instance();
   return config->GetCollectionsHistoryDirectory() + "\\" +
      StringHelper::Str_Replace( aszCollectionName, ' ', '_' ) + "." +
      string( Config::HistoryFileExtension ) + ".txt";
}

std::string
CollectionIO::GetOverheadFile( std::string aszCollectionName )
{
   Config* config = Config::Instance();
   return config->GetCollectionsOverheadDirectory() + "\\" +
      StringHelper::Str_Replace( aszCollectionName, ' ', '_' ) + "." +
      string( Config::OverheadFileExtension ) + ".txt";
}


bool 
CollectionIO::GetFileLines( string aszFileName,
                            vector<string>& rlstFileLines )
{
   // Load the collection
   ifstream in(aszFileName);
   if( !in.good() )
   {
      return false;
   }

   stringstream buffer;
   buffer << in.rdbuf();
   in.close();
   string contents(buffer.str());

   rlstFileLines = StringHelper::SplitIntoLines(contents);
   return true;
}

// Returns the name of a collection and all the lines in it.
bool 
CollectionIO::GetNameAndCollectionLines( vector<string>  alstAllLines, 
                                         string&         rlstIOLines,
                                         vector<string>& rlstCollectionLines )
{
   // There must be one line with quotations. (Exactly One)
   // This is the name. All others will be interpreted as cards or thrown away.
   vector<string>::iterator iter_Lines = alstAllLines.begin();
   for (; iter_Lines != alstAllLines.end(); ++iter_Lines)
   {
      int iOne = iter_Lines->find_first_of('\"');
      int iTwo =  iter_Lines->find_last_of('\"');
      if( iOne != iTwo && iOne == 0 )
      {
         rlstIOLines = iter_Lines->substr(iOne+1, iTwo-iOne-1);
      }
      else
      {
         rlstCollectionLines.push_back(*iter_Lines);
      }
   }

   return true;
}
