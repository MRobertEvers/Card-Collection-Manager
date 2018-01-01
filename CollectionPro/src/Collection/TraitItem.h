#pragma once
#include<string>
#include<vector>
#include<set>
#include "../Support/TypeDefs.h"

class TraitItem
{
public:
   TraitItem( const std::string& aszKeyname,
              const std::vector<std::string>& alstKeyVals, 
              const std::vector<Tag>& alstPairedTraits = std::vector<Tag>() );
   ~TraitItem();
   
   std::string GetKeyName() const;
   std::string GetDefaultValue() const;
   std::vector<std::string> GetAllowedValues();

   bool IsAllowedValue(std::string aszTestVal);

private:
   std::vector<std::string> m_lstPairedValues;

   // This must be a vector to maintain ordering.
   std::vector<std::string> m_lstPossibleValues;
   std::string m_szKeyName;
};

