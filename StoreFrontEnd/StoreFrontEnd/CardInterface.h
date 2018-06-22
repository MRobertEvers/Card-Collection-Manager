#pragma once
#include <string>
#include <vector>
class CollectionInterface;
class CardInterface
{
public:
   CardInterface() {};
   CardInterface(const std::string& aszCardLine, CollectionInterface* apParent);
   ~CardInterface();

   int GetNumber() const;
   std::string GetHash() const;
   std::string GetFirstUID() const;
   std::string GetName() const;
   std::string GetMultiverseID() const;
   std::string GetManaCost() const;
   std::string GetCMC() const;
   std::string GetCardType() const;
   std::string GetSet() const;
   std::string GetMetaTag( const std::string& aszKey, const std::string& aszUID = "" ) const;
   std::string GetAttribute( const std::string& aszKey ) const;

   CollectionInterface* GetCollection();

private:
   std::vector<std::string> m_vecRepresentingUIDs;
   std::string m_szName;
   unsigned int m_iCount;
   CollectionInterface* m_pCollection;
};

