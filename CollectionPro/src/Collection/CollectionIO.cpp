#include "CollectionIO.h"
#include "Collection.h"
#include "CollectionFactory.h"
#include "CollectionLedger.h"
#include "CopyItem.h"
#include "../StringInterface.h"
#include "../Addressing/Addresser.h"

#include <algorithm>

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

void 
CollectionIO::SaveCollection()
{
   saveCollection();

   saveMeta();

   saveHistory();

   saveOverhead();
}

void 
CollectionIO::ExportCollection( Query aQuery )
{
   exportCollection( aQuery );
}

map<unsigned long, vector<string>> 
CollectionIO::GetHistoryTransactions( unsigned int aiStart, unsigned int aiEnd )
{
   auto ptCollectionDetails = m_ptCollection->m_ptrCollectionDetails;

   map<unsigned long, vector<string>> mapRetVal;

   string szHistFile = GetHistoryFile( ptCollectionDetails->GetFileName() );

   ifstream oHistFile( szHistFile, std::ios::in );
   unsigned int iLookingAtTrans = 0;

   // Find the first
   bool bIsDateline = false;
   map<unsigned long, vector<string>>::iterator iter_current = mapRetVal.end();
   while( iLookingAtTrans < aiEnd )
   {
      bIsDateline = false;

      string szLine;
      if( !std::getline( oHistFile, szLine ) )
      {
         break;
      }

      size_t iOpenDate = szLine.find_first_of( '[' );
      size_t iCloseDate = szLine.find_first_of( ']' );
      if( iOpenDate >= 0 && iCloseDate > iOpenDate )
      {
         iLookingAtTrans++;
         bIsDateline = true;
      }

      if( iLookingAtTrans >= aiStart )
      {
         if( bIsDateline )
         {
            string szTimeParse = szLine.substr( iOpenDate + 1, iCloseDate - iOpenDate - 1 );
            auto time = StringInterface::ToTimeValue( szTimeParse, "%a %b %d %H:%M:%S %Y" );

            auto iter_mapIns = mapRetVal.insert( make_pair( time, vector<string>() ) );
            if( iter_mapIns.second )
            {
               iter_current = iter_mapIns.first;
            }
            else
            {
               break;
            }
         }
         else if( iter_current != mapRetVal.end() )
         {
            string szTrimLine = StringHelper::Str_Trim( szLine, ' ' );
            if( szTrimLine != "" )
            {
               iter_current->second.push_back( szTrimLine );
            }
         }
      }
   }

   oHistFile.close();

   return mapRetVal;
}

bool 
CollectionIO::PrepareCollectionInitialization( const string& aszFileName )
{
   bool bRetVal = true;

   // a. Save the file name.
   m_ptLoadToken->CollectionFilePath = aszFileName;
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
   m_ptCollection->m_ptrCollectionDetails->SetFilePath( m_ptLoadToken->CollectionFilePath );

   // setting the filepath sets this.
   //m_ptCollection->m_ptrCollectionDetails->SetFileName( m_ptLoadToken->CollectionFileName );

   m_ptCollection->m_ptrCollectionDetails->SetInitialized( true );

   for( auto& szPropertLine : m_ptLoadToken->OverheadPropertyLines )
   {
      loadOverheadPropertyLine( szPropertLine );
   }

   for( auto& szOverheadLine : m_ptLoadToken->OverheadProcessLines )
   {
      loadOverheadProcessLine( szOverheadLine );
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
            auto ptExistingCopy = tgExistingCopy.Value();

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
         else if( !ptNewCopy->GetAddress().IsSuperset(newColAddr) )
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
      // TODO: Only local
      auto vecCopiesReferencingThisCol = item->FindCopies( newColAddr );
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

      // Handle transfer from newly loaded copies to pre-loaded copies.
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

   // TODO: Do this for the other special values.
   if( m_ptLoadToken->CollectionAddress == "" )
   {
      m_ptLoadToken->CollectionAddress = m_ptCollection->GetIdentifier().ToString();
   }

   return true;
}

bool 
CollectionIO::loadOverheadProcessLine( const std::string& aszLine )
{
   auto ptDetails = m_ptCollection->m_ptrCollectionDetails;

   string szBaseLine = aszLine;
   vector<string> lstSplitLine = StringHelper::Str_Split( szBaseLine, " " );

   size_t iWords = lstSplitLine.size();
   if( iWords < 1 ) { return false; }

   for( auto& szLine : lstSplitLine )
   {
      szLine = StringHelper::Str_Trim( szLine, ' ' );
   }

   // TODO: This should not be literall.
   if( iWords > 2 && lstSplitLine[0] == "Peek" )
   {
      size_t indOfValueStart = 0;
      for( size_t i = 1; i < iWords; i++ )
      {
         auto szWord = lstSplitLine[i];
         if( szWord.size() > 0 && szWord[0] == '\"' )
         {
            indOfValueStart = i;
            break;
         }
      }

      string szKey = "";
      for( size_t i = 1; i < indOfValueStart; i++ )
      {
         auto szWord = lstSplitLine[i];
         szKey += " "+szWord;
      }
      szKey = StringHelper::Str_Trim( szKey, ' ' );

      string szValue = "";
      for( size_t i = indOfValueStart; i < iWords; i++ )
      {
         auto szWord = lstSplitLine[i];
         szValue += " "+szWord;
      }
      szValue = StringHelper::Str_Trim( szValue, ' ' );
      szValue = StringHelper::Str_Trim( szValue, '\"' );

      ptDetails->AddPeekValue( szKey, szValue );
   }

   return true;
}

bool 
CollectionIO::loadOverheadPropertyLine(const std::string& aszLine)
{
   auto ptDetails = m_ptCollection->m_ptrCollectionDetails;

   string szBaseLine = aszLine.substr( 2 );
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
      ptDetails->SetName( szValue );
   }
   else if( szKey == "ID" )
   {
      ptDetails->AssignAddress( szValue );

      // TODO: This might be weird. Maybe 'details' should be allowed to interact with
      // multiple systems.
      if( m_ptCollection->m_ptrCollectionLedger->GetLocation().Compare( *ptDetails->GetAddress()) == -2 )
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
      auto time = StringInterface::ToTimeValue( szValue, "%Y-%m-%d_%T" );
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
      auto cItem = item->CreateCopy( Location(m_ptLoadToken->CollectionAddress),
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
      auto szPlainHash = item->GenerateHash( Location( m_ptLoadToken->CollectionAddress ),
                                             sudoItem.Identifiers,
                                             vector<Tag>() );
      for( size_t i = 0; i < sudoItem.Count; i++ )
      {
         // Look for a copy with a matchin hash
         TryGetCopy<std::shared_ptr<CopyItem>> matchingCopy;
         for( auto& hashCopy : m_ptLoadToken->CardItems[sudoItem.Name] )
         {
            if( hashCopy->GetHash() == szPlainHash )
            {
               matchingCopy.Set( hashCopy );
               break;
            }
         }

         if( matchingCopy.Good() )
         {
            // Bind the tags.
            for( auto& Tag : vecMetaTags )
            {
               matchingCopy.Value()->SetMetaTag( Tag.first, Tag.second,
                                                 MetaTag::DetermineMetaTagType( Tag.first ) );
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
   // TODO:
   //for( auto& aAResis : aA->GetResidentIn() )
   //{
   //   aB->AddResident( aAResis );
   //}

   return true;
}


void
CollectionIO::saveHistory()
{
   auto ptCollectionDetails = m_ptCollection->m_ptrCollectionDetails;
   auto ptCollectionTracker = m_ptCollection->m_ptrCollectionTracker;

   ptCollectionTracker->Track();
   CollectionDeltaClass cdc = ptCollectionTracker->CalculateChanges();

   vector<string> lstHistoryLines;
   for( size_t i = 0; i < cdc.Additions.size(); i++ )
   {
      lstHistoryLines.push_back( cdc.Additions[i] );
   }

   for( size_t i = 0; i < cdc.Removals.size(); i++ )
   {
      lstHistoryLines.push_back( cdc.Removals[i] );
   }

   for( size_t i = 0; i < cdc.Changes.size(); i++ )
   {
      lstHistoryLines.push_back( cdc.Changes[i] );
   }

   if( lstHistoryLines.size() > 0 )
   {
      string szTime = StringInterface::ToTimeString( StringInterface::GetCurrentTimeCount() );

      string szHistFile = GetHistoryFile( ptCollectionDetails->GetFileName() );

      // If a history file already exists, rename it.
      string szHistFileTmp = szHistFile + ".tmp";
      std::rename( szHistFile.c_str(), szHistFileTmp.c_str() );

      ofstream oHistFile( szHistFile, std::ios::out );
      if( oHistFile.good() )
      {
         // Add the new entry at the top.
         oHistFile << "[" << szTime << "] " << endl;

         vector<string>::iterator iter_histLines = lstHistoryLines.begin();
         for( ; iter_histLines != lstHistoryLines.end(); ++iter_histLines )
         {
            oHistFile << *iter_histLines << endl;
         }
      }

      // Append the old.
      ifstream oHistFileOld( szHistFileTmp, std::ios::in );
      if( oHistFileOld.good() )
      {
         oHistFile << oHistFileOld.rdbuf();
      }

      oHistFile.close();
      oHistFileOld.close();

      // Delete the tmp.
      std::remove( szHistFileTmp.c_str() );
   }
}

void 
CollectionIO::saveMeta()
{
   auto ptCollectionDetails = m_ptCollection->m_ptrCollectionDetails;

   ofstream oMetaFile;
   Query listQuery( true );
   listQuery.TrackingMeta();
   vector<string> lstMetaLines = m_ptCollection->QueryCollection( listQuery );

   oMetaFile.open( GetMetaFile( ptCollectionDetails->GetFileName() ) );

   vector<string>::iterator iter_MetaLine = lstMetaLines.begin();
   for( ; iter_MetaLine != lstMetaLines.end(); ++iter_MetaLine )
   {
      if( iter_MetaLine->find_first_of( ':' ) != string::npos )
      {
         oMetaFile << *iter_MetaLine << endl;
      }
   }

   oMetaFile.close();
}

void
CollectionIO::saveOverhead()
{
   auto ptCollectionDetails = m_ptCollection->m_ptrCollectionDetails;

   ofstream oColFile;
   oColFile.open( GetOverheadFile( ptCollectionDetails->GetFileName() ) );

   // Populate the timestampe with the current time.
   ptCollectionDetails->SetTimeStamp();

   // Get the time in string format.
   string szTime = StringInterface::ToTimeString( ptCollectionDetails->GetTimeStamp(), "%F_%T" );

   oColFile << Config::CollectionDefinitionKey
      << " ID=\"" << m_ptCollection->GetIdentifier().ToString() << "\"" << endl;

   oColFile << Config::CollectionDefinitionKey
      << " CC=\"" << ptCollectionDetails->GetChildrenCount() << "\"" << endl;

   oColFile << Config::CollectionDefinitionKey
      << " Session=\"" << szTime << "\"" << endl;

   for( auto szLine : ptCollectionDetails->GetProcessLines() )
   {
      oColFile << szLine << endl;
   }

   saveRequiredPeekValues(oColFile);

   oColFile.close();
}

void 
CollectionIO::saveRequiredPeekValues( ofstream& aFile )
{
   auto ptDetails = m_ptCollection->m_ptrCollectionDetails;
   auto ptLedger = m_ptCollection->m_ptrCollectionLedger;

   auto vecPeek = ptDetails->GetPeekValues();
   map<string, string> mapPeek( vecPeek.begin(), vecPeek.end() );
   
   auto iter_find = mapPeek.find( "Count" );
   if( iter_find == mapPeek.end() )
   {
      aFile << "Peek " << "Count \"" << ptLedger->GetSize() << "\"" << endl;
   }

   iter_find = mapPeek.find( "Id" );
   if( iter_find == mapPeek.end() )
   {
      aFile << "Peek " << "Id \"" << ptDetails->GetAddress()->ToString() << "\"" << endl;
   }

   iter_find = mapPeek.find( "Icon" );
   if( iter_find == mapPeek.end() )
   {
      auto colItems = ptLedger->ViewPresent();
      if( colItems.size() > 0 )
      {
         aFile << "Peek " << "Icon \"" << (*colItems.begin())->GetObject()->GetName() << "\"" << endl;
      }
   }
}

void 
CollectionIO::saveCollection()
{
   // Group lists only by id. When loading, these lists are only
   // used to create a template card. We only need the base details.
   Query listQuery( true );
   listQuery.Short();
   listQuery.IncludeMetaType( MetaTag::None );
   // listQuery.HashType( CopyItem::HashType::Ids );
   
   exportCollection( listQuery );
}

void 
CollectionIO::exportCollection( Query aQuery )
{
   auto ptCollectionDetails = m_ptCollection->m_ptrCollectionDetails;

   vector<string> lstLines = m_ptCollection->QueryCollection( aQuery );

   ofstream oColFile;
   oColFile.open( ptCollectionDetails->GetFileDirectory() + ptCollectionDetails->GetName() + ".txt" );

   oColFile << "\"" << ptCollectionDetails->GetName() << "\"" << endl;

   for( auto& szLine : lstLines )
   {
      oColFile << szLine << endl;
   }

   oColFile.close();
}


bool 
CollectionIO::CollectionFileExists( string aszFileName )
{
   string szFullFileName = GetCollectionFile( aszFileName );
   ifstream f( szFullFileName.c_str() );
   bool bRetVal = f.good();
   f.close();
   return bRetVal;
}

string
CollectionIO::StripFileName( string aszFilePath )
{
   string tmp;
   return StripFileName(aszFilePath, tmp );
}

string
CollectionIO::StripFileName( string aszFilePath, string& rszDirectory )
{
   auto lstSplitFile = StringHelper::Str_Split( aszFilePath, "\\" );
   auto szFile = lstSplitFile[lstSplitFile.size() - 1];
   rszDirectory = aszFilePath.substr(0, aszFilePath.size() - szFile.size() );
   auto lstSplitExt = StringHelper::Str_Split( szFile, "." );
   szFile = lstSplitExt[0];
   return szFile;
}

string 
CollectionIO::GetCollectionFile( string aszCollectionName )
{
   return Config::Instance()->GetCollectionsDirectory() + "\\" +
      StringHelper::Str_Replace( aszCollectionName, ' ', '_' ) + ".txt";
}

string 
CollectionIO::GetMetaFile( string aszCollectionName )
{
   Config* config = Config::Instance();
   return config->GetCollectionsMetaDirectory() + "\\" +
      StringHelper::Str_Replace( aszCollectionName, ' ', '_' ) + "." +
      string( Config::MetaFileExtension ) + ".txt";
}

string 
CollectionIO::GetHistoryFile( string aszCollectionName )
{
   Config* config = Config::Instance();
   return config->GetCollectionsHistoryDirectory() + "\\" +
      StringHelper::Str_Replace( aszCollectionName, ' ', '_' ) + "." +
      string( Config::HistoryFileExtension ) + ".txt";
}

string
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
