#include "TraitItem.h"
#include <algorithm>
#include "../Config.h"

using namespace std;

CardVariantField::CardVariantField( const string& aszKeyname,
                                    const vector<string>& alstKeyVals, 
                                    const vector<Tag>& alstPairedTraits )
{
   Config* config = Config::Instance();
   m_szKeyName = aszKeyname;
   m_setPossibleValues = set<string>( alstKeyVals.begin(), 
                                      alstKeyVals.end() );

   if (m_setPossibleValues.size() == 0)
   {
      m_setPossibleValues.insert("");
   }

   for( auto tag : alstPairedTraits )
   {
      if( tag.first == m_szKeyName )
      {
         m_setLinkedFields.insert( tag.second );
      }
      else if( tag.second == m_szKeyName )
      {
         m_setLinkedFields.insert( tag.first );
      }
   }
}

CardVariantField::~CardVariantField()
{
}

string 
CardVariantField::GetKeyName() const
{
   return m_szKeyName;
}

string 
CardVariantField::GetDefaultValue() const
{
   return *m_setPossibleValues.begin();
}

set<string> 
CardVariantField::GetAllowedValues() const
{
   return m_setPossibleValues;
}

bool 
CardVariantField::IsAllowedValue(string aszTestVal) const
{
   auto iter_find = find( m_setPossibleValues.begin(), 
                          m_setPossibleValues.end(), 
                          aszTestVal );
   return iter_find != m_setPossibleValues.end();
}

// DO NOT CALL THIS FUNCTION ON A VARIANT FIELD THAT IS A COPY!
CardInstanceField 
CardVariantField::GetInstanceField( const std::string aszValue ) const
{
   return CardInstanceField(this, aszValue);
}

CardInstanceField::CardInstanceField( CardVariantField const* apBase, const std::string& aszValue )
   : m_pBase( apBase )
{
   SetValue( aszValue );
}

CardInstanceField::~CardInstanceField()
{

}

std::string
CardInstanceField::GetKey() const
{
   return m_pBase->GetKeyName();
}

std::string
CardInstanceField::GetValue() const
{
   return m_szValue;
}

bool
CardInstanceField::SetValue( const std::string aszNewValue )
{
   if( m_pBase->IsAllowedValue( aszNewValue ) )
   {
      m_szValue = aszNewValue;
      return true;
   }
   else
   {
      m_szValue = m_pBase->GetDefaultValue();
      return false;
   }
}