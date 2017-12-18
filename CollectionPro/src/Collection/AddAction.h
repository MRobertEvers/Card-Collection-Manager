#pragma once
#include <string>
#include <vector>
#include "Action.h"

class RemoveAction;
class Collection;

class AddAction : public Action
{
public:
   AddAction();
   ~AddAction();

   bool Execute(TransactionManager* aoCol) override;
   bool Rollback(TransactionManager* aoCol) override;

   std::shared_ptr<Action> GetCopy() const override;

   void SetName(std::string aszName);
   void SetIDs(const std::vector<Tag>& alstIDs);
   void SetMeta(const std::vector<Tag>& alstMetas);

private:
   std::string m_szName;
   std::vector<Tag> m_lstIDs;
   std::vector<Tag> m_lstMetas;

   std::string m_szAddedUID;

   std::shared_ptr<Action> getUndoAction(TransactionManager* aoCol) const override;
};

