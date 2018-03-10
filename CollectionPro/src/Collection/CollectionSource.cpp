#include "..\stdafx.h"

#include "../Config.h"
#include "../Support/StringHelper.h"
#include "../StringInterface.h"
#include "CollectionSource.h"


#include <fstream>
#include <algorithm>
#include <iostream>
#include <sstream>

using namespace rapidxml;
using namespace std;

CollectionSource::CollectionSource()
{
   m_bIsLoaded = false;
   resetBuffer();
}

CollectionSource::~CollectionSource()
{
   m_vecCardCache.clear();
   m_vecCardDataBuffer.clear();

   delete[] m_AllCharBuff;
}

// The source file needs to be handled independently of this .exe.
void
CollectionSource::LoadLib( string aszFilePath )
{
   Config* config = Config::Instance();

   // Verify the file is good.
   ifstream file( aszFilePath );
   if( !file.good() )
   {
      // TODO Close collection.
      return Debug::Log( "CollectionSource",
         "Could not load library from " + aszFilePath );
   }

   stringstream buffer;
   buffer << file.rdbuf();
   file.close();

   // Prepare the document to be read.
   xml_document<> doc;
   string* textContent = new string( buffer.str() );
   doc.parse<0>( &textContent->front() );

   xml_node<> *xmlNode_CardDatabase = doc.first_node();
   xml_node<>* xmlNode_Cards = xmlNode_CardDatabase->first_node( "cards" );

   // Load the cards.
   xml_node<>* xmlNode_Card = xmlNode_Cards->first_node();
   while( xmlNode_Card != 0 )
   {
      loadCard( xmlNode_Card );
      xmlNode_Card = xmlNode_Card->next_sibling();
   }

   // Free up the captured memory.
   delete textContent;
   finalizeBuffer();

   m_bIsLoaded = true;
}

void 
CollectionSource::HotSwapLib( string aszFileName )
{
   vector<string> vecHoldCards;
   for( auto& card : m_vecCardCache )
   {
      vecHoldCards.push_back( card.GetName() );
   }

   resetBuffer();
   LoadLib( aszFileName );

   for( auto card : vecHoldCards )
   {
      LoadCard( card );
   }
}

int
CollectionSource::LoadCard( string aszCardName )
{
   Config* config = Config::Instance();
   string szCardName = StringHelper::Str_Trim( aszCardName, ' ' );

   // Since it is faster to look in the cache, check that first.
   int iCacheLocation = findInCache( szCardName );
   if( iCacheLocation == -1 )
   {
      // Look in the Source Object Buffer for a matching item.
      // If the card is found in the buffer, create a CollectionObject and cache it.
      int iFound = findInBuffer( szCardName );
      if( iFound != -1 )
      {
         iCacheLocation = loadCardToCache( iFound );
      }
   }

   return iCacheLocation;
}


TryGet<CollectionObject>
CollectionSource::GetCardPrototype( int aiCacheIndex )
{
   TryGet<CollectionObject> ColRetVal;
   if( aiCacheIndex < m_vecCardCache.size() )
   {
      ColRetVal.Set( &m_vecCardCache.at( aiCacheIndex ) );
   }

   return ColRetVal;
}

TryGet<CollectionObject>
CollectionSource::GetCardPrototype( string aszCardName )
{
   int iCacheIndex = LoadCard( aszCardName );
   return GetCardPrototype( iCacheIndex );
}

vector<int>
CollectionSource::GetCollectionCache( Location aAddrColID,
                                      CollectionObjectType aColItemType )
{
   vector<int> lstRetVal;

   for( size_t i = 0; i < m_vecCardCache.size(); i++ )
   {
      if( m_vecCardCache[i].FindCopies( aAddrColID, aColItemType ).size() > 0 )
      {
         lstRetVal.push_back( i );
      }
   }

   return lstRetVal;
}

vector<shared_ptr<CopyItem>>
CollectionSource::GetCollection( Location aAddrColID,
                                 CollectionObjectType aColItemType )
{
   vector<shared_ptr<CopyItem>> lstRetVal;

   for( auto& item : m_vecCardCache )
   {
      auto lstCopies = item.FindCopies( aAddrColID, aColItemType );

      auto iter_Copy = lstCopies.begin();
      for( ; iter_Copy != lstCopies.end(); ++iter_Copy )
      {
         lstRetVal.push_back( *iter_Copy );
      }
   }

   return lstRetVal;
}

// This is not case sensitive
vector<string>
CollectionSource::GetAllCardsStartingWith( const Query& aQuery )
{
   vector<string> lstCards;
   string szSearch = aQuery.GetSearch();
   transform( szSearch.begin(), szSearch.end(), szSearch.begin(), ::tolower );

   bool bActiveCache = szSearch.size() > 2;

   if( !bActiveCache )
   {
      m_lstSearchCache.clear();
   }

   // Check if the search is cached already
   if( bActiveCache )
   {
      int iEnd = m_lstSearchCache.size();
      for( int i = iEnd - 1; i >= 0; i-- )
      {
         auto pairICache = m_lstSearchCache[i];
         if( pairICache.first == szSearch )
         {
            break;
         }
         else if( szSearch.substr( 0, pairICache.first.size() ) == pairICache.first )
         {
            break;
         }
         else
         {
            m_lstSearchCache.pop_back();
         }
      }
   }

   // If there are still entries in the searchCache, and we are here, then we only need to search
   //  the sublist.
   vector<SourceObject>* lstSearchList;
   vector<SourceObject> lstCache;
   if( bActiveCache && m_lstSearchCache.size() > 0 )
   {
      lstSearchList = &m_lstSearchCache[m_lstSearchCache.size() - 1].second;
   }
   else
   {
      lstSearchList = &m_vecCardDataBuffer;
   }

   vector<string> lstStartCards;
   vector<string> lstOthers;

   vector<SourceObject>::iterator iter_Cards = lstSearchList->begin();
   for( ; iter_Cards != lstSearchList->end(); ++iter_Cards )
   {
      string szCard = iter_Cards->GetName( m_AllCharBuff );
      transform( szCard.begin(), szCard.end(), szCard.begin(), ::tolower );
      size_t iFindIndex = 0;
      iFindIndex = szCard.find( szSearch );
      if( iFindIndex != string::npos )
      {
         auto szStore = iter_Cards->GetName( m_AllCharBuff );
         vector<Tag> vecMeta;
         if( aQuery.GetUIDs() )
         {
            auto mapSets = iter_Cards->GetIDAttrRestrictions( m_AllCharBuff );

            // TODO: This should not be a literal string.
            auto iter_Set = mapSets.find( "set" );
            if( iter_Set != mapSets.end() )
            {
               for( auto szOpt : iter_Set->second )
               {
                  vecMeta.push_back( make_pair( "set", szOpt ) );
               }
            }
         }

         szStore = CollectionObject::ToCardLine( Address(), szStore,
            vector<Tag>(), vecMeta );
         if( iFindIndex == 0 )
         {
            lstStartCards.push_back( szStore );
         }
         else
         {
            lstOthers.push_back( szStore );
         }

         if( bActiveCache )
         {
            lstCache.push_back( *iter_Cards );
         }
      }
   }

   for( string sz : lstStartCards )
   {
      lstCards.push_back( sz );
   }
   for( string sz : lstOthers )
   {
      lstCards.push_back( sz );
   }

   if( bActiveCache )
   {
      m_lstSearchCache.push_back( make_pair( szSearch, lstCache ) );
   }

   return lstCards;
}


void
CollectionSource::ClearCache()
{
   m_vecCardCache.clear();
}

bool
CollectionSource::IsLoaded()
{
   return m_bIsLoaded;
}

void
CollectionSource::CollapseCardLine( string& rszCard, bool abIncludeCount )
{
   Config* config = Config::Instance();
   string szFirstKey = "";
   vector<Tag> vecPairedKeys;
   vector<string> vecIdentifyingKeys;
   vector<TraitItem> vecPrototypeTraits;
   CollectionObject::PseudoIdentifier oParser;

   bool bGoodParse = CollectionObject::ParseCardLine( rszCard, oParser );
   if( bGoodParse )
   {
      // Get which traits are paired from the config.
      vecIdentifyingKeys = config->GetIdentifyingAttributes();
      vecPairedKeys = config->GetPairedKeysList();

      vector<string> vecKeepKeys;
      vector<string> vecRemoveKeys;
      for( auto keyPair : vecPairedKeys )
      {
         auto iter_find = find( vecKeepKeys.begin(), vecKeepKeys.end(), keyPair.first );
         int iHaveFirst = distance( vecKeepKeys.begin(), iter_find );

         iter_find = find( vecRemoveKeys.begin(), vecRemoveKeys.end(), keyPair.first );
         int iIgnoreFirst = distance( vecRemoveKeys.begin(), iter_find );

         iter_find = find( vecKeepKeys.begin(), vecKeepKeys.end(), keyPair.second );
         int iHaveSecond = distance( vecKeepKeys.begin(), iter_find );

         iter_find = find( vecRemoveKeys.begin(), vecRemoveKeys.end(), keyPair.second );
         int iIgnoreSecond = distance( vecRemoveKeys.begin(), iter_find );

         // We already have this key.
         // Remove its pair.
         if( iHaveFirst != -1 || iIgnoreFirst != -1 )
         {
            if( iIgnoreSecond == -1 && iHaveSecond == -1 )
            {
               vecRemoveKeys.push_back( keyPair.second );
               continue;
            }
         }

         if( iHaveSecond != -1 || iIgnoreSecond != -1 )
         {
            if( iIgnoreFirst == -1 && iHaveFirst == -1 )
            {
               vecKeepKeys.push_back( keyPair.first );
               continue;
            }
         }

         vecKeepKeys.push_back( keyPair.first );
         vecRemoveKeys.push_back( keyPair.second );
      }

      for( auto szRemoveKey : vecRemoveKeys )
      {
         auto iter_find = find( vecIdentifyingKeys.begin(), vecIdentifyingKeys.end(), szRemoveKey );
         vecIdentifyingKeys.erase( iter_find );
      }

      // Now get the trait value for each of the remaining keys.
      // A list of each key needed to identify a card.
      // Use the first of a pair for paired keys.
      vector<string> vecImportantValues;
      auto oCard = GetCardPrototype( oParser.Name );
      if( oCard.Good() )
      {
         for( auto keepKey : vecIdentifyingKeys )
         {
            string szFoundValue = StringInterface::FindTagInList( oParser.Identifiers, keepKey );

            if( szFoundValue != "" )
            {
               vecImportantValues.push_back( szFoundValue );
            }
         }
      }

      // Now collapse the values.
      string szShort;
      if( abIncludeCount )
      {
         szShort += "x" + to_string( oParser.Count ) + " ";
      }

      szShort += oParser.Name;

      if( vecImportantValues.size() > 0 )
      {
         szShort += " ";
         szShort += "[";
         for( auto szVal : vecImportantValues )
         {
            szShort += szVal;
            szShort += ",";
         }
         szShort = szShort.substr( 0, szShort.size() - 1 );
         szShort += "]";
      }

      rszCard = szShort;
   }
}

// Modifies the string to the long hand version if it is a shorthand,
// otherwise, this does nothing.
// Takes the form Name [id]
// Can also take the form Name [id1=val1,id2=val2]
// Can also take the form Name [val1,val2]
// TODO: move this to the collection source.
void
CollectionSource::ExpandAdditionLine( string& rszLine )
{
   string szKey, szDetails, szName, szId, szCount;

   int iDetEnd = rszLine.find_first_of( ']' );
   int iNameStart = rszLine.find_first_of( ' ' );
   if( iNameStart == string::npos )
   {
      iNameStart = 0;
   }

   if( iDetEnd == rszLine.size() - 1 )
   {
      int iDetStart = rszLine.find_first_of( '[' );
      szCount = rszLine.substr( 0, iNameStart );
      szName = rszLine.substr( iNameStart, iDetStart - iNameStart );
      szName = StringHelper::Str_Trim( szName, ' ' );
      szId = rszLine.substr( iDetStart + 1, rszLine.size() - iDetStart - 2 );
      auto card = GetCardPrototype( szName );

      auto vecVals = StringHelper::Str_Split( szId, "," );

      // Find the key for each unpair value.
      for( auto& szPair : vecVals )
      {
         bool bGoodVal = false;

         // Check if the dets are solo.
         bool bNeedMatch = szId.find_first_of( '=' ) == string::npos;
         if( bNeedMatch )
         {
            if( card.Good() )
            {
               bGoodVal = card->MatchIdentifyingTrait( szPair, szKey );
            }

            if( bGoodVal )
            {
               szPair = szKey + "=" + szPair;
            }
         }
      }

      // Now we have to put quotes around each of the "values"
      for( auto& szPair : vecVals )
      {
         auto vecSplitPair = StringHelper::Str_Split( szPair, "=" );
         if( vecSplitPair.size() == 2 )
         {
            string szKeyVal = vecSplitPair[0];
            string szValVal = vecSplitPair[1];

            // Make sure we dont double down on "
            szValVal = StringHelper::Str_Trim( szValVal, '"' );
            szPair = szKeyVal + "=\"" + szValVal + "\"";
         }
      }

      // Wrap szID with { }
      szDetails = "{ ";
      for( auto& szPair : vecVals )
      {
         szDetails += szPair;
         szDetails += " ";
      }
      szDetails += "}";

      rszLine = szCount + " " + szName + " " + szDetails;
   }
}

void
CollectionSource::loadCard( rapidxml::xml_node<char> * xmlNode_Card )
{
   Config* config = Config::Instance();
   SourceObject* sO = getNewSourceObject();

   xml_node<> *xmlNode_CardAttribute = xmlNode_Card->first_node();
   while( xmlNode_CardAttribute != 0 )
   {
      string szCardKey = xmlNode_CardAttribute->name();
      char keyCode = config->GetKeyCode( szCardKey );
      if( keyCode != -1 )
      {
         m_iAllCharBuffSize += sO->AddAttribute( szCardKey, xmlNode_CardAttribute->value(),
                                                 m_AllCharBuff, ms_iMaxBufferSize );
      }

      xmlNode_CardAttribute = xmlNode_CardAttribute->next_sibling();
   }
}

int
CollectionSource::loadCardToCache( unsigned int iDataBuffInd )
{
   Config* config = Config::Instance();
   SourceObject* oSource = &m_vecCardDataBuffer.at( iDataBuffInd );
   string szCardName = oSource->GetName( m_AllCharBuff );

   // Get the static Attrs
   vector<Tag> lstStaticAttrs = oSource->GetAttributes( m_AllCharBuff );

   // Build the trait items from each of the ID attrs and their
   // allowed values.
   auto lstAttrRestrictions = oSource->GetIDAttrRestrictions( m_AllCharBuff );

   vector<TraitItem> lstIdentifyingTraits;
   auto iter_Traits = lstAttrRestrictions.begin();
   for( ; iter_Traits != lstAttrRestrictions.end(); ++iter_Traits )
   {
      TraitItem newTrait( iter_Traits->first,
                          iter_Traits->second,
                          config->GetPairedKeysList() );
      lstIdentifyingTraits.push_back( newTrait );
   }

   CollectionObject oCard( szCardName, lstStaticAttrs,
                           lstIdentifyingTraits );

   // Store the location of the CollectionObject in the cache
   oSource->SetCacheIndex( m_vecCardCache.size() );

   // Cache the CollectionObject
   m_vecCardCache.push_back( oCard );
   return m_vecCardCache.size() - 1;
}

SourceObject*
CollectionSource::getNewSourceObject()
{
   m_vecCardDataBuffer.push_back( SourceObject( m_iAllCharBuffSize ) );
   return &m_vecCardDataBuffer.back();
}

int
CollectionSource::findInBuffer( const string& aszCardName )
{
   // Binary search chokes on all sorts of characters so Im just 
   // using linear search.
   string szCardNameFixed = aszCardName;

   StringHelper::convertToSearchString( szCardNameFixed );

   int iSize = m_vecCardDataBuffer.size();
   int iLeft = 0;
   int iRight = iSize;
   if( iRight < 1 )
   {
      return -1;
   }

   string szName;
   while( iLeft <= iRight )
   {
      int middle = (iLeft + iRight) / 2;

      if( middle < 0 || middle >= iSize )
      {
         return -1;
      }

      szName = m_vecCardDataBuffer.at( middle ).GetName( m_AllCharBuff );
      StringHelper::convertToSearchString( szName );

      if( szName == szCardNameFixed )
         return middle;
      else if( szName < szCardNameFixed )
         iLeft = middle + 1;
      else
         iRight = middle - 1;
   }
   return -1;
}

int
CollectionSource::findInCache( const string& aszName )
{
   auto iter_ColObj = m_vecCardCache.begin();
   int index = 0;
   for( ; iter_ColObj != m_vecCardCache.end(); ++iter_ColObj )
   {
      if( iter_ColObj->GetName() == aszName )
      {
         return index;
      }
      index++;
   }
   return -1;
}

void
CollectionSource::resetBuffer()
{
   m_vecCardCache.clear();
   m_vecCardDataBuffer.clear();

   m_iAllCharBuffSize = 0;
   delete[] m_AllCharBuff;
   m_AllCharBuff = new char[ms_iMaxBufferSize];
}

void
CollectionSource::finalizeBuffer()
{
   char* newBufferSize = new char[m_iAllCharBuffSize];
   memcpy( newBufferSize, m_AllCharBuff, m_iAllCharBuffSize );
   delete[] m_AllCharBuff;
   m_AllCharBuff = newBufferSize;
}

CollectionSource::
SourceMapFunctor::SourceMapFunctor(char* aiBuffer)
   : m_charBuffer(aiBuffer)
{

}

CollectionSource::
SourceMapFunctor::~SourceMapFunctor()
{

}

bool
CollectionSource::
SourceMapFunctor::operator()( const SourceObject& agrpLeft, const SourceObject& agrpRight ) const
{
   return agrpLeft.GetName( m_charBuffer ) < agrpRight.GetName( m_charBuffer );
}