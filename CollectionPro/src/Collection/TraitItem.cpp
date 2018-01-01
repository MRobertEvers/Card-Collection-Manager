#include "TraitItem.h"
#include <algorithm>
#include "../Support/Config.h"

using namespace std;

TraitItem::TraitItem( const string& aszKeyname,
                      const vector<string>& alstKeyVals, 
                      const vector<Tag>& alstPairedTraits )
{
   Config* config = Config::Instance();
   m_szKeyName = aszKeyname;
   m_lstPossibleValues = vector<string>( alstKeyVals.begin(), 
                                         alstKeyVals.end() );

   if (m_lstPossibleValues.size() == 0)
   {
      m_lstPossibleValues.push_back("");
   }

   vector<Tag>::const_iterator iter_Tags = alstPairedTraits.cbegin();
   for (; iter_Tags != alstPairedTraits.cend(); ++iter_Tags)
   {
      if (aszKeyname == iter_Tags->first)
      {
         m_lstPairedValues.push_back(iter_Tags->first);
      }
      else if (aszKeyname == iter_Tags->second)
      {
         m_lstPairedValues.push_back(iter_Tags->second);
      }
   }
}

TraitItem::~TraitItem()
{
}

string TraitItem::GetKeyName() const
{
   return m_szKeyName;
}

string TraitItem::GetDefaultValue() const
{
   return *m_lstPossibleValues.begin();
}

vector<string> TraitItem::GetAllowedValues()
{
   return vector<string>( m_lstPossibleValues.begin(), 
                          m_lstPossibleValues.end() );
}

bool TraitItem::IsAllowedValue(string aszTestVal)
{
   auto iter_find = find( m_lstPossibleValues.begin(), 
                          m_lstPossibleValues.end(), 
                          aszTestVal );
   return iter_find != m_lstPossibleValues.end();
}