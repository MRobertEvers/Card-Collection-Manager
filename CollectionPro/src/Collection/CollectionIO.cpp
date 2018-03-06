#include "CollectionIO.h"
#include "Collection.h"
#include "CollectionFactory.h"
#include "../Support/ListHelper.h"
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
   Location colAddr( m_ptLoadToken->CollectionAddress );

   // a. Consolodate local items.
   // Look for local items that may have already been loaded by other collections.
   for( auto& itemGroup : m_ptLoadToken->CardItems )
   {
      // Get all the local items already loaded
      // Items should be good at this point.
      auto item = ptSource->GetCardPrototype( itemGroup.first ).Value();

      auto& vecNewCopies = itemGroup.second;

      // Look for already existing items that match a newly loaded copy's UID.
      auto vecAlreadyLoadedCopies = item->FindCopies( colAddr, CollectionObjectType::Local );
      for( auto& alreadyCopy : vecAlreadyLoadedCopies )
      {
         auto newItem = item->FindCopy( alreadyCopy->GetUID(), FindType::UID, vecNewCopies );
         if( newItem.Good() )
         {
            auto newCopy = *newItem.Value();
            // We found a matching copy.
            if( isAOlderThanB( newCopy, alreadyCopy ) )
            {
               // The newcopy is from an older session, use the already item.
               transferCopyFirstToSecond( newCopy, alreadyCopy );
            }
            else
            {
               transferCopyFirstToSecond( alreadyCopy, newCopy );
               alreadyCopy = newCopy;
            }

            // Erase-remove idiom
            vecNewCopies.erase( remove( vecNewCopies.begin(), vecNewCopies.end(), newCopy ),
                                vecNewCopies.end() );
         }
         else
         {
            if( isAOlderThanB( m_ptLoadToken->CollectionSessionTime, alreadyCopy->GetSession() ) )
            {
               // Remove the old copy because this new collection is newer
               // TODO: REALLY I SHOULD MAKE THE COPY VIRTUAL.
               item->RemoveCopy( colAddr, alreadyCopy->GetUID() );
            }
            else
            {
               // Add this collection to its residence since this card has been added since
               // this col was last edited.
               alreadyCopy->AddResident( colAddr );
            }
         }
      }
   }

   // b. Consolodate borrowed items.
   for( auto& itemGroup : m_ptLoadToken->CardItems )
   {
      // Get all the local items already loaded
      // Items should be good at this point.
      auto item = ptSource->GetCardPrototype( itemGroup.first ).Value();

      auto& vecNewCopies = itemGroup.second;

      auto vecAlreadyBorrowedCopies = item->FindCopies( colAddr, CollectionObjectType::Borrowed );
      // Any copies in vecMatchedCopies will not be added at the end.
      vector<shared_ptr<CopyItem>> vecMatchedNewCopies;
      for( auto& newCopy : vecNewCopies )
      {
         auto alreadyItem = item->FindCopy( newCopy->GetUID(), FindType::UID );
         if( alreadyItem.Good() )
         {
            auto alreadyCopy = *alreadyItem.Value();
            if( isAOlderThanB( newCopy, alreadyCopy ) )
            {
               // The newcopy is from an older session, use the already item.
               transferCopyFirstToSecond( newCopy, alreadyCopy );
            }
            else
            {
               transferCopyFirstToSecond( alreadyCopy, newCopy );
               alreadyCopy = newCopy;
            }

            vecMatchedNewCopies.push_back( newCopy );
         }
         else if( m_ptFactory->CollectionExists( newCopy->GetParent() ) )
         {
            auto otherCol = m_ptFactory->GetCollection( newCopy->GetParent() ).Value();
            string szOtherColSession = to_string(otherCol->m_ptrCollectionDetails->GetTimeStamp());
            if( isAOlderThanB( szOtherColSession, newCopy->GetSession() ) )
            {
               // Keep this copy.
            }
            else
            {
               vecMatchedNewCopies.push_back( newCopy );
            }
         }
         else
         {
            // Always use this copy.
         }
      }

      for( auto& handledCopy : vecMatchedNewCopies )
      {
         // Erase-remove idiom
         vecNewCopies.erase( remove( vecNewCopies.begin(), vecNewCopies.end(), handledCopy ),
                             vecNewCopies.end() );
      }
   }

   // c. If there are unhandled copies. They are good. Add them.
   for( auto& itemGroup : m_ptLoadToken->CardItems )
   {
      // Get all the local items already loaded
      // Items should be good at this point.
      auto item = ptSource->GetCardPrototype( itemGroup.first ).Value();
      auto& vecNewCopies = itemGroup.second;

      for( auto& newCopy : vecNewCopies )
      {
         item->AddCopy( newCopy );
      }
   }

   return true;
}

bool 
CollectionIO::FinalizeLoad()
{
   // TODO: Put the below in the loader.
   m_ptCollection->IsLoaded = (m_ptCollection->GetIdentifier().GetMain() != "");

   if( m_ptCollection->IsLoaded )
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

   StringInterface stringIFace;
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
      if( stringIFace.IsCollectionOverheadPropertyLine( szLine ) )
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
   return f.good();
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
   return config->GetCollectionsDirectory() + "\\" +
      config->GetMetaFolderName() + "\\" +
      StringHelper::Str_Replace( aszCollectionName, ' ', '_' ) + "." +
      string( Config::MetaFileExtension ) + ".txt";
}

string CollectionIO::GetHistoryFile( string aszCollectionName )
{
   Config* config = Config::Instance();
   return config->GetCollectionsDirectory() + "\\" +
      config->GetHistoryFolderName() + "\\" +
      StringHelper::Str_Replace( aszCollectionName, ' ', '_' ) + "." +
      string( Config::HistoryFileExtension ) + ".txt";
}

std::string
CollectionIO::GetOverheadFile( std::string aszCollectionName )
{
   Config* config = Config::Instance();
   return config->GetCollectionsDirectory() + "\\" +
      config->GetHistoryFolderName() + "\\" +
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
