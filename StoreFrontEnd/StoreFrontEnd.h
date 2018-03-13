#pragma once
#include "../CollectionPro/src/StoreFront.h"
#include "../CollectionPro/src/StringInterface.h"
#include "../CollectionPro/src/Config.h"
#include <memory>
#include <vector>

class wxString;
class ImageFetcherCallback;

class StoreFrontEnd
{
public:
   bool DownloadCardImage( const wxString& aszFilePath,
                           const wxString& aszCardName,
                           const wxString& aszSet,
                           const wxString& aszMUD,
                           std::shared_ptr<ImageFetcherCallback> aptCallback );

   bool DownloadImportSourceFile();

   void EstablishFolderStructure();

   void EstablishFolder( const wxString& aszPath );

   std::vector<wxString> GetTextFilesInDirectory( const wxString& aszDir );

private:
   StoreFrontEnd();
   ~StoreFrontEnd();



public:
   static StoreFront* Server();
   static StoreFrontEnd* Client();

private:
   static StoreFront* m_ptInstance;
   static StoreFrontEnd* m_ptClient;
};