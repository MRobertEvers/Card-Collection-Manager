#include "Transaction.h"
#include "Action.h"

Transaction::Transaction()
{
   m_bIsOpen = true;
}

Transaction::~Transaction()
{
   m_lstActions.clear();
}

void 
Transaction::AddAction(const Action& aAct)
{
   std::shared_ptr<Action> ptNewAct = aAct.GetCopy();
   m_lstActions.push_back(ptNewAct);
}

bool 
Transaction::Finalize(TransactionManager* aoCol)
{
   bool bAllSuccess = true;

   std::vector<std::shared_ptr<Action>>::iterator iter_Action;
   for (iter_Action  = m_lstActions.begin();
        iter_Action != m_lstActions.end();
        ++iter_Action )
   {
      Action* action = iter_Action->get();
      bAllSuccess |= action->Execute(aoCol);
   }

   m_bIsOpen = false;

   return bAllSuccess;
}

bool 
Transaction::Rollback(TransactionManager* aoCol)
{
   bool bAllSuccess = true;

   std::vector<std::shared_ptr<Action>>::reverse_iterator iter_Action;
   for (iter_Action  = m_lstActions.rbegin();
        iter_Action != m_lstActions.rend();
        ++iter_Action )
   {
      Action* action = iter_Action->get();
      bAllSuccess |= action->Rollback(aoCol);
   }

   m_bIsOpen = true;

   return bAllSuccess;
}

bool 
Transaction::IsOpen()
{
   return m_bIsOpen;
}