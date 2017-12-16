#include "TraitItem.h"
#include "../Support/Config.h"

TraitItem::TraitItem( const std::string& aszKeyname,
                      const std::vector<std::string>& alstKeyVals, 
                      const std::vector<Tag>& alstPairedTraits )
{
   Config* config = Config::Instance();
   m_szKeyName = aszKeyname;
   m_lstPossibleValues = std::set<std::string>( alstKeyVals.begin(), 
                                                alstKeyVals.end() );

   if (m_lstPossibleValues.size() == 0)
   {
      m_lstPossibleValues.insert("");
   }

   std::vector<Tag>::const_iterator iter_Tags = alstPairedTraits.cbegin();
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

std::string TraitItem::GetKeyName() const
{
   return m_szKeyName;
}

std::string TraitItem::GetDefaultValue() const
{
   return *m_lstPossibleValues.begin();
}

std::vector<std::string> TraitItem::GetAllowedValues()
{
   return std::vector<std::string>( m_lstPossibleValues.begin(), 
                                    m_lstPossibleValues.end() );
}

bool TraitItem::IsAllowedValue(std::string aszTestVal)
{
   return m_lstPossibleValues.find(aszTestVal) != m_lstPossibleValues.end();
}