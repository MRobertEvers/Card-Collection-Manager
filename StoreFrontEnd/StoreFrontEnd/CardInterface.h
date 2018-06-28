#pragma once
#include "Group.h"
#include <string>
#include <vector>
class CollectionInterface;

// This should only ever represent cards of the same hash!
// GetMetaTag offers a UID field if you want to see Tracking/Hidden Uids.
class CardInterface : public IGroupItem
{
public:
   CardInterface() {};
   CardInterface(const std::string& aszCardLine, CollectionInterface* apParent);
   CardInterface( const std::string& aszName, const std::string& aszUID, CollectionInterface* apParent );
   ~CardInterface();

   int GetNumber() const;
   std::string GetHash() const;
   std::string GetFirstUID() const;
   std::vector<std::string> GetRepresentingUIDs() const;
   std::string GetName() const;
   std::string GetMultiverseID() const;
   std::string GetManaCost() const;
   std::string GetCMC() const;
   std::string GetCardType() const;
   std::string GetSet() const;
   std::string GetMetaTag( const std::string& aszKey ) const;
   std::string GetMetaTag( const std::string& aszKey, const std::string& aszUID) const;
   std::string GetAttribute( const std::string& aszKey ) const;

   // These modify the collection interface to add a new CardInterface to represent it.
   std::string SetMetaTag( const std::string& aszKey, const std::string& aszVal, const std::string& aszUID = "" ) const;
   std::string SetAttributes( const std::string& aszKey, const std::string& aszUID,
                              const std::vector<std::pair<std::string, std::string>>& avecAttrs ) const;
   std::string SetAttribute( const std::string& aszKey, const std::string& aszVal, const std::string& aszUID = "" ) const;

   CollectionInterface* GetCollection();

private:
   std::vector<std::string> m_vecRepresentingUIDs;
   std::string m_szName;
   unsigned int m_iCount;
   CollectionInterface* m_pCollection;
};

