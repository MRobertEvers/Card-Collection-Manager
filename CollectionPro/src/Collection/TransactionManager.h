#pragma once
#include<vector>
#include<string>
#include "Transaction.h"
#include "../Addressing/Addresser.h"

typedef std::pair<std::string, std::string> Tag;

class Collection;
class CopyItem;
class CollectionSource;

/* TransactionManager
*  Stores a list of transactions. Exposes the private interface of the stored
*  collection so that Actions may perform the corresponding function on the
*  collection.
*/
class TransactionManager
{
public:
   TransactionManager(Collection* aptrCollection);
   ~TransactionManager();
   
   void IncludeAction(const Action& aAction);

   void FinalizeTransaction(bool abActual = true);
   void RollbackTransaction();

   void TransactionsAsynced();

   Collection* GetCollection();
   CollectionSource* GetSource();

   CopyItem* Add(
      std::string aszName, 
      std::vector<Tag> alstAttrs, 
      std::vector<Tag> alstMetaTags);

   void AddFrom(
      std::string aszName,
      std::string aszIdentifyingHash,
      const Identifier& aResiAddress);
   
   void Remove(
      std::string aszName, 
      std::string aszIdentifyingHash);

   void Change(
      std::string aszName,  
      std::string aszIdentifyingHash, 
      std::vector<Tag> alstChanges, 
      std::vector<Tag> alstMetaChanges);

   void Replace(
      std::string aszName, 
      std::string aszIdentifyingHash, 
      std::string aszNewName, 
      std::vector<Tag> alstIdChanges, 
      std::vector<Tag> alstMetaChanges);

private:
   Collection* m_ptrCollection;
   std::vector<Transaction*> m_lstOpenTransactions;

   Transaction* getTransaction();
   Transaction* getLastTransaction();

   void clearTransactions();
};

