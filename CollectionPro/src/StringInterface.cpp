#include "StringInterface.h"
#include <algorithm>
#include "Support/StringHelper.h"

using namespace std;

StringInterface::StringInterface()
{
}


StringInterface::~StringInterface()
{
}

// This can handle forms of
// [[x]<#>] <name> [{<Dets>}] [: {<Meta>}]
bool 
StringInterface::ParseCardLine( const string& aszLine,
                                unsigned int& riCount,
                                string& rszName,
                                string& rszDetails,
                                string& rszMeta ) const
{
   string szLine, szMeta, szDetails, szName;
   
   // Start with a line with no spaces at the beginning nor end.
   szLine = StringHelper::Str_Trim(aszLine, ' ');

   // STEP 1. Parse the number if there is one. It takes the form [x]<#>
   if( (szLine.size() > 0) && (szLine[0] == 'x') )
   {
	   szLine = szLine.substr(1);
   }

   // Get every character up to the first space. Default 1.
   riCount = 1;
   unsigned int iFirstBreak = szLine.find_first_of(' ');
   if( iFirstBreak != string::npos )
   {
      // If that chunk is a number, then try to parse it.
      unsigned int iTryParse, iParseChars;
      string szMaybeNum = szLine.substr(0, iFirstBreak);

      // An exception is thrown if there are 0 num chars.
      if( ( szMaybeNum.size() > 0 ) && 
          ( isdigit( szMaybeNum[0] ) ) )
      {
         iTryParse = stoul(szMaybeNum, &iParseChars);
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
         szLine = szLine.substr(iFirstBreak);
         szLine = StringHelper::Str_Trim(szLine, ' ');
      }
   }

   if( ( ((int)riCount) < 0 ) || 
       ( szLine.size() == 0 ) )
   {
      return false;
   }

   // STEP 2. Determine if there are details/meta.
   //         We need to know which to parse the name.
   bool bHasDets, bHasMeta;
   unsigned int iFirstBrace =  szLine.find_first_of('{');
   unsigned int iColon = szLine.find_first_of(':');
   unsigned int iNameEnd = szLine.size();
   if( iFirstBrace == iColon )
   {
      // There are neither details nor meta. The only time these
      // can be equal is if they are both npos.
      bHasDets = false;
      bHasMeta = false;
   }
   else if ( iFirstBrace < iColon )
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
   szName = szLine.substr(0, iNameEnd);
   szName = StringHelper::Str_Trim(szName, ' ');
   rszName = szName;

   szLine = szLine.substr(iNameEnd);
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
      
      szDetails = szLine.substr(0, iDetsEnd);
      szDetails = StringHelper::Str_Trim(szDetails, ' ');
      rszDetails = szDetails;

      if( szDetails.front() != '{' && szDetails.back() != '}' )
      {
         return false;
      }
   }

   // STEP 5. Get the metatags if there are any.
   if( bHasMeta )
   {
      szLine = szLine.substr(iColon);
      // In case there is an erroneous colon at the end.
      if( szLine.size() > 0 )
      {
         szLine = szLine.substr(1);
         szLine = StringHelper::Str_Trim(szLine, ' ');
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
                                vector<Tag>& rszMeta ) const
{
   string szDets, szMeta;
   
   bool bGoodParse = ParseCardLine( aszLine, riCount, rszName, szDets, szMeta );
   if( bGoodParse )
   {
      bGoodParse &= ParseTagString(szDets, rszDetails);
      bGoodParse &= ParseTagString(szMeta, rszMeta);
   }

   return bGoodParse;
}

bool
StringInterface::ParseTagString( const string& aszDetails,
                                 vector<Tag>& rlstTags ) const
{
   vector<Tag> lstKeyVals;
   vector<string> lstPairs, lstVal;

   vector<string> lstDetails = StringHelper::Str_Split(aszDetails, " ");

   vector<string>::iterator iter_attrs;
   for (iter_attrs = lstDetails.begin(); 
        iter_attrs != lstDetails.end(); 
        ++iter_attrs)
   {
      lstPairs = StringHelper::Str_Split(*iter_attrs, "=");
      if (lstPairs.size() > 1)
      {
         string szVal = StringHelper::Str_Trim(lstPairs[1], '\"');
         lstKeyVals.push_back(make_pair(lstPairs[0], szVal));
      }
   }
   rlstTags = lstKeyVals;
   return true;
}

bool 
StringInterface::ParseListDelimString( const string& aszDelimStr,
                                       vector<string>& rlstStrings,
                                       const string& aszIndicatorString,
                                       const string& aszDelim ) const
{
   if( aszIndicatorString != aszDelimStr.substr( 0, aszIndicatorString.size() ) )
   {
      return false;
   }

   string szParseStr = aszDelimStr.substr(aszIndicatorString.size());
   rlstStrings = StringHelper::Str_Split(szParseStr, aszDelim);
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
      PairListToTagStr(alstAttrs.cbegin(), alstAttrs.cend(), szTagline);
      szLine += szTagline;
   }

   if( alstMetaTags.size() > 0 )
   {
      szLine += " : ";
      PairListToTagStr(alstMetaTags.cbegin(), alstMetaTags.cend(), szTagline);
      szLine += szTagline;
   }

   return szLine;
}

string
StringInterface::CmdCreateAddition(const string& aszName, const string& aszSet)
{
   // TODO: This should not be a string literal.
   vector<Tag> pairvec = { make_pair("set", aszSet) };
   string szRetVal;
   PairListToTagStr(pairvec.begin(), pairvec.end(), szRetVal);
 
   szRetVal = "+ " + GetNameFromCardLine(aszName) + " " + szRetVal;
   return szRetVal;
}

string
StringInterface::CmdCreateRemove(const string& aszLongName, const string& aszUID)
{
   // TODO: Factor this these cmds better.
   // TODO: This should not be a string literal.
   vector<Tag> pairvec = { make_pair("__UID", aszUID) };
   string szRetVal;
   PairListToTagStr(pairvec.begin(), pairvec.end(), szRetVal);

   szRetVal = "- " + GetNameFromCardLine(aszLongName) + " : " + szRetVal;
   return szRetVal;
}

string
StringInterface::CmdCreateReplace(const string& aszLongNameRemove, const string& aszUID,
                                  const string& aszNameAddition, const string& aszSet)
{
   string szRetVal = CmdCreateRemove(aszLongNameRemove, aszUID);
   szRetVal[0] = '%';
   szRetVal += " -> ";
   szRetVal += CmdCreateAddition(aszNameAddition, aszSet).substr(1);
   return szRetVal;
}

string 
StringInterface::CmdAppendCount(const string& aszCmd, int Count)
{
   return aszCmd.substr(0,1) + " x" + std::to_string(Count) + " " + aszCmd.substr(1);
}

string 
StringInterface::GetNameFromCardLine(const string& aszLongIdentifier)
{
   int iLoseAfter = aszLongIdentifier.find_first_of("[{");
   if( iLoseAfter == string::npos )
   {
      iLoseAfter = aszLongIdentifier.size();
   }
   return aszLongIdentifier.substr(0, iLoseAfter);
}

string 
StringInterface::FindTagInList(const vector<Tag>& aszVector, const string& aszKey)
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