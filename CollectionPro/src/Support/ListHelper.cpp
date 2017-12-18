#include "ListHelper.h"

ListHelper::ListHelper()
{
}

ListHelper::~ListHelper()
{
}

std::vector<std::pair<std::string, std::string>>
ListHelper::ConvertMapToList(const std::map<std::string, std::string>& aMap)
{
   std::vector<std::pair<std::string, std::string>> lstRetVal;
   std::map<std::string, std::string>::const_iterator iter_Map = aMap.cbegin();
   for( ; iter_Map != aMap.cend(); ++iter_Map )
   {
      lstRetVal.push_back( std::make_pair( iter_Map->first, iter_Map->second ) );
   }
   return lstRetVal;
}

bool ListHelper::CompareKeyValPairList( const std::vector<std::pair<std::string, std::string>>& alstFirst,
                                        const std::vector<std::pair<std::string, std::string>>& alstSecond )
{
   bool bMatch = alstFirst.size() == alstSecond.size();

   if( bMatch )
   {
      std::vector<std::pair<std::string, std::string>>::const_iterator iter_First = alstFirst.cbegin();
      std::vector<std::pair<std::string, std::string>>::const_iterator iter_Second = alstSecond.cbegin();

      for( ; iter_First != alstFirst.cend(); ++iter_First )
      {
         bool bFoundMatch = false;
         for( ; iter_Second != alstSecond.cend(); ++iter_Second )
         {
            if( ( iter_First->first  == iter_Second->first ) && 
                ( iter_First->second == iter_Second->second ) )
            {
               bFoundMatch = true;
            }
         }

         if( !bFoundMatch )
         {
            bMatch = false;
            break;
         }
      }
   }
   return bMatch;
}
