#pragma once
#include "../Support/TypeDefs.h"

#include <string>
#include <vector>
#include <set>
#include <map>

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
   std::vector<std::string> GetAllowedValues() const;
   std::set<std::string> GetPairedTraits() const;

   bool IsAllowedValue(std::string aszTestVal) const;

   CardInstanceField GetInstanceField( const std::string aszValue = "" ) const;

private:
   std::set<std::string> m_setLinkedFields;

   // This must be a vector to maintain ordering.
   std::vector<std::string> m_vecPossibleValues;
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

   CardVariantField const* GetBase() const;

private:
   std::string m_szValue;
   CardVariantField const* m_pBase;
};

class CardFieldCollection
{
public:
   CardFieldCollection( const std::map<std::string, CardVariantField>& amapParent );
   ~CardFieldCollection();

   bool SetAttributes( const std::vector<Tag>& avecAttrs );

   bool SetAttribute( const std::string& aszKey,
                      const std::string& aszValue );

   std::string GetAttribute( const std::string& aszKey ) const;
   std::vector<Tag> GetIdentifyingAttributes() const;

   std::map<std::string, CardInstanceField>::const_iterator begin() const;
   std::map<std::string, CardInstanceField>::const_iterator end() const;
private:
   std::map<std::string, CardInstanceField> m_mapIdentifyingTraits;

   bool trySet( const std::string& aszKey,
                const std::string& aszValue );

   std::string getPairedValueEquivalent( const std::string& aszSource,
                                         const std::string& aszTarget );
};