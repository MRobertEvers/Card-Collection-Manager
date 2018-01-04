#pragma once
#include<string>
#include<vector>

#include "Support\TypeDefs.h"

class __declspec(dllexport) StringInterface
{
public:
   StringInterface();
   ~StringInterface();

   bool ParseCardLine( const std::string& aszLine,
                       unsigned int& riCount,
                       std::string& rszName,
                       std::string& rszDetails,
                       std::string& rszMeta ) const;

   bool ParseCardLine( const std::string& aszLine,
                       unsigned int& riCount,
                       std::string& rszName,
                       std::vector<Tag>& rszDetails,
                       std::vector<Tag>& rszMeta ) const;

   bool ParseTagString( const std::string& aszDetails,
                        std::vector<Tag>& rlstTags ) const;

   bool ParseListDelimString( const std::string& aszDelimStr,
                              std::vector<std::string>& rlstStrings,
                              const std::string& aszIndicatorString = "*",
                              const std::string& aszDelim = "::") const;

   std::string ToCardLine( const std::string& aszName,
                           const std::vector<Tag>& alstAttrs    = std::vector<Tag>(),
                           const std::vector<Tag>& alstMetaTags = std::vector<Tag>() );

public:
   template<class Iter>
   bool ListToDelimStr( const Iter aptBegin,
                        const Iter aptEnd,
                        std::string& rszResult,
                        const std::string& aszIndicatorString = "*",
                        const std::string& aszDelim = "::" ) const
   {
      std::string szResult = aszIndicatorString;
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
                          std::string& rszResult ) const
   {
      std::string szResult = "{ ";
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

