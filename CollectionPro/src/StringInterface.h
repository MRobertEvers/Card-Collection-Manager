#pragma once
#include<string>
#include<vector>

#include "Support\TypeDefs.h"

using namespace std;

class __declspec(dllexport) StringInterface
{
public:
   StringInterface();
   ~StringInterface();

   bool ParseCardLine( const string& aszLine,
                       unsigned int& riCount,
                       string& rszName,
                       string& rszDetails,
                       string& rszMeta ) const;

   bool ParseCardLine( const string& aszLine,
                       unsigned int& riCount,
                       string& rszName,
                       vector<Tag>& rszDetails,
                       vector<Tag>& rszMeta ) const;

   bool ParseTagString( const string& aszDetails,
                        vector<Tag>& rlstTags ) const;

   bool ParseListDelimString( const string& aszDelimStr,
                              vector<string>& rlstStrings,
                              const string& aszIndicatorString = "*",
                              const string& aszDelim = "::") const;

   string ToCardLine( const string& aszName,
                      const vector<Tag>& alstAttrs    = vector<Tag>(),
                      const vector<Tag>& alstMetaTags = vector<Tag>() );

   string CmdCreateAddition(const string& aszName, const string& aszSet);
   string CmdCreateRemove(const string& aszLongName, const string& aszUID);
   string CmdCreateReplace( const string& aszLongNameRemove, const string& aszUID,
                            const string& aszNameAddition, const string& aszSet );
   string CmdCreateModify( const string& aszLongName, const string& aszUID,
                           const vector<Tag>& alstAttrs = vector<Tag>(),
                           const vector<Tag>& alstMetaTags = vector<Tag>() );

   string CmdAppendCount(const string& aszCmd, int Count);
   string GetNameFromCardLine(const string& aszLongIdentifier);
   string FindTagInList(const vector<Tag>& avecList, const string& aszKey);

public:
   template<class Iter>
   bool ListToDelimStr( const Iter aptBegin,
                        const Iter aptEnd,
                        string& rszResult,
                        const string& aszIndicatorString = "*",
                        const string& aszDelim = "::" ) const
   {
      string szResult = aszIndicatorString;
      Iter iter = aptBegin;
      for( ; iter != aptEnd; iter++ )
      {
         szResult += *iter;
         szResult += aszDelim;
      }
      rszResult = szResult.substr(0, szResult.size()-aszDelim.size());
      return true;
   }

   template<class Iter>
   bool PairListToTagStr( const Iter aptBegin,
                          const Iter aptEnd,
                          string& rszResult ) const
   {
      string szResult = "{ ";
      Iter iter = aptBegin;
      for( ; iter != aptEnd; iter++ )
      {
         szResult += iter->first;
         szResult += "=\"";
         szResult += iter->second;
         szResult += "\" ";
      }
      szResult += "}";
      rszResult = szResult;
      return true;
   }
};

