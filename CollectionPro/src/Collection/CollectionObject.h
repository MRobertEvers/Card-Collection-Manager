#pragma once
#include "../Support/TryGet.h"
#include "../Support/TypeDefs.h"
#include "../Addressing/Addresser.h"
#include "TraitItem.h"
#include "CopyItem.h"

#include <string>
#include <memory>

using namespace std;

// CollectionObject
//  Contains all the common data related to the object of this type.
//  Maintains a list of all instances and their variations via copy items.
//  Acts as a manager of Copy Items.
class CollectionObject
{
public:
   // Aids in the construction of CopyItems
   class PseudoIdentifier
   {
   public:
      PseudoIdentifier();
      PseudoIdentifier(unsigned int aiCount, string aszName,
                       string aszDetails, string aszMeta);
      ~PseudoIdentifier();

      unsigned int Count;
      string Name;
      string DetailString;
      string MetaString;
      vector<Tag> Identifiers;
      vector<Tag> MetaTags;
   };

public:
   CollectionObject( const string& aszItemName,
                     const vector<Tag>& alstCommon,
                     const vector<CardVariantField>& alstRestrictions );
   ~CollectionObject();

   shared_ptr<CopyItem> CreateCopy( const Identifier& aAddrColID,
                                    const vector<Tag>& alstAttrs = vector<Tag>(),
                                    const vector<Tag>& alstMetaTags = vector<Tag>() );

   shared_ptr<CopyItem> AddCopy( const Location& aAddrColID,
                                 const vector<Tag>& alstAttrTags,
                                 const vector<Tag>& alstMetaTags );
   shared_ptr<CopyItem> AddCopy( const shared_ptr<CopyItem>& aCopy );

   bool RemoveCopy( const Location& aAddrColID,
                    const string aszUniqueID );

   string GenerateHash( const Identifier& aAddrIdentifier,
                        const vector<Tag>& alstAttrs,
                        const vector<Tag>& alstMetaTags );

   void DeleteCopy( shared_ptr<CopyItem> aptCopy );

   TryGetCopy<shared_ptr<CopyItem>> FindCopy( const string& aszUniqueID ) const;

   vector<shared_ptr<CopyItem>> FindCopies( const Location& aCollection ) const;

   string GetName() const;
   string GetCommonTrait(const string& aszTrait) const;

private:
   string m_szName;
   map<string, string> m_lstCommonTraits;
   map<string, CardVariantField> m_lstIdentifyingTraits;

   vector<shared_ptr<CopyItem>> m_vecCopies;

   void ledgerCopy( shared_ptr<CopyItem> aptCopy );

   // Copy Item Interface
   //
public:
   map<string, CardVariantField> GetIdentifyingTraits();

public:
   static bool ParseCardLine(const string& aszLine, PseudoIdentifier& rPIdentifier);

   static string ToCardLine( const Identifier& aAddrParentID,
                             const string& aszName,
                             const vector<Tag>& alstAttrs = vector<Tag>(),
                             const vector<Tag>& alstMetaTags = vector<Tag>(),
                             const Identifier& aAddrCompareID = Location(),
                             const unsigned int aiCount = 0 );
};                                                                                                                                                                                                                                                                                                                                