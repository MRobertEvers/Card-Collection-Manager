#pragma once
#include<string>
#include<memory>
#include "../Support/TryGet.h"
#include "../Support/TypeDefs.h"
#include "../Addressing/Addresser.h"
#include "TraitItem.h"
#include "CopyItem.h"

enum CollectionObjectType : int
{
   Local = 0x1,
   Borrowed = 0x2,
   Real = 0x3,
   Virtual = 0x4,
   All = 0xF
};

enum FindType : int
{
   UID = 0x1,
   Hash = 0x2
};

// CollectionObject
//  Contains all the common data related to the object of this type.
//  Maintains a list of all instances and their variations via copy items.
//  Acts as a manager of Copy Items.
class CollectionObject
{
   // Aids in the construction of CopyItems
   class PseudoIdentifier
   {
   public:
      PseudoIdentifier();
      PseudoIdentifier(unsigned int aiCount, std::string aszName,
         std::string aszDetails, std::string aszMeta);
      ~PseudoIdentifier();

      unsigned int Count;
      std::string Name;
      std::string DetailString;
      std::string MetaString;
      std::vector<Tag> Identifiers;
      std::vector<Tag> MetaTags;
   };

public:
   CollectionObject( const std::string& aszItemName,
                     const std::vector<Tag>& alstCommon,
                     const std::vector<TraitItem>& alstRestrictions );
   ~CollectionObject();
   // Adds an item to the list of copies.
   std::shared_ptr<CopyItem> AddCopy(const Location& aAddrColID,
      const std::vector<Tag>& alstAttrTags,
      const std::vector<Tag>& alstMetaTags);

   bool RemoveCopy(const Location& aAddrColID,
      const std::string aszUniqueID);

   std::string GenerateHash(const Identifier& aAddrIdentifier,
      const std::vector<Tag>& alstAttrs,
      const std::vector<Tag>& alstMetaTags) const;

   // Deletes an item from the list of copies.
   void DeleteCopy(const std::string& aszUniqueHash);

   TryGetCopy<std::shared_ptr<CopyItem>> FindCopy( const std::string& aszUniqueHash,
                                                   FindType aiType = UID ) const;
   std::vector<std::shared_ptr<CopyItem>> FindCopies( const Identifier& aCollection,
                                                      CollectionObjectType aSearchType ) const;
   std::vector<std::shared_ptr<CopyItem>> FindCopies( const Location& aCollection,
                                                      CollectionObjectType aSearchType ) const;

   std::string GetName() const;
   std::string GetProtoType() const;

private:
   std::string m_szName;
   std::vector<Tag> m_lstCommonTraits;
   std::vector<TraitItem> m_lstIdentifyingTraits;

   std::vector<std::shared_ptr<CopyItem>> m_lstCopies;

   // Copy Item Interface
   //
public:
   std::vector<TraitItem> GetIdentifyingTraits();
   bool MatchIdentifyingTrait(const std::string& aszValue,
      std::string& rszKey);

   // Attempts to set an identifying trait on a copy item.
   // Returns true if the value is valid.
   bool SetIdentifyingTrait( CopyItem* aptItem,
                             const std::string& aszTraitKey,
                             const std::string& aszTraitValue,
                             bool bSession = true ) const;
   void SetIdentifyingTraitDefaults(CopyItem* aptItem) const;

   void DeleteCopy(CopyItem* aptItem);
   std::string CopyToString( CopyItem const* aptItem,
                             const MetaTagType& aAccessType = MetaTagType::Public,
                             const Identifier& aAddrCompareID = Location() ) const;


private:
   CopyItem * createCopy(const Identifier& aAddrColID,
      const std::vector<Tag>& alstAttrs = std::vector<Tag>(),
      const std::vector<Tag>& alstMetaTags = std::vector<Tag>()) const;

   void setCopyPairAttrs(CopyItem* aptItem, const std::string& aszKey, int iVal) const;

public:
   static bool ParseCardLine(const std::string& aszLine, PseudoIdentifier& rPIdentifier);

   static bool ParseTagString(const std::string& aszDetails, std::vector<Tag>& rlstTags);

   static std::string ToCardLine( const Identifier& aAddrParentID,
                                  const std::string& aszName,
                                  const std::vector<Tag>& alstAttrs = std::vector<Tag>(),
                                  const std::vector<Tag>& alstMetaTags = std::vector<Tag>(),
                                  const Identifier& aAddrCompareID = Location() );
};                                                                                                                                                                                                                                                                                                                                