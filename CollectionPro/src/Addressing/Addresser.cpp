#include "../stdafx.h"
#include "Addresser.h"
#include "../Config.h"
#include "../Support/StringHelper.h"

#include <time.h>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <stack>

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

SubAddress::SubAddress( const std::string& aszBranch )
   : m_szBranch( aszBranch )
{
}

// -2 not related, -1 this is a subset, 0 same, 1 this is a superset.
int 
SubAddress::Compare( const ISubAddress & other ) const
{
   if( m_szBranch == "" )
   {
      return -1;
   }

   std::string szLeft = m_szBranch;
   std::string szRight = other.ToString();
   size_t iLeft = szLeft.size();
   size_t iRight = szRight.size();
   if( iLeft < iRight )
   {
      if( szRight.substr( 0, iLeft ) == szLeft )
      {
         return 1;
      }
      else
      {
         return -2;
      }
   }
   else if( iRight < iLeft )
   {
      if( szLeft.substr( 0, iRight ) == szRight )
      {
         return -1;
      }
      else
      {
         return -2;
      }
   }
   else if( szLeft == szRight )
   {
      return 0;
   }
   else
   {
      return -2;
   }
}

std::string 
SubAddress::ToString() const
{
   return std::string();
}

void 
SubAddress::Push( const std::string & aszNextStep )
{
   m_szBranch += aszNextStep;
}

std::string 
SubAddress::Pop( )
{
   if( m_szBranch.size() <= 1 )
   {
      m_szBranch = "";
   }
   else
   {
      m_szBranch = m_szBranch.substr( 0, m_szBranch.size() - 1 );
   }
   return m_szBranch;
}

bool 
SubAddress::operator<( const ISubAddress& rhs ) const
{
   return Compare( rhs ) == -1;
}

std::vector<std::string>
SubAddress::ParseLeafString( const std::string & aszTree )
{
   std::vector<std::string> vecParsed;

   // Default to nothing or just empty vec?
   vecParsed.push_back( "" );

   size_t index = 0;
   std::string szCurrentBase = "";
   std::string szCurrentBranch = "";
   std::stack<std::string> stackChar;
   while( index < aszTree.size() )
   {
      char cCurrentChar = aszTree[index];
      if( cCurrentChar == '<' )
      {
         // Append to current base and continue
         szCurrentBase = szCurrentBase + szCurrentBranch;
         szCurrentBranch = "";
         // Push current base onto stack
         stackChar.push( szCurrentBase );
      }
      else if( cCurrentChar == ')' )
      {
         // Append to current base and continue
         if( szCurrentBranch.size() > 0 )
         {
            vecParsed.push_back( szCurrentBase + szCurrentBranch );
         }
         szCurrentBranch = "";

         if( stackChar.size() > 0 )
         {
            szCurrentBase = stackChar.top();
            stackChar.pop();
         }
         else
         {
            // Malformed!!
            return vecParsed;
         }
      }
      else
      {
         szCurrentBranch += cCurrentChar;
      }

      if( ++index == aszTree.size() && szCurrentBranch.size() > 0 )
      {
         vecParsed.push_back( szCurrentBase + szCurrentBranch );
      }
   }

   return vecParsed;
}

std::string
SubAddress::ToLeafString( const std::vector<std::string>& aszSubAddresses )
{
   if( aszSubAddresses.size() == 0 )
   {
      return std::string();
   }
   std::string szRetval;


   std::map<std::string, std::vector<std::string>> myMap;
   for( auto string : aszSubAddresses )
   {
      if( string.size() < 1 )
      {
         continue;
      }
      myMap[string.substr( 0, 1 )].push_back( string.substr( 1 ) );
   }


   for( auto branch : myMap )
   {
      szRetval += "<";
      szRetval += branch.first;
      szRetval += ToLeafString( branch.second );
      szRetval += ")";
   }

   // String < and )
   if( myMap.size() == 1 )
   {
      szRetval = szRetval.substr( 1, szRetval.size() - 2 );
   }

   return szRetval;
}

std::string
SubAddress::CleanLeafString( const std::string & aszTree )
{
   return ToLeafString( ParseLeafString( aszTree ) );
}

Identifier::Identifier()
{
   // Create random identifier
   // TODO: Random here
}

Identifier::Identifier(const string& aszId)
{
   auto vecIdSplit = StringHelper::Str_Split( aszId, "-" );
   if( vecIdSplit.size() > 0 )
   {
      m_szMain = vecIdSplit[0];
   }
   else
   {
      m_szMain = aszId;
   }
}

Identifier::~Identifier()
{

}

std::set<Location> 
Identifier::GetLocations() const
{
   std::set<Location> leaves;
   leaves.insert( Location( m_szMain ) );
   for( auto leaf : GetLeaves() )
   {
      leaves.insert( Location( m_szMain, leaf.ToString() ) );
   }
   return leaves;
}

bool 
Identifier::IsEmpty() const
{
   return (ToString() == "");
}

int
Identifier::Compare( const Identifier & other ) const
{
   if( GetMain() != other.GetMain() )
   {
      return -2;
   }

   if( ToString() == other.ToString() )
   {
      return 0;
   }

   if( IsSuperset( other ) )
   {
      return 1;
   }
   else if( other.IsSuperset( *this ) )
   {
      return -1;
   }
   else
   {
      return -2;
   }
}

string
Identifier::GetMain() const
{
   return m_szMain;
}

void
Identifier::SetMain( const std::string & aszMain )
{
   m_szMain = aszMain;
}

string 
Identifier::ToString() const
{
   if( m_szMain == "" )
   {
      return "";
   }
   else if( GetLeafString() == "" )
   {
      return m_szMain;
   }

   return m_szMain + "-" + GetLeafString();
}

bool 
Identifier::IsSuperset( const Identifier & aSet ) const
{
   return IsPartOfRootPath( aSet );
}

bool 
Identifier::IsPartOfRootPath( const Identifier& aSet ) const
{
   if( GetMain() != aSet.GetMain() )
   {
      return false;
   }

   for( auto oTargetNode : aSet.GetLeaves() )
   {
      for( auto oLeafNode : GetLeaves() )
      {
         int iComp = oTargetNode.Compare( oLeafNode );
         if( iComp == -2 || iComp == -1 )
         {
            continue;
         }
         else
         {
            // If the target node is a superset (part of root path) or equal
            return true;
         }
      }
   }

   // If we haven't found a match
   return false;
}

bool 
Identifier::operator<( const Identifier & rhs ) const
{
   return Compare( rhs ) == -1;
}

void 
Identifier::parseIdentifierString( const std::string & aszString )
{
}

Address::Address()
{

}

Address::Address(const string& aszId)
{
   auto vecIdSplit = StringHelper::Str_Split( aszId, "-" );
   if( vecIdSplit.size() > 0 )
   {
      m_szMain = vecIdSplit[0];

      if( vecIdSplit.size() > 1 )
      {
         auto vecLeaves = SubAddress::ParseLeafString( SubAddress::CleanLeafString(vecIdSplit[1]) );
         for( auto leaf : vecLeaves )
         {
            m_setLeaves.insert( leaf );
         }
      }
   }
   else
   {

   }

}

Address::~Address()
{

}

std::string 
Address::GetLeafString() const
{
   std::string szRetval = "";
   for( auto sub : GetLeaves() )
   {
      szRetval += sub.ToString() + ",";
   }
   return szRetval.substr( 0, szRetval.size() - 1 );
}

std::set<SubAddress_t> 
Address::GetLeaves() const
{
   return m_setLeaves;
}

bool 
Address::AddLeaf( SubAddress_t aiSub )
{
   return false;
}

int 
Address::RemoveLeaf( SubAddress_t aiSub )
{
   return 0;
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
   return 1;
}

bool 
Address::MergeIdentifier( const Identifier& aID )
{
   return true;
}

bool
Address::ExtractIdentifier( const Identifier& aID )
{
   return true;
}

Location::Location()
{
   m_szMain = std::to_string( Addresser::GetRandom() % 1000000 );
}

Location::Location( const string& aszId ) : Location()
{
   auto vecIdSplit = StringHelper::Str_Split( aszId, "-" );
   if( vecIdSplit.size() > 0 )
   {
      m_szMain = vecIdSplit[0];

      if( vecIdSplit.size() > 1 )
      {
         m_Leaf = vecIdSplit[1];
      }
   }
   else
   {
         m_szMain = std::to_string( Addresser::GetRandom() % 1000000 );
   }
}

Location::Location( const std::string & aszId, SubAddress_t aSub ) : Location()
{
   m_szMain = aszId;
   m_Leaf = aSub;
}

Location::~Location()
{

}

std::set<Location> 
Location::GetRootPath() const
{
   std::set<Location> setRet;
   Location tmpLoc( *this );
   setRet.insert( tmpLoc );

   while( tmpLoc.GetLeafString() != "" )
   {
      tmpLoc.m_Leaf.Pop();
      setRet.insert( tmpLoc );
   }

   return setRet;
}

std::string 
Location::GetLeafString() const
{
   return m_Leaf.ToString();
}

std::set<SubAddress_t> 
Location::GetLeaves() const
{
   std::set<SubAddress> set;
   set.insert( m_Leaf );
   return set;
}

SubAddress_t
Location::GetLeaf() const
{
   return m_Leaf;
}