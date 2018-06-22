#pragma once
#include "../Support/TypeDefs.h"

#include <string>
#include <vector>
#include <set>

class CardInstanceField;
class CardVariantField
{
public:
   CardVariantField( const std::string& aszKeyname,
                     const std::vector<std::string>& alstKeyVals, 
                     const std::vector<Tag>& alstPairedTraits = std::vector<Tag>() );
   ~CardVariantField();
   
   std::string GetKeyName() const;
   std::string GetDefaultValue() const;
   std::set<std::string> GetAllowedValues() const;

   bool IsAllowedValue(std::string aszTestVal) const;

   CardInstanceField GetInstanceField( const std::string aszValue = "" ) const;

private:
   std::set<std::string> m_setLinkedFields;

   // This must be a vector to maintain ordering.
   std::set<std::string> m_setPossibleValues;
   std::string m_szKeyName;
};

class CardInstanceField
{
public:
   CardInstanceField( CardVariantField const* m_pBase, 
                      const std::string& aszValue );
   ~CardInstanceField();

   std::string GetKey() const;

   std::string GetValue() const;
   bool SetValue( const std::string aszNewValue );

private:
   std::string m_szValue;
   CardVariantField const* m_pBase;
};