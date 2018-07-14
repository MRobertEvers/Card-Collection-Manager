#include "TraitItem.h"
#include "../Config.h"
#include <algorithm>

using namespace std;

CardVariantField::CardVariantField( const string& aszKeyname,
                                    const vector<string>& alstKeyVals, 
                                    const vector<Tag>& alstPairedTraits )
{
   Config* config = Config::Instance();
   m_szKeyName = aszKeyname;
   m_vecPossibleValues = alstKeyVals;

   if (m_vecPossibleValues.size() == 0)
   {
      m_vecPossibleValues.push_back("");
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
   return *m_vecPossibleValues.begin();
}

vector<string> 
CardVariantField::GetAllowedValues() const
{
   return m_vecPossibleValues;
}

set<string>
CardVariantField::GetPairedTraits( ) const
{
   return m_setLinkedFields;
}

bool 
CardVariantField::IsAllowedValue(string aszTestVal) const
{
   auto iter_find = find( m_vecPossibleValues.begin(), 
                          m_vecPossibleValues.end(), 
                          aszTestVal );
   return iter_find != m_vecPossibleValues.end();
}

// DO NOT CALL THIS FUNCTION ON A VARIANT FIELD THAT IS A COPY!
CardInstanceField 
CardVariantField::GetInstanceField( const string aszValue ) const
{
   return CardInstanceField(this, aszValue);
}

CardInstanceField::CardInstanceField( CardVariantField const* apBase, const string& aszValue )
   : m_pBase( apBase )
{
   SetValue( aszValue );
}

CardInstanceField::~CardInstanceField()
{

}

string
CardInstanceField::GetKey() const
{
   return m_pBase->GetKeyName();
}

string
CardInstanceField::GetValue() const
{
   return m_szValue;
}

bool
CardInstanceField::SetValue( const string aszNewValue )
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

CardVariantField const*
CardInstanceField::GetBase() const
{
   return m_pBase;
}

CardFieldCollection::CardFieldCollection( const map<string, CardVariantField>& amapParent )
{
   for( auto& field : amapParent )
   {
      m_mapIdentifyingTraits.emplace( field.first, field.second.GetInstanceField() );
   }
}

CardFieldCollection::~CardFieldCollection()
{
}

bool 
CardFieldCollection::SetAttributes( const vector<Tag>& avecAttrs )
{
   bool bAllSuccess = true;
   set<string> setAlready;

   for( auto& vecSet : avecAttrs )
   {
      // See if we want to skip this trait, e.g. if we've already set the trait.
      if( setAlready.find( vecSet.first ) != setAlready.end() )
      {
         continue;
      }

      if( !trySet( vecSet.first, vecSet.second ) )
      {
         bAllSuccess = false;
         continue;
      }
      // We succeeded.
      setAlready.insert( vecSet.first );

      // If we've succeeded so far, then the attr exists.
      auto iter_hasAttr = m_mapIdentifyingTraits.find( vecSet.first );
      auto paired = iter_hasAttr->second.GetBase()->GetPairedTraits();
         
      // Check if we are setting any of the paired traits
      for( auto& pairItem : paired )
      {
         // Always stop trying to set this trait a second time.
         setAlready.insert( pairItem );

         // Try to set the paired trait always
         // If it failed. Panic
         bAllSuccess &= trySet( pairItem, getPairedValueEquivalent( vecSet.first, pairItem ) );
      }
   }

   return bAllSuccess;
}

bool 
CardFieldCollection::SetAttribute( const string & aszKey, const string & aszValue )
{
   return SetAttributes(vector<Tag>(1, std::make_pair( aszKey, aszValue ) ));
}

string 
CardFieldCollection::GetAttribute( const string & aszKey ) const
{
   auto iter_find = m_mapIdentifyingTraits.find( aszKey );
   if( iter_find != m_mapIdentifyingTraits.end() )
   {
      return iter_find->second.GetValue();
   }
   else
   {
      return Config::NotFoundString;
   }
}

vector<Tag>
CardFieldCollection::GetIdentifyingAttributes() const
{
   vector<Tag> vecRetval;
   for( auto& tag : m_mapIdentifyingTraits )
   {
      vecRetval.push_back( make_pair( tag.first, tag.second.GetValue() ) );
   }
   return vecRetval;
}

map<string, CardInstanceField>::const_iterator
CardFieldCollection::begin() const
{
   return m_mapIdentifyingTraits.cbegin();
}

map<string, CardInstanceField>::const_iterator
CardFieldCollection::end() const
{
   return m_mapIdentifyingTraits.cend();
}

bool 
CardFieldCollection::trySet( const std::string & aszKey, const std::string & aszValue )
{
   auto iter_hasAttr = m_mapIdentifyingTraits.find( aszKey );
   if( iter_hasAttr != m_mapIdentifyingTraits.end() )
   {
      // Try to set this attribute
      return iter_hasAttr->second.SetValue( aszValue );
   }
   else
   {
      return false;
   }
}

// Gets the current value of 'source' and returns the 'equivalent' value of the target field.
std::string 
CardFieldCollection::getPairedValueEquivalent( const std::string & aszSource, const std::string & aszTarget )
{
   // Assumes both inputs are valid!
   auto iter_source = m_mapIdentifyingTraits.find( aszSource );
   auto iter_target = m_mapIdentifyingTraits.find( aszTarget );

   auto sourceAllowed = iter_source->second.GetBase()->GetAllowedValues();
   auto targetAllowed = iter_target->second.GetBase()->GetAllowedValues();

   int iSourceInd = distance( sourceAllowed.begin(),
                              find( sourceAllowed.begin(),
                                    sourceAllowed.end(),
                                    iter_source->second.GetValue()
                                  ) 
                            );

   return targetAllowed[iSourceInd];
}
