#pragma once
#include<string>
#include<map>
#include<vector>


using namespace std;

// SourceObject
//  Stores the necessary data to extract necessary information from
//  the collection source buffer.
class SourceObject
{
public:
   SourceObject();
   ~SourceObject();

   unsigned int AddAttribute(
      const string& key,
      const string& value,
      char* aplstCharBuf,
      unsigned int aiMaxBuffSize);

   // Used in loading from source DB.
   string GetName(const char* aiSearchBuffer) const;

   // Requires the 3 char keycode
   string GetAttribute(const string& aszKeyCode,
      const char* aiSearchBuffer) const;
   vector<pair<string, string>> GetAttributes(char* aiSearchBuffer);

   map<string, vector<string>> GetIDAttrRestrictions(char* aiSearchBuffer);

   unsigned int GetBufferOffset() const;
   void SetBufferOffset(unsigned int aiOffset);
   unsigned short GetCacheIndex() const;
   void SetCacheIndex(unsigned short aiIndex);

private:
   static const char ms_iNumKeys = 13;
   unsigned short m_pLstKeyVals[ms_iNumKeys];

   // Stores the buffer offset in the first 23 bits.
   // Stores the cache  index  in the last  14 bits.
   unsigned char m_iOffsetAndIndex[5];

   void setSize( unsigned short& riSize,
                 unsigned short aiValue ) const;
   void setFieldCode( unsigned short& riFC,
                      unsigned short aiValue ) const;
   unsigned short extractSize(unsigned short aiCheck) const;
   unsigned char extractFieldCode(unsigned short aiCheck) const;

   unsigned int insertAttribute( const string& value,
                                 char* aplstCharBuf,
                                 unsigned int aiMaxBuffSize,
                                 char acLocation );

   unsigned int getObjectSize(int aiTruncate = -1) const;
   unsigned int getKeyListEnd() const;
};