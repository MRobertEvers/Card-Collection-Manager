#pragma once

#include "../CollectionPro/src/StoreFront.h"

class StoreFrontEnd
{
public:
   StoreFrontEnd();
   ~StoreFrontEnd();

public:
   static StoreFront* Instance();
  
private:
   static StoreFront* m_ptInstance;
};