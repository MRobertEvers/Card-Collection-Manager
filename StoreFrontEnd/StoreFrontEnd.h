#pragma once
#include <memory>
#include "../CollectionPro/src/StoreFront.h"
#include "../CollectionPro/src/StringInterface.h"

class wxString;
class ImageFetcherCallback;

class StoreFrontEnd
{
public:
   StoreFrontEnd();
   ~StoreFrontEnd();

   bool DownloadCardImage( const wxString& aszFilePath,
                           const wxString& aszCardName,
                           const wxString& aszSet,
                           const wxString& aszMUD,
                           std::shared_ptr<ImageFetcherCallback> aptCallback );


public:
   static StoreFront* Instance();
  
private:
   static StoreFront* m_ptInstance;
};