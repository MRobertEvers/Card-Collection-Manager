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

}

CollectionSource::~CollectionSource() {

}

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

void 
CollectionSource::loadCard(rapidxml::xml_node<char> * xmlNode_Card) {
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

SourceObject* 
CollectionSource::getNewSourceObject() {
   m_vecCardDataBuffer.push_back(SourceObject(m_iAllCharBuffSize));
   return &m_vecCardDataBuffer.back();
}


int
CollectionSource::findInBuffer(string aszCardName, bool abCaseSensitive) {
   // Binary search chokes on all sorts of characters so Im just 
   // using linear search.
   string szCardNameFixed = StringHelper::convertToSearchString(aszCardName);
   if( !abCaseSensitive ) {
      transform(szCardNameFixed.begin(),
         szCardNameFixed.end(),
         szCardNameFixed.begin(), ::tolower);
   }

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
      szName = StringHelper::convertToSearchString(szName);
      if( !abCaseSensitive ) {
         transform(szName.begin(), szName.end(),
            szName.begin(), ::tolower);
      }

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
CollectionSource::findInCache(string aszName,
   bool abCaseSensitive) {
   auto iter_ColObj = m_vecCardCache.begin();
   int index = 0;
   for( ; iter_ColObj != m_vecCardCache.end(); ++iter_ColObj ) {
      if( iter_ColObj->GetName() == aszName ) {
         return index;
      }
      index++;
   }
   return -1;
}

void CollectionSource::resetBuffer() {
   m_vecCardCache.clear();
   m_vecCardDataBuffer.clear();

   m_iAllCharBuffSize = 0;
   delete[] m_AllCharBuff;
   m_AllCharBuff = new char[ms_iMaxBufferSize];
}

void CollectionSource::finalizeBuffer() {
   char* newBufferSize = new char[m_iAllCharBuffSize];
   memcpy(newBufferSize, m_AllCharBuff, m_iAllCharBuffSize);
   delete[] m_AllCharBuff;
   m_AllCharBuff = newBufferSize;
}
