#include "StringInterface.h"
#include "Support/StringHelper.h"
#include "Collection\CopyItem.h"
#include "Config.h"

#include <sstream>
#include <algorithm>
#include <iomanip>
#include <cstdio>
#include <ctime>

using namespace std;

// This can handle forms of
// [[x]<#>] <name> [{<Dets>}] [: {<Meta>}]
bool
StringInterface::ParseCardLine( const string& aszLine,
   unsigned int& riCount,
   string& rszName,
   string& rszDetails,
   string& rszMeta )
{
   string szLine, szMeta, szDetails, szName;

   // Start with a line with no spaces at the beginning nor end.
   szLine = StringHelper::Str_Trim( aszLine, ' ' );

   // STEP 1. Parse the number if there is one. It takes the form [x]<#>
   if( (szLine.size() > 0) && (szLine[0] == 'x') )
   {
      szLine = szLine.substr( 1 );
   }

   // Get every character up to the first space. Default 1.
   riCount = 1;
   unsigned int iFirstBreak = szLine.find_first_of( ' ' );
   if( iFirstBreak != string::npos )
   {
      // If that chunk is a number, then try to parse it.
      unsigned int iTryParse, iParseChars;
      string szMaybeNum = szLine.substr( 0, iFirstBreak );

      // An exception is thrown if there are 0 num chars.
      if( (szMaybeNum.size() > 0) &&
         (isdigit( szMaybeNum[0] )) )
      {
         iTryParse = stoul( szMaybeNum, &iParseChars );
      }
      else
      {
         iParseChars = 0;
      }

      if( (iParseChars != iFirstBreak) &&
         (iParseChars != 0) )
      {
         // There are non num chars mixed in the number.
         return false;
      }

      if( iParseChars > 0 )
      {
         riCount = iTryParse;
         szLine = szLine.substr( iFirstBreak );
         szLine = StringHelper::Str_Trim( szLine, ' ' );
      }
   }

   if( (((int)riCount) < 0) ||
      (szLine.size() == 0) )
   {
      return false;
   }

   // STEP 2. Determine if there are details/meta.
   //         We need to know which to parse the name.
   bool bHasDets, bHasMeta;
   unsigned int iFirstBrace = szLine.find_first_of( '{' );
   unsigned int iColon = szLine.find_first_of( ':' );
   unsigned int iNameEnd = szLine.size();
   if( iFirstBrace == iColon )
   {
      // There are neither details nor meta. The only time these
      // can be equal is if they are both npos.
      bHasDets = false;
      bHasMeta = false;
   }
   else if( iFirstBrace < iColon )
   {
      bHasDets = true;
      bHasMeta = iColon != string::npos;
      iNameEnd = iFirstBrace;
   }
   else
   {
      bHasDets = false;
      bHasMeta = true;
      iNameEnd = iColon;
   }

   if( (iFirstBrace == 0) || (iColon == 0) )
   {
      // There was no name.
      return false;
   }

   // STEP 3. Parse the name.
   szName = szLine.substr( 0, iNameEnd );
   szName = StringHelper::Str_Trim( szName, ' ' );
   rszName = szName;

   szLine = szLine.substr( iNameEnd );
   iColon = iColon - iNameEnd;

   // STEP 4. Get the details if there are any.
   if( bHasDets )
   {
      unsigned int iDetsEnd;
      if( bHasMeta )
      {
         iDetsEnd = iColon;
      }
      else
      {
         iDetsEnd = szLine.size();
      }

      szDetails = szLine.substr( 0, iDetsEnd );
      szDetails = StringHelper::Str_Trim( szDetails, ' ' );
      rszDetails = szDetails;

      if( szDetails.front() != '{' && szDetails.back() != '}' )
      {
         return false;
      }
   }

   // STEP 5. Get the metatags if there are any.
   if( bHasMeta )
   {
      szLine = szLine.substr( iColon );
      // In case there is an erroneous colon at the end.
      if( szLine.size() > 0 )
      {
         szLine = szLine.substr( 1 );
         szLine = StringHelper::Str_Trim( szLine, ' ' );
      }

      szMeta = szLine;
      rszMeta = szMeta;

      if( szMeta.front() != '{' && szMeta.back() != '}' )
      {
         return false;
      }
   }

   return true;
}

bool
StringInterface::ParseCardLine( const string& aszLine,
   unsigned int& riCount,
   string& rszName,
   vector<Tag>& rszDetails,
   vector<Tag>& rszMeta )
{
   string szDets, szMeta;

   bool bGoodParse = ParseCardLine( aszLine, riCount, rszName, szDets, szMeta );
   if( bGoodParse )
   {
      bGoodParse &= ParseTagString( szDets, rszDetails );
      bGoodParse &= ParseTagString( szMeta, rszMeta );
   }

   return bGoodParse;
}


bool
StringInterface::ParseInterfaceLine( const string& aszLine,
   unsigned int& riCount,
   string& rszName,
   vector<Tag>& rszDetails,
   vector<Tag>& rszMeta,
   unsigned int& riCount2,
   string& rszName2,
   vector<Tag>& rszDetails2,
   vector<Tag>& rszMeta2,
   StringInterface::InterfaceLineType& riType )
{
   string szFixedLine = StringHelper::Str_Trim( aszLine, ' ' );
   if( szFixedLine.size() < 1 )
   {
      return false;
   }

   riType = ParseInterfaceLine( szFixedLine );

   if( riType == InterfaceLineType::ChangeLine )
   {
      auto vecPair = StringHelper::Str_Split( szFixedLine, "->" );
      if( vecPair.size() < 2 )
      {
         return false;
      }

      for( auto& pairItem : vecPair )
      {
         pairItem == StringHelper::Str_Trim( pairItem, ' ' );
      }

      if( ParseCardLine( vecPair[0], riCount, rszName, rszDetails, rszMeta ) )
      {
         return ParseCardLine( vecPair[1], riCount2, rszName2, rszDetails2, rszMeta2 );
      }
   }
   else
   {
      return ParseCardLine( szFixedLine, riCount, rszName, rszDetails, rszMeta );
   }

   return false;
}

bool
StringInterface::ParseTagString( const string& aszDetails,
   vector<Tag>& rlstTags )
{
   vector<Tag> lstKeyVals;
   vector<string> lstPairs, lstVal;

   string szDetails = StringHelper::Str_Trim( aszDetails, '{' );
   szDetails = StringHelper::Str_Trim( szDetails, '}' );
   szDetails = StringHelper::Str_Trim( szDetails, ' ' );
   vector<string> lstDetails = StringHelper::Str_Split( szDetails, "\"" );
   //vector<string> lstDetails = StringHelper::Str_Split( aszDetails, " " );
   // { one="two" three=" four" }
   // one="two" three=" four"
   // one=,two, three=, four

   string szKey;
   string szVal;
   vector<string>::iterator iter_attrs;
   for( iter_attrs = lstDetails.begin();
      iter_attrs != lstDetails.end();
      ++iter_attrs )
   {
      if( iter_attrs->find_last_of( '=' ) == iter_attrs->size() - 1 )
      {
         szKey = iter_attrs->substr( 0, iter_attrs->size() - 1 );
         szKey = StringHelper::Str_Trim( szKey, ' ' );
      }
      else
      {
         szVal = StringHelper::Str_Trim( *iter_attrs, ' ' );
      }
      if( szVal != "" && szKey != "" )
      {
         lstKeyVals.push_back( make_pair( szKey, szVal ) );
         szVal = szKey = "";
      }
   }
   rlstTags = lstKeyVals;
   return true;
}

bool
StringInterface::ParseListDelimString( const string& aszDelimStr,
   vector<string>& rlstStrings,
   const string& aszIndicatorString,
   const string& aszDelim )
{
   if( aszIndicatorString != aszDelimStr.substr( 0, aszIndicatorString.size() ) )
   {
      return false;
   }

   string szParseStr = aszDelimStr.substr( aszIndicatorString.size() );
   rlstStrings = StringHelper::Str_Split( szParseStr, aszDelim );
   return true;
}

string
StringInterface::ToCardLine( const string& aszName,
   const vector<Tag>& alstAttrs,
   const vector<Tag>& alstMetaTags )
{
   string szTagline, szLine;

   szLine = aszName;

   if( alstAttrs.size() > 0 )
   {
      szLine += " ";
      PairListToTagStr( alstAttrs.cbegin(), alstAttrs.cend(), szTagline );
      szLine += szTagline;
   }

   if( alstMetaTags.size() > 0 )
   {
      szLine += " : ";
      PairListToTagStr( alstMetaTags.cbegin(), alstMetaTags.cend(), szTagline );
      szLine += szTagline;
   }

   return szLine;
}

unsigned long
StringInterface::GetCurrentTimeCount()
{
   return time( 0 );
}

unsigned long
StringInterface::ToTimeValue( const string& aszTime, const string& aszParse )
{
   struct std::tm tm;
   string szTimeParse = aszTime;// szLine.substr( iOpenDate + 1, iCloseDate - iOpenDate - 1 );
   istringstream ss( szTimeParse );
   // %a day of week abbrev or full
   // %b month abbrev or full
   // %d day of month
   // %Y 4 digit year
   ss >> std::get_time( &tm, aszParse.c_str() );//"%a %b %d %H:%M:%S %Y" );
   return mktime( &tm );
}

string
StringInterface::ToTimeString( unsigned long aulTime )
{
   // OLD WAY
   //time_t now = aulTime;
   //struct tm timeinfo;
   //localtime_s( &timeinfo, &now );
   //char str[26];
   //asctime_s( str, sizeof str, &timeinfo );
   //str[strlen( str ) - 1] = 0;
   //return string( str )

   // This should be the same as the above.
   return ToTimeString( aulTime, "%a %b %d %H:%M:%S %Y" );
}

string
StringInterface::ToTimeString( unsigned long aulTime, const string& aszFormat )
{
   time_t timeType = aulTime;
   tm tmStruct;
   localtime_s( &tmStruct, &timeType );

   stringstream ss;
   ss << put_time( &tmStruct, aszFormat.c_str() );//"%F_%T" );

   return ss.str();
}

StringInterface::InterfaceLineType
StringInterface::ParseInterfaceLine( string& rszLine )
{
   if( rszLine.size() <= 2 )
   {
      return Corrupt;
   }

   rszLine = StringHelper::Str_Trim( rszLine, ' ' );

   string szLoadDirective = rszLine.substr( 0, 1 );
   if( szLoadDirective == "-" ) // REMOVE
   {
      // Of the form ([] meaning optional)
      // Sylvan Card Name [{ set="val" color="val2" } ][: { metatag1="val" metatag2="val2" }]
      rszLine = rszLine.substr( 1 );
      return RemoveLine;
   }
   else if( szLoadDirective == "%" ) // CHANGE
   {
      // Of the form
      // Sylvan Card Name [{ set="val" color="val2" } ][: { metatag1="val" metatag2="val2" }] ->
      //   Another Card Name [{ set="val" color="val2" } ][: { metatag1="val" metatag2="val2" }]
      rszLine = rszLine.substr( 1 );
      return ChangeLine;
   }
   else // ADD
   {
      if( szLoadDirective == "+" )
      {
         rszLine = rszLine.substr( 1 );
      }
      // Of the form
      // Sylvan Card Name [{ set="val" color="val2" } ][: { metatag1="val" metatag2="val2" }]
      return AddLine;
   }
}

string
StringInterface::CmdCreateAddition( const string& aszName, const string& aszSet, int aiCount )
{
   // TODO: This should not be a string literal.
   vector<Tag> pairvec = { make_pair( "set", aszSet ) };
   string szRetVal;
   PairListToTagStr( pairvec.begin(), pairvec.end(), szRetVal );
   szRetVal = "+ " + std::to_string( aiCount ) + " " + GetNameFromCardLine( aszName ) + " " + szRetVal;
   if( aiCount > 1 )
   {
      CmdAppendCount( szRetVal, aiCount );
   }
   return szRetVal;
}

string
StringInterface::CmdCreateRemove( const string& aszLongName, const string& aszUID, int aiCount )
{
   StringInterface szIface;
   // TODO: Factor this these cmds better.
   vector<Tag> pairvec = { make_pair( szIface.GetUIDKey(), aszUID ) };
   string szMeta;
   PairListToTagStr( pairvec.begin(), pairvec.end(), szMeta );

   string szRetVal;
   szRetVal += "- " + GetNameFromCardLine( aszLongName ) + " : " + szMeta;
   if( aiCount > 1 )
   {
      CmdAppendCount( szRetVal, aiCount );
   }

   return szRetVal;
}

string
StringInterface::CmdCreateReplace( const string& aszLongNameRemove, const string& aszUID,
   const string& aszNameAddition, const string& aszSet, int aiCount )
{
   string szRetVal = CmdCreateRemove( aszLongNameRemove, aszUID, aiCount );
   szRetVal[0] = '%';
   szRetVal += " -> ";
   szRetVal += CmdCreateAddition( aszNameAddition, aszSet, 1 ).substr( 1 );
   return szRetVal;
}

string
StringInterface::CmdCreateModify( const string& aszLongName, const string& aszUID,
   const vector<Tag>& alstAttrs,
   const vector<Tag>& alstMetaTags, int aiCount )
{
   string szRetVal = CmdCreateRemove( aszLongName, aszUID, aiCount );
   szRetVal[0] = '%';
   szRetVal += " -> ";
   string szTags;
   PairListToTagStr( alstAttrs.begin(), alstAttrs.end(), szTags );
   szRetVal += GetNameFromCardLine( aszLongName ) + szTags;
   PairListToTagStr( alstMetaTags.begin(), alstMetaTags.end(), szTags );
   szRetVal += " : " + szTags;
   return szRetVal;
}

string
StringInterface::CmdAppendCount( const string& aszCmd, int Count )
{
   return aszCmd.substr( 0, 1 ) + " x" + std::to_string( Count ) + " " + aszCmd.substr( 1 );
}

string
StringInterface::DeltaRemoveCmdString( const string & aszCardName, const vector<Tag>& avecUIDs )
{
   return DeltaCmdString( "-", aszCardName, avecUIDs );
}

string
StringInterface::DeltaAddCmdString( const string & aszCardName, const vector<Tag>& avecUIDs )
{
   return DeltaCmdString( "+", aszCardName, avecUIDs );
}

string
StringInterface::DeltaCmdString( const string & aszCmd, const string & aszCardName, const vector<Tag>& avecUIDs )
{
   return aszCmd + " " + ToCardLine( aszCardName, vector<Tag>(), avecUIDs );
}

string
StringInterface::GetNameFromCardLine( const string& aszLongIdentifier )
{
   int iLoseAfter = aszLongIdentifier.find_first_of( "[{" );
   if( iLoseAfter == string::npos )
   {
      iLoseAfter = aszLongIdentifier.size();
   }
   return aszLongIdentifier.substr( 0, iLoseAfter );
}

string
StringInterface::FindTagInList( const vector<Tag>& aszVector, const string& aszKey )
{
   for( auto& tag : aszVector )
   {

      if( tag.first == aszKey )
      {
         return tag.second;
      }
   }
   return "";
}

string
StringInterface::GetUIDKey()
{
   return MetaTag::GetUIDKey();
}

string
StringInterface::GetSessionKey()
{
   return MetaTag::GetSessionKey();
}

string
StringInterface::GetHashKey()
{
   return MetaTag::GetHashKey();
}

string
StringInterface::GetAddressKey()
{
   return MetaTag::GetAddressKey();
}

bool
StringInterface::IsCollectionOverheadPropertyLine( const string& aszLine )
{
   string szDefKey( Config::CollectionDefinitionKey );
   if( aszLine.size() < szDefKey.size() ) { return false; }
   return (aszLine.substr( 0, szDefKey.size() ) == szDefKey);
}