#pragma once
#include <vector>
#include "Action.h"

class ReplaceAction : public Action
{
public:
   ReplaceAction();
   ~ReplaceAction();

   bool Execute(TransactionManager* aoCol) override;
   bool Rollback(TransactionManager* aoCol) override;

   std::shared_ptr<Action> GetCopy() const override;

   void SetName(std::string aszName);
   void SetUID(std::string aszHash);
   void SetNewCard(std::string aszNewCardName);
   void SetIDs(std::vector<Tag> alstIds);
   void SetMeta(std::vector<Tag> alstMetas);

private:
   std::string m_szName;
   std::string m_szUID;
   std::string m_szNewName;
   std::vector<Tag> m_lstIdChanges;
   std::vector<Tag> m_lstMetaChanges;

   std::vector<Tag> m_lstUndoIdChanges;
   std::vector<Tag> m_lstUndoMetaChanges;

   std::shared_ptr<Action> getUndoAction(TransactionManager* aoCol) const override;
};

