#include "StoreFront.h"

StoreFront* StoreFrontEnd::m_ptInstance = nullptr;

StoreFrontEnd::StoreFrontEnd()
{
   
}

StoreFrontEnd::~StoreFrontEnd()
{

}

StoreFront*
StoreFrontEnd::Instance()
{
   if( m_ptInstance == NULL )
   {
      m_ptInstance = new StoreFront();
   }

   return m_ptInstance;
}