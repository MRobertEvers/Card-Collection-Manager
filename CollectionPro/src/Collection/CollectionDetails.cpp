#include "CollectionDetails.h"

#include <ctime>
#include "../Config.h"
#include "../Support/StringHelper.h"

CollectionDetails::CollectionDetails()
{
   m_ptrAddress = new Location();
}


CollectionDetails::~CollectionDetails()
{
   delete m_ptrAddress;
}

void 
CollectionDetails::SetName(std::string aszName)
{
   m_szName = aszName;
}

std::string 
CollectionDetails::GetName()
{
   return m_szName;
}

void 
CollectionDetails::SetFileName(std::string aszFileName, bool abDefaultLocation)
{
   m_szFileName = aszFileName;
   
   if( abDefaultLocation )
   {
      SetFile(Config::Instance()->GetCollectionsDirectory() + "\\\\" + m_szFileName + ".txt");
   }
}

// Returns the name of the file to save to.
std::string 
CollectionDetails::GetFileName()
{
   return m_szFileName;
}

void 
CollectionDetails::SetFile( std::string aszFile )
{
   m_szFile = aszFile;

   // Get the file name.
   auto lstSplitFile = StringHelper::Str_Split(aszFile, "\\");
   auto szFile = lstSplitFile[lstSplitFile.size() - 1];
   auto lstSplitExt = StringHelper::Str_Split(szFile, ".");
   szFile = lstSplitExt[0];

   SetFileName(szFile);
}
std::string 
CollectionDetails::GetFile()
{
   return m_szFile;
}

void 
CollectionDetails::SetTimeStamp(unsigned long aulTimeStamp)
{
   if( aulTimeStamp == 0 )
   {
      time_t timer;
      time(&timer);
      aulTimeStamp = timer;
   }

   m_ulTimeStamp = aulTimeStamp;
}

unsigned long 
CollectionDetails::GetTimeStamp()
{
   return m_ulTimeStamp;
}

void 
CollectionDetails::SetChildrenCount(unsigned int auiChildCount)
{
   m_iChildrenCount = auiChildCount;
}

unsigned int 
CollectionDetails::GetChildrenCount()
{
   return m_iChildrenCount;
}

void 
CollectionDetails::IncrementChildCount()
{
   m_iChildrenCount++;
}

void 
CollectionDetails::SetInitialized( bool abInits )
{
   m_bInitialized = abInits;
}

bool 
CollectionDetails::IsInitialized()
{
   return m_bInitialized;
}

void 
CollectionDetails::SetProcessLines( const std::vector<std::string>& alstProcesslines )
{
   m_vecProcessLines = alstProcesslines;
}

std::vector<std::string> 
CollectionDetails::GetProcessLines()
{
   return m_vecProcessLines;
}

void 
CollectionDetails::SetAddress(const Location& aAddress)
{
   delete m_ptrAddress;
   m_ptrAddress = new Location(aAddress);
}

void 
CollectionDetails::AssignAddress(std::string aszStringAddress)
{
   if (aszStringAddress == "")
   {
      Addresser addr;
      aszStringAddress = std::to_string(addr.GetRandom() % 1000000);
   }

   delete m_ptrAddress;
   m_ptrAddress = new Location(aszStringAddress);
}

Location* 
CollectionDetails::GetAddress()
{
   return m_ptrAddress;
}