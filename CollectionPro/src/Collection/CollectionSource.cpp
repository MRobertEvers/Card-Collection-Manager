#include "..\stdafx.h"

#include "../rapidxml-1.13\rapidxml_print.hpp"
#include "../rapidxml-1.13\rapidxml.hpp"
#include "../rapidxml-1.13\rapidxml_utils.hpp"
#include "../Support/Config.h"
#include "../Support/StringHelper.h"
#include "CollectionSource.h"


#include <fstream>
#include <algorithm>
#include <iostream>
#include <sstream>

using namespace rapidxml;
using namespace std;

CollectionSource::CollectionSource() {
   m_bIsLoaded = false;
   resetBuffer();
}

CollectionSource::~CollectionSource() {
   m_vecCardCache.clear();
   m_vecCardDataBuffer.clear();

   delete[] m_AllCharBuff;
}

// The source file needs to be handled independently of this .exe.
void 
CollectionSource::LoadLib(string aszFilePath) {
   Config* config = Config::Instance();

   // Verify the file is good.
   ifstream file(aszFilePath);
   if (!file.good()) {
      // TODO Close collection.
      return Debug::Log( "CollectionSource",
                         "Could not load library from " + aszFilePath );
   }

   stringstream buffer;
   buffer << file.rdbuf();
   file.close();

   // Prepare the document to be read.
   xml_document<> doc;
   string* textContent = new string(buffer.str());
   doc.parse<0>(&textContent->front());

   xml_node<> *xmlNode_CardDatabase = doc.first_node();
   xml_node<>* xmlNode_Cards = xmlNode_CardDatabase->first_node("cards");

   // Load the cards.
   xml_node<>* xmlNode_Card = xmlNode_Cards->first_node();
   while( xmlNode_Card != 0 ) {
      loadCard(xmlNode_Card);
      xmlNode_Card = xmlNode_Card->next_sibling();
   }

   // Free up the captured memory.
   delete textContent;
   finalizeBuffer();

   m_bIsLoaded = true;
}


int
CollectionSource::LoadCard(string aszCardName)
{
   Config* config = Config::Instance();
   string szCardName = StringHelper::Str_Trim(aszCardName, ' ');

   // Since it is faster to look in the cache, check that first.
   int iCacheLocation = findInCache(szCardName);
   if( iCacheLocation == -1 )
   {
      // Look in the Source Object Buffer for a matching item.
      // If the card is found in the buffer, create a CollectionObject and cache it.
      int iFound = findInBuffer(szCardName);
      if( iFound != -1 )
      {
         iCacheLocation = loadCardToCache(iFound);
      }
   }

   return iCacheLocation;
}


TryGet<CollectionObject>
CollectionSource::GetCardPrototype(int aiCacheIndex)
{
   TryGet<CollectionObject> ColRetVal;
   if( aiCacheIndex < m_vecCardCache.size() ) 
   {
      ColRetVal.Set(&m_vecCardCache.at(aiCacheIndex));
   }

   return ColRetVal;
}

TryGet<CollectionObject>
CollectionSource::GetCardPrototype(string aszCardName)
{
   int iCacheIndex = LoadCard(aszCardName);
   return GetCardPrototype(iCacheIndex);
}

// Notifies all collections other than the input collection that they
// need to sync.
void
CollectionSource::NotifyNeedToSync(const Location& aAddrForcedSync) 
{
   for( auto iter_SyncCols = m_mapSync.begin();
             iter_SyncCols != m_mapSync.end();
             ++iter_SyncCols )
   {
      if( iter_SyncCols->first != aAddrForcedSync )
      {
         iter_SyncCols->second = true;
      }
      else
      {
         iter_SyncCols->second = false;
      }
   }
}

bool 
CollectionSource::IsSyncNeeded(const Location& aAddrNeedSync)
{
   auto oFound = m_mapSync.find(aAddrNeedSync);
   if( oFound != m_mapSync.end() ) 
   {
      return oFound->second;
   }
   else 
   {
      m_mapSync.insert(make_pair(aAddrNeedSync, false));
      return false;
   }
}

vector<int>
CollectionSource::GetCollectionCache( Location aAddrColID,
                                      CollectionObjectType aColItemType ) 
{
   vector<int> lstRetVal;

   for( size_t i = 0; i < m_vecCardCache.size(); i++ ) 
   {
      if( m_vecCardCache[i].FindCopies(aAddrColID, aColItemType).size() > 0 )
      {
         lstRetVal.push_back(i);
      }
   }

   return lstRetVal;
}

vector<shared_ptr<CopyItem>>
CollectionSource::GetCollection( Location aAddrColID,
                                 CollectionObjectType aColItemType) 
{
   vector<shared_ptr<CopyItem>> lstRetVal;

   for( auto& item : m_vecCardCache ) 
   {
      auto lstCopies = item.FindCopies(aAddrColID, aColItemType);

      auto iter_Copy = lstCopies.begin();
      for( ; iter_Copy != lstCopies.end(); ++iter_Copy ) 
      {
         lstRetVal.push_back(*iter_Copy);
      }
   }

   return lstRetVal;
}

vector<string>
CollectionSource::GetAllCardsStartingWith(string aszText) 
{
   vector<string> lstCards;

   bool bActiveCache = aszText.size() > 2;

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
         if( pairICache.first == aszText ) 
         {
            auto iter_Result = pairICache.second.begin();
            for( ; iter_Result != pairICache.second.end(); ++iter_Result ) 
            {
               lstCards.push_back((iter_Result)->GetName(m_AllCharBuff));
            }

            return lstCards;
         }
         else if( aszText.substr(0, pairICache.first.size()) == pairICache.first )
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
      string szCard = iter_Cards->GetName(m_AllCharBuff);
      transform(szCard.begin(), szCard.end(), szCard.begin(), ::tolower);
      size_t iFindIndex = 0;
      iFindIndex = szCard.find(aszText);
      if( iFindIndex != string::npos ) 
      {
         if( iFindIndex == 0 ) 
         {
            lstStartCards.push_back(iter_Cards->GetName(m_AllCharBuff));
         }
         else 
         {
            lstOthers.push_back(iter_Cards->GetName(m_AllCharBuff));
         }

         if( bActiveCache ) 
         {
            lstCache.push_back(*iter_Cards);
         }
      }
   }

   for( string sz : lstStartCards ) 
   {
      lstCards.push_back(sz);
   }
   for( string sz : lstOthers )
   {
      lstCards.push_back(sz);
   }

   if( bActiveCache ) 
   {
      m_lstSearchCache.push_back(make_pair(aszText, lstCache));
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
CollectionSource::loadCard(rapidxml::xml_node<char> * xmlNode_Card) 
{
   Config* config = Config::Instance();
   SourceObject* sO = getNewSourceObject();

   xml_node<> *xmlNode_CardAttribute = xmlNode_Card->first_node();
   while( xmlNode_CardAttribute != 0 ) {
      string szCardKey = xmlNode_CardAttribute->name();
      char keyCode = config->GetKeyCode(szCardKey);
      if( keyCode != -1 ) {
         m_iAllCharBuffSize += sO->AddAttribute(szCardKey, xmlNode_CardAttribute->value(),
            m_AllCharBuff, ms_iMaxBufferSize);
      }

      xmlNode_CardAttribute = xmlNode_CardAttribute->next_sibling();
   }
}

int 
CollectionSource::loadCardToCache(unsigned int iDataBuffInd)
{
   Config* config = Config::Instance();
   SourceObject* oSource = &m_vecCardDataBuffer.at(iDataBuffInd);
   string szCardName = oSource->GetName(m_AllCharBuff);

   // Get the static Attrs
   vector<Tag> lstStaticAttrs = oSource->GetAttributes(m_AllCharBuff);

   // Build the trait items from each of the ID attrs and their
   // allowed values.
   auto lstAttrRestrictions = oSource->GetIDAttrRestrictions(m_AllCharBuff);

   vector<TraitItem> lstIdentifyingTraits;
   auto iter_Traits = lstAttrRestrictions.begin();
   for( ; iter_Traits != lstAttrRestrictions.end(); ++iter_Traits )
   {
      TraitItem newTrait( iter_Traits->first,
                          iter_Traits->second,
                          config->GetPairedKeysList() );
      lstIdentifyingTraits.push_back(newTrait);
   }

   CollectionObject oCard( szCardName, lstStaticAttrs,
                           lstIdentifyingTraits );

   // Store the location of the CollectionObject in the cache
   oSource->SetCacheIndex(m_vecCardCache.size());

   // Cache the CollectionObject
   m_vecCardCache.push_back(oCard);
   return m_vecCardCache.size() - 1;
}

SourceObject* 
CollectionSource::getNewSourceObject()
{
   m_vecCardDataBuffer.push_back(SourceObject(m_iAllCharBuffSize));
   return &m_vecCardDataBuffer.back();
}

int
CollectionSource::findInBuffer( const string& aszCardName ) 
{
   // Binary search chokes on all sorts of characters so Im just 
   // using linear search.
   string szCardNameFixed = aszCardName;

   StringHelper::convertToSearchString(szCardNameFixed);

   int iSize = m_vecCardDataBuffer.size();
   int iLeft = 0;
   int iRight = iSize;
   if( iRight < 1 ) {
      return -1;
   }

   string szName;
   while( iLeft <= iRight ) {
      int middle = (iLeft + iRight) / 2;

      if( middle < 0 || middle >= iSize ) {
         return -1;
      }

      szName = m_vecCardDataBuffer.at(middle).GetName(m_AllCharBuff);
      StringHelper::convertToSearchString(szName);

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
   memcpy(newBufferSize, m_AllCharBuff, m_iAllCharBuffSize);
   delete[] m_AllCharBuff;
   m_AllCharBuff = newBufferSize;
}

