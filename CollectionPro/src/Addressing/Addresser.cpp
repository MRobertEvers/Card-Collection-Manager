#include "../stdafx.h"
#include "Addresser.h"

#include <time.h>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include "../Config.h"
#include "../Support/StringHelper.h"

using namespace std;

unsigned int
Addresser::ms_iRandom = 0;

int
Addresser::GetRandom()
{
   ms_iRandom += rand();

   srand( time( 0 ) - ms_iRandom );

   return rand();
}

SubAddress::SubAddress( unsigned char aiAddress )
{
   m_vecPath.push_back( aiAddress );
}

SubAddress::SubAddress( std::vector<unsigned char> avecAdressPath )
{
   m_vecPath = avecAdressPath;
}

SubAddress::SubAddress( std::string aszPath )
{
   string szSubAddress = aszPath;
   char iSubAddress;
   int iStart = 0;
   while( iStart <= szSubAddress.size() - 2 )
   {
      string szSubByte = szSubAddress.substr( iStart, 2 );

      // Convert hex string to int.
      unsigned int iNumChars;
      iSubAddress = stoi( szSubAddress, &iNumChars, 16 );
      if( iNumChars > 0 )
      {
         // Reverses order.
         m_vecPath.push_back( iSubAddress );
      }

      iStart += 2;
   }
}

bool 
SubAddress::IsSuperSetOf( const SubAddress& rhs )
{
   bool bIsSuper = true;
   int i;
   for( i = 0; i < Size() && i < rhs.Size(); i++ )
   {
      bIsSuper &= m_vecPath[i] == rhs.m_vecPath[i];
   }

   bIsSuper &= i == Size();

   return bIsSuper;
}

vector<unsigned char> 
SubAddress::GetPath( int aiStop ) const
{
   if( aiStop == -1 )
   {
      return m_vecPath;
   }
   else if( aiStop < (int)Size() )
   {
      return vector<unsigned char>( m_vecPath.begin(), m_vecPath.begin() + aiStop );
   }
   else
   {
      return m_vecPath;
   }
}

string
SubAddress::ToString()
{
   stringstream ss;
   for( auto node : m_vecPath )
   {
      ss << std::setfill('0') << std::setw(2) << std::hex << (int)node;
   }
   return ss.str();
}

void 
SubAddress::Push( unsigned char aiNode )
{
   m_vecPath.insert( m_vecPath.begin(), aiNode );
}


bool
SubAddress::operator==( const SubAddress& rhs ) const
{
   if( m_vecPath.size() != rhs.m_vecPath.size() )
   {
      return false;
   }
   else
   {
      for( int i = 0; i < m_vecPath.size(); i++ )
      {
         if( m_vecPath[i] != rhs.m_vecPath[i] )
         {
            return false;
         }
      }

      return true;
   }
}

bool
SubAddress::operator!=( const SubAddress& rhs ) const
{
   return !(*this == rhs);
}

bool
SubAddress::operator<( const SubAddress& rhs ) const
{
   if( m_vecPath.size() < rhs.m_vecPath.size() )
   {
      return true;
   }
   else if( m_vecPath.size() > rhs.m_vecPath.size() )
   {
      return false;
   }
   else
   {
      for( int i = 0; i < m_vecPath.size(); i++ )
      {
         if( m_vecPath[i] < rhs.m_vecPath[i] )
         {
            return true;
         }
         else if( m_vecPath[i] > rhs.m_vecPath[i] )
         {
            return false;
         }
      }

      // They are equal
      return false;
   }
}

Identifier::Identifier()
{

}

Identifier::Identifier(const string& aszId)
{
   parseIdName(aszId);
}

Identifier::~Identifier()
{

}

bool 
Identifier::IsEmpty() const
{
   return m_veciSubAddresses.size()==0;
}

vector<SubAddress>
Identifier::GetSubAddresses() const
{
   return m_veciSubAddresses;
}

vector<Location> 
Identifier::GetLocations() const
{
   vector<Location> vecRetVal;
   for( auto iSub : m_veciSubAddresses )
   {
      vecRetVal.push_back( Location( m_szMain, iSub ) );
   }
   return vecRetVal;
}

string 
Identifier::GetMain() const
{
   return m_szMain;
}

string 
Identifier::GetFullAddress() const
{
   string szFullString = GetMain();
   bool first = true;
   for( SubAddress_t subAddr : m_veciSubAddresses)
   {
      if (!first) { szFullString += ","; }
      else { szFullString += "-"; }
      szFullString += subAddr.ToString();
      first = false;
   }

   return szFullString;
}

Location
Identifier::GetBase() const
{
   return Location(GetMain());
}

Address 
Identifier::ToAddress() const
{
   return Address(GetFullAddress());
}
   
bool 
Identifier::operator==( const Identifier& rhs ) const
{
   return GetFullAddress()==rhs.GetFullAddress();
}

bool 
Identifier::operator!=( const Identifier& rhs ) const
{
   return !(*this == rhs);
}

bool 
Identifier::operator<( const Identifier& rhs ) const
{
   return false;
}

void 
Identifier::parseIdName( const string& aszID )
{
   string szSubAddress; 
   unsigned int iNumChars; 
   unsigned int iSubAddress;
   vector<string> lstUIandPF;
   vector<string> lstSubAddresses;

   // Record the family name.
   lstUIandPF = StringHelper::Str_Split(aszID, string("-"));
   if( lstUIandPF.size() > 0 )
   {
      m_szMain = lstUIandPF[0];
   }
   else
   {
      m_szMain = Config::NotFoundString;
   }
   //TODO

   // Add the parsed sub-addresses, no subaddresses defaults to 0 meaning root.
   if( lstUIandPF.size() == 1 )
   {
      lstUIandPF.push_back("00");
   }
   
   if( lstUIandPF.size() > 1 )
   {
      lstSubAddresses = StringHelper::Str_Split( lstUIandPF[1], string( "," ) );
      size_t iSAC = lstSubAddresses.size();
      for (size_t i = 0; i < iSAC; i++)
      {
         szSubAddress = lstSubAddresses[i];
         if( szSubAddress.size() % 2 == 0 && szSubAddress.size() >= 2 )
         {
            int iStart = 0;
            while( iStart <= szSubAddress.size()-2 )
            {
               string szSubByte = szSubAddress.substr( iStart, 2 );

               // Convert hex string to int.
               iSubAddress = stoi( szSubAddress, &iNumChars, 16 );
               if( iNumChars > 0 )
               {
                  addSubAddress( m_veciSubAddresses, iSubAddress );
               }

               iStart += 2;
            }

         }

      }
   }
   else
   {
      // Root collection
   }

   //// Add the parsed subAddresses, default to 1 if none named.
   //lstUIandPF.push_back("1");
   //if( lstUIandPF.size() > 1 )
   //{
   //   lstSubAddresses = StringHelper::Str_Split(lstUIandPF[1], string(","));
   //   size_t iSAC = lstSubAddresses.size();
   //   for (size_t i = 0; i < iSAC; i++)
   //   {
   //      szSubAddress = lstSubAddresses[i];
   //      iSubAddress = stoi(szSubAddress, &iNumChars);
   //      if (iNumChars > 0)
   //      {
   //         addSubAddress(m_veciSubAddresses, iSubAddress);
   //      }
   //   }
   //}

   //// Remove the default if this address is empty
   //if( aszID == "" )
   //{
   //    m_veciSubAddresses.clear();
   //}
}

// Returns
// -1 := Did not add b/c it is already specified.
// 0 := Failed to Add
// 1 := Replaced Exisiting aisa
// 2 := Inserted ordered
int 
Identifier::addSubAddress(vector<SubAddress_t>& avecSAs, SubAddress_t aiSA)
{
   int iAdded = 0;

   // Replace a less specific set with this more
   // general set.
   // or do nothing because this SA is less specific.
   for( size_t i = 0; i < avecSAs.size(); i++ )
   {
      SubAddress SA = avecSAs[i];
      if( isSuperSet( SA, aiSA ) )
      {
         avecSAs[i] = aiSA;
         iAdded = 1;
         break;
      }

      if( isSuperSet( aiSA, SA ) )
      {
         iAdded = -1;
         break;
      }
   }
   
   // Add the sub address in an ordered manner.
   if( iAdded == 0 )
   {
      int iCmp = 0;
      auto iter = avecSAs.begin();
      for(; iter != avecSAs.end(); ++iter)
      {
         iCmp = compareSubAddress(aiSA, *iter);
         if (iCmp <= 0) { break; }
      }
   
      if( ( iter == avecSAs.begin() ) || 
          ( iCmp != 0               ) )
      {
         avecSAs.insert(iter, aiSA);
      }

      iAdded = 2;
   }

   return iAdded;
}

bool
Identifier::isSuperSet(SubAddress_t aiSuperSet, SubAddress_t aiSubSet) const
{
   return aiSuperSet.IsSuperSetOf( aiSubSet );
}

int 
Identifier::compareSubAddress( SubAddress_t aiSOne,
                               SubAddress_t aiSTwo ) const
{
   if( aiSOne == aiSTwo )
   {
      return 0;
   }
   else
   {
      return aiSOne < aiSTwo ? 1 : -1;
   }
}

Address::Address()
{

}

Address::Address(const string& aszId) : Identifier(aszId)
{

}

Address::~Address()
{

}

vector<SubAddress> 
Address::GetSubAddresses() const
{
   return m_veciSubAddresses;
}


// Returns true if ANY of the subaddresses of this address
// is a superset of the location.
// There should only ever be exactly 1 subaddress in the found address.
bool 
Address::ContainsLocation( const Location& aLoc ) const
{
   if( GetMain() != aLoc.GetMain() ){ return false; }

   bool bContains = false;
   for( auto iSub : m_veciSubAddresses )
   {
      if( isSuperSet( iSub, aLoc.GetSubAddress() ) )
      {
         bContains = true;
         break;
      }
   }

   return bContains;
}

bool 
Address::AddSubAddress( SubAddress aiSub )
{
   return addSubAddress(m_veciSubAddresses, aiSub) > 0;
}

// TODO: This will completely remove all matchin addresses
// instead of pithing the subset subaddresses.
int 
Address::RemoveSubAddress( SubAddress aiSub )
{
   int iResult = 0;
   int iSetVal = aiSub.GetLeastSignificantValue();

   for( auto iSub : GetSubAddresses() )
   {
      if( isSuperSet( aiSub, iSub ) )
      {
         SetSubAddress(iSub, iSetVal);
         iResult = 1;
      }
   }
   return iResult;
}

// Inputting aiSub = 1 will remove that item.
// Returns
// 0 := Item doesn't exist
// 1 := Perfect Replace
// 2 := Replaced another and removed old.
// 3 := Another already specifies, removed old.
// 4 := Error
int 
Address::SetSubAddress(SubAddress_t aiAlreadySub, SubAddress_t aiSub)
{
   int iResult = 0;
   auto iter_found = find( m_veciSubAddresses.begin(),
                           m_veciSubAddresses.end(),
                           aiAlreadySub );

   if( iter_found != m_veciSubAddresses.end() )
   {
      vector<SubAddress_t> vecAddTest(m_veciSubAddresses);
      vecAddTest.erase(vecAddTest.begin() + 
                       distance(m_veciSubAddresses.begin(), iter_found));

      // Now we want the subaddress to be added new
      int iAdded = addSubAddress(vecAddTest, aiSub);
      if( aiSub == 0 )
      {
         // Set iAdded to -1 so we can remove the value.
         iAdded = -1;
      }

      if( iAdded == 2 )
      {
         // aiSub is specified by no other SA.
         *iter_found = aiSub;
         iResult = 1;
      }
      else if( iAdded == 1 )
      {
         // aiSub more specific than another SA.
         // (i.e. specifies MORE sets)
         m_veciSubAddresses.erase(iter_found);
         addSubAddress(m_veciSubAddresses, aiSub);
         iResult = 2;
      }
      else if( iAdded == -1 )
      {
         // aiSub is less specific
         // (i.e. specifies fewer sets)
         m_veciSubAddresses.erase(iter_found);
         iResult = 3;
      }
      else
      {
         iResult = 4;
      }
   }

   return iResult;
}

bool 
Address::MergeIdentifier( const Identifier& aID )
{
   if( GetMain() != aID.GetMain() ){ return false; }

   for( auto iSub : aID.GetSubAddresses() )
   {
      return AddSubAddress(iSub);
   }

   return true;
}

bool
Address::ExtractIdentifier( const Identifier& aID )
{
   if( GetMain() != aID.GetMain() ){ return false; }

   bool bResult = false;

   // This tries to remove all the addresses,
   // regardless of whether they are present or not.
   for( auto iSub : aID.GetSubAddresses() )
   {
      bool bSub = RemoveSubAddress( iSub ) != 0;
      bResult |= bSub;
   }

   return bResult;
}

Location::Location()
{

}

Location::Location(const string& aszId) : Identifier(aszId)
{
   if( m_veciSubAddresses.size() > 0 )
   {
      m_iAddress = m_veciSubAddresses.at(0);
   }
   else
   {
      m_iAddress = 1;
   }
}

Location::Location( const string& aszMain, SubAddress_t aiSA )
{
   m_iAddress = aiSA;
   m_szMain = aszMain;

   // Add the address to this vector so the "IsEmpty" function works.
   m_veciSubAddresses.push_back(m_iAddress);
}

Location::~Location()
{

}

// Returns true if this location is a super set of ANY of the
// subaddresses in aAddress.
bool 
Location::IsSpecifiedBy( const Address& aAddress ) const
{
   //rAddrIn = Address(aAddress.GetMain());

   if (aAddress.GetMain() != GetMain()) { return false; }

   bool bFoundSubAddressMatch = false;
   auto vecSAs = aAddress.GetSubAddresses();
   for( auto iSub : vecSAs )
   {
      bool bIsSuper = isSuperSet(GetSubAddress(), iSub);
      if( bIsSuper ) 
      {
         //rAddrIn.AddSubAddress(iSub);
      } 
      bFoundSubAddressMatch |= bIsSuper;
   }

   return bFoundSubAddressMatch;
}

vector<SubAddress_t>
Location::GetSubAddresses() const
{
   return vector<SubAddress_t>(1, m_iAddress);
}

vector<Location> 
Location::GetLocationsSpecified() const
{
   vector<Location> vecRetVal;
   vecRetVal.push_back( *this );
   for( int i = 0; i < m_iAddress.Size(); i++ )
   {
      vecRetVal.push_back( Location(m_szMain, m_iAddress.GetPath(i)) );
   }

   return vecRetVal;
}

SubAddress_t
Location::GetSubAddress() const
{
   return m_iAddress;
}

Address 
Location::ToAddress() const
{
   return Address(GetFullAddress());
}