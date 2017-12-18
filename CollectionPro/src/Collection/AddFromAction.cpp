#include "AddFromAction.h"
#include "TransactionManager.h"
#include "Collection.h"

AddFromAction::AddFromAction()
{
}


AddFromAction::~AddFromAction()
{
}

   
bool 
AddFromAction::Execute(TransactionManager* aoCol)
{
   aoCol->AddFrom(m_szName, m_szIdentifyingHash, m_FromAddress);

   return true;
}

bool 
AddFromAction::Rollback(TransactionManager* aoCol)
{
   aoCol->Remove(m_szName, m_szIdentifyingHash);

   return true;
}

std::shared_ptr<Action> 
AddFromAction::GetCopy() const
{
   Action* ptCopy = new AddFromAction(*this);
   return std::shared_ptr<Action>(ptCopy);
}

void 
AddFromAction::SetName(std::string aszName)
{
   m_szName = aszName;
}

void 
AddFromAction::SetHash(std::string aszHash)
{
   m_szIdentifyingHash = aszHash;
}

void 
AddFromAction::SetResi(const Address& aAddress)
{
   //m_FromAddress = aAddress;
}

std::shared_ptr<Action> 
AddFromAction::getUndoAction(TransactionManager* aoCol) const
{
   return std::shared_ptr<Action>(nullptr);
}