#pragma once

#include <vector>
#include <map>
#include <iterator>
#include <string>
#include <memory>



class TransactionManager;
class Action;

/* Class Transaction
* Wraps a list of actions. Provides interface for executing associated
* actions and rolling back those actions. Performs action 'on a collection'
* through the passed in TransactionManager.
*/
class Transaction
{
public:
   Transaction();
   ~Transaction();

   void AddAction(const Action& aAct);
   bool Finalize(TransactionManager* aoCol);
   bool Rollback(TransactionManager* aoCol);

   bool IsOpen();

private:
   bool m_bIsOpen;
   std::vector<std::shared_ptr<Action>> m_lstActions;
};
