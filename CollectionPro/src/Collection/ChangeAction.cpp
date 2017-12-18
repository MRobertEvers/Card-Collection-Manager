#include "ChangeAction.h"
#include "Collection.h"

ChangeAction::ChangeAction()
{
}


ChangeAction::~ChangeAction()
{
}

bool 
ChangeAction::Execute(TransactionManager* aoCol) 
{
   CollectionSource* refSource = aoCol->GetSource();
   Collection* refCollection = aoCol->GetCollection();

   auto refItem = refSource->GetCardPrototype(m_szName);
   if (!refItem.Good()) { return false; }

   auto refCItem = refItem->FindCopy( m_szUID );
   if (!refCItem.Good()) { return false; }

   auto CItem = refCItem.Value()->get();
   aoCol->Change( m_szName, m_szUID, m_lstIdChanges, m_lstMetaChanges );

   m_lstUndoMeta = CItem->GetMetaTags(MetaTagType::Any);
   m_lstUndoID = CItem->GetIdentifyingAttributes();

   return true;
}

bool 
ChangeAction::Rollback(TransactionManager* aoCol)
{
   std::shared_ptr<Action> chAction;

   chAction = getUndoAction(aoCol);
   return chAction->Execute(aoCol);
}

std::shared_ptr<Action>
ChangeAction::GetCopy() const
{
   ChangeAction* ptCopy = new ChangeAction(*this);
   return std::shared_ptr<Action>((Action*)ptCopy);
}

void 
ChangeAction::SetName(const std::string& aszName)
{
   m_szName = aszName;
}
   
void 
ChangeAction::SetIDs(std::vector<Tag> alstIds)
{
   m_lstIdChanges = alstIds;
}
   
void 
ChangeAction::SetUID(const std::string& aszHash)
{
   m_szUID = aszHash;
}

void 
ChangeAction::SetMeta(std::vector<Tag> alstMeta)
{
   m_lstMetaChanges = alstMeta;
}

std::shared_ptr<Action>
ChangeAction::getUndoAction(TransactionManager* aoCol) const
{
   ChangeAction* chAction = new ChangeAction();
   chAction->SetUID(m_szUID);
   chAction->SetIDs(m_lstUndoID);
   chAction->SetMeta(m_lstUndoMeta);
   chAction->SetName(m_szName);

   return std::shared_ptr<Action>(chAction);
}