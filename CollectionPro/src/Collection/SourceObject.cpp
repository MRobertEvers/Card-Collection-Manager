#include "SourceObject.h"

SourceObject::SourceObject() {

}

SourceObject::~SourceObject() {

}


unsigned int
SourceObject::AddAttribute(
   const std::string& aszkey,
   const std::string& value,
   char* aplstCharBuf,
   unsigned int aiMaxBuffSize) {
   Config* config = Config::Instance();

   // Name needs to go first
   int iInsertion = getKeyListEnd();
   if (aszkey == "name") {
      iInsertion = 0;
   }

   unsigned int iSizeIncrease = insertAttribute(value, aplstCharBuf,
      aiMaxBuffSize, iInsertion);

   // Shift all field codes down
   for (int i = ms_iNumKeys - 2; i >= iInsertion; --i) {
      setSize(m_pLstKeyVals[i + 1], extractSize(m_pLstKeyVals[i]));
      setFieldCode(m_pLstKeyVals[i + 1], extractFieldCode(m_pLstKeyVals[i]));
   }

   m_pLstKeyVals[iInsertion] = 0;
   setSize(m_pLstKeyVals[iInsertion], value.size());
   setFieldCode(m_pLstKeyVals[iInsertion], config->GetKeyCode(aszkey));

   return iSizeIncrease;
}

std::string
SourceObject::GetName(const char* aiSearchBuffer) const {
   return GetAttribute("name", aiSearchBuffer);
}

std::string
SourceObject::GetAttribute(const std::string& aszKey,
   const char* aiSearchBuffer) const {
   // Get the key code.
   Config* config = Config::Instance();

   int iKeyCode = config->GetKeyCode(aszKey);
   if (iKeyCode != -1) {
      for (int i = 0; i < ms_iNumKeys; i++) {
         if (extractFieldCode(m_pLstKeyVals[i]) == iKeyCode) {
            return std::string(aiSearchBuffer + GetBufferOffset() + getObjectSize(i),
               extractSize(m_pLstKeyVals[i]));
         }
      }
   }

   return Config::NotFoundString;
}

// Only returns unique attrs
std::vector<std::pair<std::string, std::string>>
SourceObject::GetAttributes(char* aiSearchBuffer) {
   Config* config = Config::Instance();
   std::vector<std::pair<std::string, std::string>> lstRetVal;
   for (int i = 0; i < ms_iNumKeys; i++) {
      unsigned char iFC = extractFieldCode(m_pLstKeyVals[i]);
      std::string szKey = config->GetFullKey(iFC);
      if (config->IsStaticAttribute(szKey)) {
         lstRetVal.push_back(std::make_pair(szKey, GetAttribute(szKey, aiSearchBuffer)));
      }
   }

   return lstRetVal;
}

std::map<std::string, std::vector<std::string>>
SourceObject::GetIDAttrRestrictions(char* aiSearchBuffer) {
   std::map<std::string, std::vector<std::string>> mapNonUAttrs;
   Config* config = Config::Instance();

   for (int i = 0; i < ms_iNumKeys; i++) {
      unsigned char iFC = extractFieldCode(m_pLstKeyVals[i]);
      std::string szKey = config->GetFullKey(iFC);
      if (config->IsIdentifyingAttributes(szKey)) {
         std::string szValue = GetAttribute(szKey, aiSearchBuffer);
         std::vector<std::string> lstNewlst = StringHelper::Str_Split(szValue, "::");
         mapNonUAttrs.insert(std::make_pair(szKey, lstNewlst));
      }
   }

   return mapNonUAttrs;
}


unsigned int
SourceObject::GetBufferOffset() const {
   return (m_iOffsetAndIndex[0] << 16) | (m_iOffsetAndIndex[1] << 8) | (m_iOffsetAndIndex[2] & 0xFF);
}

void
SourceObject::SetBufferOffset(unsigned int aiOffset) {
   unsigned char byteOne = aiOffset & 0xFF;
   unsigned char byteTwo = (aiOffset & 0xFF00) >> 8;
   unsigned char byteThree = (aiOffset & 0xFF0000) >> 16;
   m_iOffsetAndIndex[0] = byteThree;
   m_iOffsetAndIndex[1] = byteTwo;
   m_iOffsetAndIndex[2] = byteOne;
}

unsigned short
SourceObject::GetCacheIndex() const {
   return ((m_iOffsetAndIndex[3] & 0x7F) << 8) | (m_iOffsetAndIndex[4]);
}

void
SourceObject::SetCacheIndex(unsigned short aiIndex) {
   unsigned char byteOne = aiIndex & 0xFF;
   unsigned char byteTwo = (aiIndex & 0x7F00) >> 8;
   m_iOffsetAndIndex[3] = byteTwo;
   m_iOffsetAndIndex[4] = byteOne;
}

void
SourceObject::setSize(unsigned short& riSize, unsigned short aiValue) const {
   riSize = extractFieldCode(riSize) | (aiValue << 4);
}

void
SourceObject::setFieldCode(unsigned short& riFC, unsigned short aiValue) const {
   riFC = (extractSize(riFC) << 4) | (aiValue & 0xF);
}

// Returns the first 12 bits of a short.
unsigned short
SourceObject::extractSize(unsigned short aiCheck) const {
   return aiCheck >> 4;
}

unsigned char
SourceObject::extractFieldCode(unsigned short aiCheck) const {
   return (aiCheck & 0xF);
}

unsigned int
SourceObject::insertAttribute(const std::string& value,
   char* aplstCharBuf,
   unsigned int aiMaxBuffSize,
   char acLocation) {
   // Get the insertion location. -1 is the end of the list.
   unsigned int iInsertion = acLocation;

   // Get all the characters that lie AFTER the field location that we want.
   unsigned int iInsertionOffset = getObjectSize(iInsertion);
   unsigned int iBufferOffset = GetBufferOffset() + iInsertionOffset;
   unsigned int iPostBuffSize = getObjectSize() - iInsertionOffset;

   unsigned int iAdditionalSize = value.size();
   unsigned int iBufferOffAfterCopy = iBufferOffset + iAdditionalSize;

   memcpy_s(aplstCharBuf + iBufferOffAfterCopy,
      aiMaxBuffSize - iBufferOffAfterCopy,
      aplstCharBuf + iBufferOffset, iPostBuffSize);

   memcpy_s(aplstCharBuf + iBufferOffset,
      aiMaxBuffSize - iBufferOffset,
      value.c_str(), iAdditionalSize);

   return value.size();
}

unsigned int
SourceObject::getObjectSize(int aiTruncate) const {
   unsigned int iSize = 0;
   unsigned int iEnd = getKeyListEnd();

   if ((aiTruncate != -1) && (aiTruncate < iEnd)) {
      iEnd = aiTruncate;
   }

   for (int i = 0; i < iEnd; i++) {
      iSize += extractSize(m_pLstKeyVals[i]);
   }

   return iSize;
}

unsigned int
SourceObject::getKeyListEnd() const {
   unsigned int iListEnd = ms_iNumKeys;
   for (int i = 0; i < ms_iNumKeys; i++) {
      // Find the first empty field code.
      if (extractFieldCode(m_pLstKeyVals[i]) == 0) {
         iListEnd = i;
         break;
      }
   }

   return iListEnd;
}