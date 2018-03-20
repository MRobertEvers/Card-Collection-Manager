#include "StringHelper.h"

StringHelper::StringHelper()
{
}


StringHelper::~StringHelper()
{
}

std::string 
StringHelper::Str_Trim(const std::string& src, char removeChar)
{
   size_t first = src.find_first_not_of(removeChar);
   if (std::string::npos == first)
   {
      return src;
   }
   size_t last = src.find_last_not_of(removeChar);
   return src.substr(first, (last - first + 1));
}

std::string
StringHelper::Str_Replace(const std::string& srz, char removeChar, char replaceChar)
{
   std::string szRetVal = "";
   for (char c : srz)
   {
      if (c != ' ')
      {
         szRetVal += c;
      }
   }
   return szRetVal;
}

std::string 
StringHelper::Str_Clean(const std::string& src)
{
   std::string szRetVal = "";
   size_t iSize = src.size();
   for( size_t i = 0; i < iSize; i++ )
   {
      char c = src[i];
      if( c == 145 )
      {
         szRetVal += "ae";
      }
      else if( c == 146 )
      {
         szRetVal += "AE";
      }
      else if( c == '\'' || c == '\"' )
      {
         // leave out
      }
      else if( c < 0 )
      {
         // Check for em dash
         if( (i + 2 < iSize) &&
             ( c == -30         ) && 
             ( src[i+1] == -128 ) &&
             ( src[i+2] == -108 ) )
         {
            szRetVal += '-';
            i += 2;
         }
      }
      else
      {
         szRetVal += c;
      }
   }

   return szRetVal;
}
 
std::vector<std::string> 
StringHelper::Str_CmdLineParse( const std::string& srz )
{
   std::vector<std::string>  lstRetval;
   size_t iFirstQuote = srz.find_first_of('\"');
   if( iFirstQuote == -1 )
   {
      auto lstSplit = Str_Split( srz, " " );
      for( auto parm : lstSplit )
      {
         lstRetval.push_back(parm);
      }
   }
   else
   {
      std::string szThis = srz.substr(0, iFirstQuote);
      if( iFirstQuote > 0 )
      {
         szThis = Str_Trim( szThis, ' ' );
         auto lstSplit = Str_Split( szThis, " " );
         for( auto parm : lstSplit )
         {
            if( parm.size() > 0 )
            {
               lstRetval.push_back(parm);
            }
         }
      }

      if( iFirstQuote + 1 < srz.size() )
      {
         std::string szNext = srz.substr(iFirstQuote+1);
         int iSecondQuote = szNext.find_first_of('\"');
         if( iSecondQuote == -1 )
         {
            iSecondQuote = szNext.size() - 1;
         }
         lstRetval.push_back( szNext.substr(0, iSecondQuote) );

         if( iSecondQuote != szNext.size() - 1 )
         {
            std::string szNextParse = szNext.substr(iSecondQuote+1);
            auto lstNextParse = Str_CmdLineParse(szNextParse);
            for( auto parm : lstNextParse )
            {
               lstRetval.push_back(parm);
            }
         }
      }
   }

   return lstRetval;
}

std::vector<std::string> 
StringHelper::Str_Split(const std::string& aszSplit, const std::string& aszDelim)
{
   std::vector<std::string> vecRetval;
   if( aszDelim.size() == 0 ) 
   { 
      vecRetval.push_back(aszSplit);
      return vecRetval;
   }

   std::string szSplit = aszSplit;
   int iFindFirst = szSplit.find(aszDelim);
   if( iFindFirst == std::string::npos )
   {
      if( szSplit.size() > 0 )
      {
         vecRetval.push_back(szSplit);
      }
      return vecRetval;
   }
   else
   {
      if( iFindFirst > 0 )
      {
         vecRetval.push_back(szSplit.substr(0, iFindFirst));
      }

      auto vecRemaining = Str_Split(szSplit.substr(iFindFirst+aszDelim.size()), aszDelim);
      for( auto& szNext : vecRemaining )
      {
         vecRetval.push_back(szNext);
      }
   }

   return vecRetval;
}

std::vector<std::string> 
StringHelper::SplitIntoLines(const std::string& aszString)
{
   std::vector<std::string> lstLines;
   std::string szLine = "";
   for (size_t i = 0; i < aszString.size(); i++)
   {
      if (aszString[i] == '\n')
      {
         lstLines.push_back(szLine);
         szLine = "";
      }
      else
      {
         szLine = szLine + aszString[i];
         if (i == aszString.size() - 1)
         {
            lstLines.push_back(szLine);
            szLine = "";
         }
      }

   }
   return lstLines;
}

void
StringHelper::convertToSearchString(std::string& aszSearch)
{
   size_t iSize = aszSearch.size();
   for( size_t i = 0; i < iSize; i++ )
   {
      if( !isSearchCharacter(aszSearch[i]) )
      {
         aszSearch[i] = ' ';
      }
      else
      {
         aszSearch[i] = ::tolower(aszSearch[i]);
      }
   }
}

bool 
StringHelper::isSearchCharacter(char c)
{
   return ('a' <= c && c <= 'z') || 
          ('A' <= c && c <= 'Z') ||
          ('0' <= c && c <= '9') || 
          (' ' == c);
}