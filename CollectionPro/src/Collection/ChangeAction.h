#pragma once
#include <vector>
#include <string>
#include "Action.h"

class ChangeAction : public Action
{
public:
   ChangeAction();
   ~ChangeAction();

   bool Execute(TransactionManager* aoCol) override;
   bool Rollback(TransactionManager* aoCol) override;

   std::shared_ptr<Action> GetCopy() const override;

   void SetName(const std::string& aszName);
   void SetIDs(std::vector<Tag> alstIds);
   void SetUID(const std::string& aszUID);
   void SetMeta(std::vector<Tag> alstMeta);

private:
   std::string m_szName;
   std::string m_szUID;
   std::vector<Tag> m_lstIdChanges;
   std::vector<Tag> m_lstMetaChanges;

   std::vector<Tag> m_lstUndoID;
   std::vector<Tag> m_lstUndoMeta;

   std::shared_ptr<Action> getUndoAction(TransactionManager* aoCol) const override;
};

