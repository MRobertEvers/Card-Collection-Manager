#include "AddAction.h"
#include "Collection.h"
#include "RemoveAction.h"
#include "CollectionSource.h"
#include "CollectionObject.h"

AddAction::AddAction()
{
   m_szName = "";
}


AddAction::~AddAction()
{
}

bool 
AddAction::Execute(TransactionManager* aoCol)
{
   CopyItem* cItem = aoCol->Add(m_szName, m_lstIDs, m_lstMetas);
   
   if( cItem != nullptr )
   {
      m_szAddedUID = cItem->GetUID();
      return true;
   }

   return false;
}

bool 
AddAction::Rollback(TransactionManager* aoCol)
{
   std::shared_ptr<Action> rmAction;

   rmAction = getUndoAction(aoCol);
   return rmAction->Execute(aoCol);
}

// It is the responsibility of the caller to destroy the action.
std::shared_ptr<Action>
AddAction::getUndoAction(TransactionManager* aoCol) const
{
   std::string szHashRM;  
   Collection* refCollection = aoCol->GetCollection();
   CollectionSource* refSource = aoCol->GetSource();

   TryGet<CollectionObject> refItem = refSource->GetCardPrototype(m_szName);
   if (!refItem.Good()) { return false; }
   
   szHashRM = refItem->GenerateHash( refCollection->GetIdentifier(),
                                     m_lstIDs, m_lstMetas );

   RemoveAction* rmRetVal = new RemoveAction();
   rmRetVal->SetUID(szHashRM);
   rmRetVal->SetName(m_szName);

   return std::shared_ptr<Action>(rmRetVal);
}

std::shared_ptr<Action> 
AddAction::GetCopy() const
{
   Action* ptCopy = new AddAction(*this);
   return std::shared_ptr<Action>(ptCopy);
}

void 
AddAction::SetName(std::string aszName)
{
   m_szName = aszName;
}

void 
AddAction::SetIDs(const std::vector<Tag>& alstIDs)
{
   m_lstIDs = alstIDs;
}

void 
AddAction::SetMeta(const std::vector<Tag>& alstMetas)
{
   m_lstMetas = alstMetas;
}
