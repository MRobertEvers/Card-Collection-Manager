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
   SourceObject(unsigned int aiCharBufOffset);
   ~SourceObject();

   // AddAttribute
   //  Adds the value to the charBuffer. Records its location
   //  in the buffer using the key and the base offset of this
   //  sourceobject.
   unsigned int AddAttribute(
      const string& key,
      const string& value,
      char* aplstCharBuf,
      unsigned int aiMaxBuffSize);

   // GetName
   //  Returns the first element designated by this source object in the input
   //  buffer.
   // aiSearchBuffer: Buffer to search.
   string GetName(const char* aiSearchBuffer) const;

   // GetAttribute
   //  Returns the attribute at the location determined by aszKeyCode in
   //  the input buffer.
   string GetAttribute( const string& aszKeyCode,
                        const char* aiSearchBuffer) const;

   // GetAttributes
   //  Returns a vector of all common attributes.
   vector<pair<string, string>> GetAttributes(char* aiSearchBuffer);

   // GetIDAttrRestrictions
   //  Returns a map of each identifying attr and its available options.
   map<string, vector<string>> GetIDAttrRestrictions(char* aiSearchBuffer);

   // GetBufferOffset
   //  Returns the location of the first character in a buffer that this
   //  source object will reference.
   unsigned int GetBufferOffset() const;
   void SetBufferOffset(unsigned int aiOffset);

   // GetCacheIndex
   //  Returns the index of the corresponding CollectionObject in the
   //  CollectionSource's cache if loaded.
   unsigned short GetCacheIndex() const;
   void SetCacheIndex(unsigned short aiIndex);

private:
   static const char ms_iNumKeys = 13;
   unsigned short m_pLstKeyVals[ms_iNumKeys];

   // Stores the buffer offset in the first 23 bits.
   // Stores the cache  index  in the last  14 bits.
   unsigned char m_iOffsetAndIndex[5];

   // setSize
   //  Each field stores the size of its value in the first 12 bits of the short
   void setSize( unsigned short& riSize,
                 unsigned short aiValue ) const;

   // setFieldCode
   // Each field stores the field code in the first 4 bits of the short.
   // This means there can only be 15 unique fields.
   void setFieldCode( unsigned short& riFC,
                      unsigned short aiValue ) const;

   // extractSize and extractFieldCode
   // Returns the bits as defined in the above two functions.
   unsigned short inline extractSize(unsigned short aiCheck) const;
   unsigned char inline extractFieldCode(unsigned short aiCheck) const;

   // insertAttribute
   //  The value of each of the fields is stored in one continuous buffer.
   //  This function inserts the "value" string into the buffer relative
   //  to the start of this source object. It inserts the value between
   //  the two neighbouring values in the buffer by shifting the latter
   //  value. It inserts the short with this fields info into m_pLstKeyVals
   //  in the same manner.
   unsigned int insertAttribute( const string& value,
                                 char* aplstCharBuf,
                                 unsigned int aiMaxBuffSize,
                                 char acLocation );

   // getObjectSize
   //  Returns the number of chars this source object is taking up in the
   //  buffer.
   // aiTruncate: Only count the number of chars up to the nth field.
   unsigned int getObjectSize(int aiTruncate = -1) const;

   // getKeyListEnd
   //  Returns the size of the keylist array.
   unsigned int getKeyListEnd() const;
};