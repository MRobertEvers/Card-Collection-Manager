#include "ReplaceAction.h"
#include "Collection.h"

ReplaceAction::ReplaceAction()
{
}


ReplaceAction::~ReplaceAction()
{
}


bool
ReplaceAction::Execute(TransactionManager* aoCol)
{
   CollectionSource* refSource = aoCol->GetSource();
   Collection* refCollection = aoCol->GetCollection();

   // Verify the first item
   TryGet<CollectionObject> refItem = refSource->GetCardPrototype(m_szName);
   if (!refItem.Good()) { return false; }

   // Verify the second item
   TryGet<CollectionObject> refItemTwo = refSource->GetCardPrototype(m_szNewName);
   if (!refItemTwo.Good()) { return false; }

   auto refcitem = refItem->FindCopy( m_szUID );
   if (!refcitem.Good()) { return false; }

   auto copy = refcitem->get();
   m_lstUndoMetaChanges = copy->GetMetaTags(Any);
   m_lstUndoIdChanges = copy->GetIdentifyingAttributes();

   aoCol->Replace( m_szName, m_szUID, m_szNewName, 
                   m_lstIdChanges, m_lstMetaChanges );
   return true;
}

bool
ReplaceAction::Rollback(TransactionManager* aoCol)
{
   std::shared_ptr<Action> chAction;

   chAction = getUndoAction(aoCol);
   return chAction->Execute(aoCol);
}

std::shared_ptr<Action>
ReplaceAction::GetCopy() const
{
   ReplaceAction* ptCopy = new ReplaceAction(*this);
   return std::shared_ptr<Action>(ptCopy);
}

void
ReplaceAction::SetName(std::string aszName)
{
   m_szName = aszName;
}

void
ReplaceAction::SetUID(std::string aszHash)
{
   m_szUID = aszHash;
}

void
ReplaceAction::SetNewCard(std::string aszNewCardName)
{
   m_szNewName = aszNewCardName;
}

void
ReplaceAction::SetIDs(std::vector<Tag> alstIds)
{
   m_lstIdChanges = alstIds;
}

void
ReplaceAction::SetMeta(std::vector<Tag> alstMetas)
{
   m_lstMetaChanges = alstMetas;
}

std::shared_ptr<Action>
ReplaceAction::getUndoAction(TransactionManager* aoCol) const
{
   Collection* refCollection = aoCol->GetCollection();
   CollectionSource* refSource = aoCol->GetSource();

   TryGet<CollectionObject> refItem = refSource->GetCardPrototype(m_szNewName);
   if (!refItem.Good()) { return false; }
   
   std::string szHashRM;  
   szHashRM = refItem->GenerateHash( refCollection->GetIdentifier(),
                                m_lstIdChanges, m_lstMetaChanges );

   ReplaceAction* rpAction = new ReplaceAction();
   rpAction->SetName(m_szNewName);
   rpAction->SetUID(szHashRM);
   rpAction->SetIDs(m_lstUndoIdChanges);
   rpAction->SetMeta(m_lstUndoMetaChanges);
   rpAction->SetNewCard(m_szName);

   return std::shared_ptr<Action>(rpAction);
}