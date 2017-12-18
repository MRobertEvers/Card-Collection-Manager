#pragma once

#include <string>
#include <vector>
#include <map>
#include <functional>

class ListHelper
{
public:
   ListHelper();
   ~ListHelper();

 
   static std::vector<std::pair<std::string, std::string>>
   ConvertMapToList(const std::map<std::string, std::string>& aMap);

   static bool CompareKeyValPairList(const std::vector<std::pair<std::string, std::string>>& alstFirst,
                                     const std::vector<std::pair<std::string, std::string>>& alstSecond);

   template<typename T, typename R> static
   int List_Find(const T& aFind, R aBegin, R aEnd)
   {
      std::vector<T> lstFindList = std::vector<T>(aBegin, aEnd);
      return List_Find(aFind, lstFindList);
   }

   template<typename T> static
   int List_Find(const T& aFind, const std::vector<T>& alstFindList)
   {
      std::vector<T>::const_iterator iter_list = alstFindList.cbegin();
      int index = 0;
      for (; iter_list != alstFindList.cend(); iter_list++)
      {
         if (*iter_list == aFind)
         {
            return index;
         }
         index++;
      }
      return -1;
   }

   template<class T, class R, class Iter> static
   int List_Find( const T& aiFind, 
                  const Iter aBegin, const Iter aEnd, 
                  const std::function<T(const R&)>& afuncExtractor )
   {
      std::vector<R> lstFind = std::vector<R>(aBegin,aEnd);
      return List_Find(aiFind, lstFind, afuncExtractor);
   }

   template<class T, class R> static
   int List_Find( const T& aiFind, 
                  const std::vector<R>& alstFindList, 
                  const std::function<T(const R&)>& afuncExtractor )
   {
      std::vector<R>::const_iterator iter_list = alstFindList.cbegin();
      int index = 0;
      for (; iter_list != alstFindList.cend(); iter_list++)
      {
         T tRetval = afuncExtractor(*iter_list);
         if (aiFind == tRetval)
         {
            return index;
         }
         index++;
      }
      return -1;
   }

   template<class T, class R, class Iter> static
   int List_Find_Non_Const( T& aiFind, 
                            Iter aBegin, Iter aEnd, 
                            const std::function<T(R&)>& afuncExtractor )
   {
      std::vector<R> lstFind = std::vector<R>(aBegin, aEnd);
      return List_Find_Non_Const(aiFind, lstFind, afuncExtractor);
   }

   template<class T, class R> static
   int List_Find_Non_Const( T& aiFind, 
                            std::vector<R>& alstFindList, 
                            const std::function<T(R&)>& afuncExtractor )
   {
      std::vector<R>::iterator iter_list = alstFindList.begin();
      int index = 0;
      for (; iter_list != alstFindList.end(); iter_list++)
      {
         T tRetval = afuncExtractor(*iter_list);
         if (aiFind == tRetval)
         {
            return index;
         }
         index++;
      }
      return -1;
   }

   template<class T> static
   void List_Insert( const T& aInsert, 
                     std::vector<T>& alstInsertList, 
                     const std::function<int(const T&,const T&)>& afuncComparer )
   {
      std::vector<T>::const_iterator iter = alstInsertList.cbegin();
      for (; iter != alstInsertList.cend(); ++iter)
      {
         int iCmp = afuncComparer(aInsert, *iter);
         if (iCmp <= 0) { break; }
      }

      alstInsertList.insert(iter, aInsert);
   }
};
