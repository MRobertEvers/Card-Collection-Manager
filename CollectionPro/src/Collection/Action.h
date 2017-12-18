#pragma once

#include <string>
#include <memory>

typedef std::pair<std::string, std::string> Tag;

enum ActionType
{
   Add,
   AddFrom,
   Remove,
   Change, 
   Replace
};

class TransactionManager;

/* Class Action (Used In Collection.cpp)
* Used to wrap an action that changes the collection with an action
* that can undo it. Allows undo button.
* Also maintains a string description of the action so that it can
* be recorded in the .history.txt file.
* This class acts as a data structure.
*/
class Action
{
public:
   Action();

   // Pointers to Action* can access a child destructor
   // only if this is virtual.
   virtual ~Action();

   virtual bool Execute(TransactionManager* aoCol) = 0;
   virtual bool Rollback(TransactionManager* aoCol) = 0;

   virtual std::shared_ptr<Action> GetCopy() const = 0;

protected:
   bool m_bCanRollBack;

   virtual std::shared_ptr<Action> getUndoAction(TransactionManager* aoCol) const = 0;
};