#pragma once
#include "../Addressing/Addresser.h"
#include <string>
#include <map>

class CollectionDetails
{
public:
   CollectionDetails();
   ~CollectionDetails();

   void SetName(std::string aszName);
   std::string GetName();

   void SetFileName( std::string aszFileName, 
                     bool abDefaultLocation = false );
   std::string GetFileName();

   void SetFileDirectory( std::string aszFileDirectory );
   std::string GetFileDirectory();

   void SetFilePath(std::string aszFile);
   std::string GetFilePath();

   void SetTimeStamp(unsigned long aulTimeStamp = 0);
   unsigned long GetTimeStamp();

   void SetChildrenCount(unsigned int auiChildCount);
   unsigned int GetChildrenCount();
   void IncrementChildCount();

   void SetInitialized(bool abInits);
   bool IsInitialized();

   void SetIsLoaded( bool abLoaded );
   bool IsLoaded();

   void SetProcessLines(const std::vector<std::string>& alstProcesslines);
   std::vector<std::string> GetProcessLines();

   void SetAddress(const Location& aAddress);
   void AssignAddress(std::string aszStringAddress);
   Location* GetAddress();

   void AddPeekValue( const std::string& aszKey, const std::string& aszValue );
   std::vector<std::pair<std::string, std::string>> GetPeekValues();
private:
   bool m_bInitialized;
   bool m_bIsLoaded;
   std::string m_szName;
   std::string m_szFile;
   std::string m_szFileName;
   std::string m_szFileDirectory;
   unsigned long m_ulTimeStamp;
   unsigned int m_iChildrenCount;
   std::vector<std::string> m_vecProcessLines;
   std::map<std::string, std::string> m_mapPeekValues;

   // The address of a collection is always a location. ie. only has 1 subaddr.
   Location* m_ptrAddress;
};

