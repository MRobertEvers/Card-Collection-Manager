#pragma once
#include "../../CollectionPro/src/StoreFront.h"
#include "../../CollectionPro/src/StringInterface.h"
#include <memory>
#include <vector>
#include <map>
#include <chrono>

class wxString;
class ImageFetcherCallback;
class CollectionInterface;
class CardInterface;

class StoreFrontEnd
{
public:
   // Library Access -- Images
   bool DownloadCardImage( const wxString& aszFilePath,
                           const wxString& aszCardName,
                           const wxString& aszSet,
                           const wxString& aszMUD,
                           std::shared_ptr<ImageFetcherCallback> aptCallback );
   bool DownloadCardImage( CardInterface* apCard,
                           std::shared_ptr<ImageFetcherCallback> aptCallback );

   bool DownloadCardImage( const wxString& aszCardName,
                           const wxString& aszSet,
                           std::shared_ptr<ImageFetcherCallback> aptCallback );
   bool DownloadImportSourceFile();

   // System
   bool IsFileExist( const wxString& aszFilePath );
   void EstablishFolderStructure();
   void EstablishFolder( const wxString& aszPath );
   std::vector<wxString> GetTextFilesInDirectory( const wxString& aszDir );

   // Resources
   wxString GetAddSymbolFilePath();
   wxString GetRemSymbolFilePath();
   wxString GetSwapSymbolFilePath();
   wxString GetResourcesDirectory();
   wxString GetSpritesDirectory();

   // Collections
   std::shared_ptr<CollectionInterface> GetCollection( const wxString& aszID );

   long long StartStopWatch();
   long long EndStopWatch();

private:
   StoreFrontEnd();
   ~StoreFrontEnd();

   std::map<wxString, std::shared_ptr<CollectionInterface>> m_mapCollections;
   std::chrono::system_clock::time_point m_now;
public:
   static StoreFront* Server();
   static StoreFrontEnd* Client();

private:
   static StoreFront* m_ptInstance;
   static StoreFrontEnd* m_ptClient;
};