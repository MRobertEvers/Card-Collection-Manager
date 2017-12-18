#include "RemoveAction.h"
#include "Collection.h"

RemoveAction::RemoveAction()
{
   m_szName = "";
   m_szUID = "";
}


RemoveAction::~RemoveAction()
{
}

bool 
RemoveAction::Execute(TransactionManager* aoCol)
{
   CollectionSource* refSource = aoCol->GetSource();

   TryGet<CollectionObject> refItem = refSource->GetCardPrototype(m_szName);
   if (!refItem.Good()) { return false; }

   auto refCItem = refItem->FindCopy(m_szUID);
   if (!refCItem.Good()) { return false; }
   
   auto copy = refCItem.Value()->get();
   m_lstMetaOfRMItem = copy->GetMetaTags(MetaTagType::Any);
   m_lstTagsOfRMItem = copy->GetIdentifyingAttributes();
   
   aoCol->Remove(m_szName, m_szUID);
   return true;
}

bool 
RemoveAction::Rollback(TransactionManager* aoCol)
{
   std::shared_ptr<Action> adAction;

   adAction = getUndoAction(aoCol);
   return adAction->Execute(aoCol);
}

std::shared_ptr<Action>
RemoveAction::getUndoAction(TransactionManager* aoCol) const
{
   Collection* refCollection = aoCol->GetCollection();

   AddAction* adRetVal = new AddAction();
   adRetVal->SetIDs(m_lstTagsOfRMItem);
   adRetVal->SetMeta(m_lstMetaOfRMItem);
   adRetVal->SetName(m_szName);

   return std::shared_ptr<Action>(adRetVal);
}

std::shared_ptr<Action>
RemoveAction::GetCopy() const
{
   RemoveAction* ptCopy = new RemoveAction(*this);
   return std::shared_ptr<Action>(ptCopy);
}


void 
RemoveAction::SetName(const std::string& aszName)
{
   m_szName = aszName;
}

void 
RemoveAction::SetUID(const std::string& aszUID)
{
   m_szUID = aszUID;
}