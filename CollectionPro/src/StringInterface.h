#pragma once
#include<string>
#include<vector>

#include "Support\TypeDefs.h"
#include "Collection\CopyItem.h"

using namespace std;

class __declspec(dllexport) StringInterface
{
public:
   enum InterfaceLineType
   {
      AddLine = 0x1,
      RemoveLine = 0x2,
      ChangeLine = 0x3,
      Corrupt = 0x0
   };

public:
   StringInterface();
   ~StringInterface();

   static bool ParseCardLine( const string& aszLine,
                              unsigned int& riCount,
                              string& rszName,
                              string& rszDetails,
                              string& rszMeta );

   static bool ParseCardLine( const string& aszLine,
                              unsigned int& riCount,
                              string& rszName,
                              vector<Tag>& rszDetails,
                              vector<Tag>& rszMeta );

   static bool ParseTagString( const string& aszDetails,
                               vector<Tag>& rlstTags );

   static bool ParseListDelimString( const string& aszDelimStr,
                                     vector<string>& rlstStrings,
                                     const string& aszIndicatorString = "*",
                                     const string& aszDelim = "::");

   static string ToCardLine( const string& aszName,
                             const vector<Tag>& alstAttrs    = vector<Tag>(),
                             const vector<Tag>& alstMetaTags = vector<Tag>() );

   static InterfaceLineType ParseInterfaceLine( string& rszLine );

   static string CmdCreateAddition(const string& aszName, const string& aszSet);
   static string CmdCreateRemove(const string& aszLongName, const string& aszUID);
   static string CmdCreateReplace( const string& aszLongNameRemove, const string& aszUID,
                                   const string& aszNameAddition, const string& aszSet );
   static string CmdCreateModify( const string& aszLongName, const string& aszUID,
                                  const vector<Tag>& alstAttrs = vector<Tag>(),
                                  const vector<Tag>& alstMetaTags = vector<Tag>() );

   static string CmdAppendCount(const string& aszCmd, int Count);
   static string GetNameFromCardLine(const string& aszLongIdentifier);
   static string FindTagInList(const vector<Tag>& avecList, const string& aszKey);

   static string GetUIDKey();
   static string GetSessionKey();
   static string GetHashKey();
   static string GetAddressKey();

   bool IsCollectionOverheadPropertyLine( const string& aszLine );
public:
   template<class Iter> static
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

   template<class Iter> static
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

