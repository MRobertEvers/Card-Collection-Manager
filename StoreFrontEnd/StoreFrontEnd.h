#pragma once

#include "../CollectionPro/src/StoreFront.h"
#include "../CollectionPro/src/StringInterface.h"

class wxString;

class StoreFrontEnd
{
public:
   StoreFrontEnd();
   ~StoreFrontEnd();

   bool DownloadCardImage( const wxString& aszFilePath,
                           const wxString& aszCardName,
                           const wxString& aszSet,
                           const wxString& aszMUD );


public:
   static StoreFront* Instance();
  
private:
   static StoreFront* m_ptInstance;
};