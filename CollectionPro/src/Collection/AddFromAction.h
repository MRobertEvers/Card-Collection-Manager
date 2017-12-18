#pragma once
#include "Action.h"
#include <vector>
#include "../Addressing/Addresser.h"

class AddFromAction : public Action
{
public:
   AddFromAction();
   ~AddFromAction();
   
   bool Execute(TransactionManager* aoCol) override;
   bool Rollback(TransactionManager* aoCol) override;

   std::shared_ptr<Action> GetCopy() const override;

   void SetName(std::string aszName);
   void SetHash(std::string aszHash);
   void SetResi(const Address& aAddress);

private:
   std::string m_szName;
   std::string m_szIdentifyingHash;
   Location m_FromAddress;

   //Location m_ToAddress;

   std::shared_ptr<Action> getUndoAction(TransactionManager* aoCol) const override;
};

